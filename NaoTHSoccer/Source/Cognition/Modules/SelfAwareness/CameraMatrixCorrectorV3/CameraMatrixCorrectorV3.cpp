/**
* @file CameraMatrixCorrectorV3.cpp
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/

#include "CameraMatrixCorrectorV3.h"
#include <array>
#include <algorithm>

CameraMatrixCorrectorV3::CameraMatrixCorrectorV3():
    theCamMatErrorFunctionV3(getDebugRequest(), getDebugDrawings(), getDebugModify(), getFieldInfo(), getCameraInfo(), getCameraInfoTop()),
    gn_minimizer(1, 1.25, 0.005, /*double regularizer,*/ false)
{
  getDebugParameterList().add(&getCameraMatrixOffset());
  getDebugParameterList().add(&cmc_params);

  bounds.lower_bound << cmc_params.lower.body_x,
           cmc_params.lower.body_y,
           cmc_params.lower.head_x,
           cmc_params.lower.head_y,
           cmc_params.lower.head_z,
           cmc_params.lower.cam_x,
           cmc_params.lower.cam_y,
           cmc_params.lower.cam_z,
           cmc_params.lower.cam_top_x,
           cmc_params.lower.cam_top_y,
           cmc_params.lower.cam_top_z;

  bounds.upper_bound << cmc_params.upper.body_x,
           cmc_params.upper.body_y,
           cmc_params.upper.head_x,
           cmc_params.upper.head_y,
           cmc_params.upper.head_z,
           cmc_params.upper.cam_x,
           cmc_params.upper.cam_y,
           cmc_params.upper.cam_z,
           cmc_params.upper.cam_top_x,
           cmc_params.upper.cam_top_y,
           cmc_params.upper.cam_top_z;

  if(cmc_params.use_bounded_variable){
    theCamMatErrorFunctionV3.bounds = &bounds;
  } else {
    theCamMatErrorFunctionV3.bounds = nullptr;
  }

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:automatic_mode","try to do automatic calibration", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:manual:collect_calibration_data", "collect the data for calibration", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:manual:calibrate","",false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:manual:clear_calibration_data", "clears the data used for calibration", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:manual:reset_calibration", "set the calibration offsets of the CM to 0", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:manual:reset_minimizer", "reset lm parameters to initial values", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:debug:enable_CamMatErrorFunctionV3_drawings", "needed to be activated for error function drawings", true);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:debug:optimizer:use_GN",  "use Gauss-Newton based optimizer during calibration (default)", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:debug:optimizer:use_LM",  "use Levenberg-Marquardt based optimizer during calibration", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:debug:optimizer:use_LM2", "use Levenberg-Marquardt based optimizer (smoothed update) during calibration", true);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:debug:read_calibration_data_from_file", "", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:debug:write_calibration_data_to_file",  "", false);

  theCamMatErrorFunctionV3.global_position = &cmc_params.global_pose.position;
  theCamMatErrorFunctionV3.global_orientation = &cmc_params.global_pose.orientation;

  read_calibration_data = written_calibration_data = false;
  auto_cleared_data = auto_collected = auto_calibrated = false;
  play_calibrated = play_calibrating = play_collecting = true;
  last_error = 0;

  minimizer = &lm2_minimizer;

  // sampling coordinates
  std::array<double,4> pitchs {-20, -10 , 0, 10};
  std::vector<double> yaws;
  for (int i = -88; i <= 88; i=i+8) {
      yaws.push_back(i);
  }

  for(double pitch : pitchs) 
  {
      for(double yaw : yaws) {
          target_points.push_back(Vector2d(yaw, pitch));
      }
      // reverse the order for the next iteration
      std::reverse(yaws.begin(), yaws.end());
  }

  current_target = target_points.begin();

  readFromRepresentation();
}

CameraMatrixCorrectorV3::~CameraMatrixCorrectorV3()
{
    getDebugParameterList().remove(&getCameraMatrixOffset());
    getDebugParameterList().remove(&cmc_params);
}

void CameraMatrixCorrectorV3::execute()
{
  if(cmc_params.check_changed()){
      update_bounds();
  }

  DEBUG_REQUEST("CameraMatrixV3:debug:read_calibration_data_from_file",
    if(!read_calibration_data) {
      theCamMatErrorFunctionV3.read_calibration_data_from_file();
      read_calibration_data = true;
    }
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:debug:read_calibration_data_from_file",
    read_calibration_data = false;
  );

  DEBUG_REQUEST("CameraMatrixV3:debug:write_calibration_data_to_file",
    if(!written_calibration_data) {
      theCamMatErrorFunctionV3.write_calibration_data_to_file();
      written_calibration_data = true;
    }
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:debug:write_calibration_data_to_file",
    written_calibration_data = false;
  );

  DEBUG_REQUEST("CameraMatrixV3:debug:optimizer:use_GN",
    minimizer = &gn_minimizer;
  );

  DEBUG_REQUEST("CameraMatrixV3:debug:optimizer:use_LM",
    minimizer = &lm_minimizer;
  );

  DEBUG_REQUEST("CameraMatrixV3:debug:optimizer:use_LM2",
    minimizer = &lm2_minimizer;
  );

  if (getSoundPlayData().soundFile == "finished.wav"
      || getSoundPlayData().soundFile == "collectingdata.wav"
      || getSoundPlayData().soundFile == "calibrating.wav")
  {
    getSoundPlayData().mute = true;
    getSoundPlayData().soundFile = "";
  }

  // sit down if auto calibrated
  if(auto_calibrated){
    getMotionRequest().id = motion::sit;
    getMotionRequest().disable_relaxed_stand = false;
  } else {
    getMotionRequest().id = motion::stand;
    getMotionRequest().disable_relaxed_stand = true;
  }

  // enable debug drawings in manual and auto mode
  DEBUG_REQUEST("CameraMatrixV3:debug:enable_CamMatErrorFunctionV3_drawings",
      theCamMatErrorFunctionV3.plot_CalibrationData(cam_mat_offsets);
  );

  bool use_automatic_mode = false;
  DEBUG_REQUEST("CameraMatrixV3:automatic_mode",
    use_automatic_mode = true;
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:automatic_mode",
    // Note: wanted behavior because we want to save the "best" solution so far
    if(!auto_calibrated){
        writeToRepresentation();
        getCameraMatrixOffset().saveToConfig();
    }

    // reset to intial
    getHeadMotionRequest().id = HeadMotionRequest::hold;
    current_target = target_points.begin();

    auto_cleared_data = auto_collected = auto_calibrated = false;
    play_calibrated = play_calibrating = play_collecting = true;
    derrors.clear();
    last_error = 0;
    minimizer->reset();
  );

  DEBUG_REQUEST("CameraMatrixV3:manual:reset_minimizer",
        minimizer->reset();
  );

  if(use_automatic_mode){
      if(!auto_calibrated){
          doItAutomatically();
      }
  } else { // manual mode
      DEBUG_REQUEST("CameraMatrixV3:manual:clear_calibration_data",
        theCamMatErrorFunctionV3.clear();
      );

      DEBUG_REQUEST("CameraMatrixV3:manual:collect_calibration_data",
        collectingData();
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:manual:collect_calibration_data",
        // reset to initial
        getHeadMotionRequest().id = HeadMotionRequest::hold;
        current_target = target_points.begin();
      );

      DEBUG_REQUEST("CameraMatrixV3:manual:calibrate",
          calibrate();
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:manual:calibrate",
          writeToRepresentation();
          getCameraMatrixOffset().saveToConfig();
      );

      DEBUG_REQUEST("CameraMatrixV3:manual:reset_calibration",
            reset_calibration();
            minimizer->reset();
            derrors.clear();
            last_error = 0;
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:manual:reset_calibration",
          writeToRepresentation();
          getCameraMatrixOffset().saveToConfig();
      );
  }

  last_frame_info = getFrameInfo();
}

void CameraMatrixCorrectorV3::reset_calibration()
{
  getCameraMatrixOffset().body_rot = Vector2d();
  getCameraMatrixOffset().head_rot = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] = Vector3d();

  cam_mat_offsets = Parameter::Zero();
}

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV3::calibrate()
{
  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  Eigen::Matrix<double, 11, 1> epsilon = Eigen::Matrix<double, 11, 1>::Constant(1e-4);

  Parameter offset;
  bool valid;
  if(cmc_params.use_bounded_variable){
      Parameter bounded_cam_mat_offsets = bounds.bound(cam_mat_offsets);
      valid = minimizer->minimizeOneStep(theCamMatErrorFunctionV3, bounded_cam_mat_offsets, epsilon, offset);
  } else {
      valid = minimizer->minimizeOneStep(theCamMatErrorFunctionV3, cam_mat_offsets, epsilon, offset);
  }

  if(valid) {
      if(cmc_params.use_bounded_variable){
          Parameter bounded_cam_mat_offsets = bounds.bound(cam_mat_offsets);
          cam_mat_offsets = bounds.unbound(bounded_cam_mat_offsets + offset);
      } else {
          cam_mat_offsets += offset;
      }
  }

  double de_dt = (minimizer->error - last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors.add(de_dt);
  }
  last_error = minimizer->error;

  PLOT("CameraMatrixV3:error", minimizer->error);
  PLOT("CameraMatrixV3:de/dt", de_dt);
  PLOT("CameraMatrixV3:avg_derror", derrors.getAverage());
  writeToRepresentation();

  return ((  (derrors.getAverage() > -50) // average error decreases by less than this per second
           && derrors.isFull())
           || minimizer->step_failed());          // and we have a full history
}

// returns true, if the trajectory is starting again from beginning
bool CameraMatrixCorrectorV3::collectingData()
{
    if(current_target == target_points.end())
        return true;

    getHeadMotionRequest().id = HeadMotionRequest::goto_angle;
    getHeadMotionRequest().targetJointPosition.x = Math::fromDegrees(current_target->x);
    getHeadMotionRequest().targetJointPosition.y = Math::fromDegrees(current_target->y);
    getHeadMotionRequest().velocity = 20;
    MODIFY("CameraMatrixV3:collecting_velocity", getHeadMotionRequest().velocity);

    double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();
    double current_yaw   = Math::toDegrees(getSensorJointData().position[JointData::HeadYaw]);
    double current_pitch = Math::toDegrees(getSensorJointData().position[JointData::HeadPitch]);

    static double last_yaw(current_yaw);
    static double last_pitch(current_pitch);

    double vel_yaw   = (last_yaw   - current_yaw)/dt;
    double vel_pitch = (last_pitch - current_pitch)/dt;

    last_yaw   = current_yaw;
    last_pitch = current_pitch;

    // state transitions and triggering sampling
    bool target_reached =     fabs(current_yaw   - current_target->x) < 3
                           && fabs(current_pitch - current_target->y) < 3
                           && fabs(vel_yaw)   < 0.2
                           && fabs(vel_pitch) < 0.2;

    if(target_reached && current_target != target_points.end()){
        sampling();
        current_target++;
    }

    PLOT("CameraMatrixV3:vel_yaw",vel_yaw);
    PLOT("CameraMatrixV3:vel_pitch", vel_pitch);
    PLOT("CameraMatrixV3:target_reached", target_reached);

    return current_target == target_points.end();
}

void CameraMatrixCorrectorV3::sampling()
{
    LineGraphPercept lineGraphPercept(getLineGraphPercept());

    // TODO: ignore upper image if the center is above the horizon
    // HACK: ignore it if headpitch is smaller than -15
    //if(head_state == look_right_up || head_state == look_left_up
    //   || last_head_state == look_right_up || last_head_state == look_left_up){
    if(Math::toDegrees(getSensorJointData().position[JointData::HeadPitch]) < -15) {
        lineGraphPercept.edgelsInImageTop.clear();
    }

    theCamMatErrorFunctionV3.add(CamMatErrorFunctionV3::CalibrationDataSample(getKinematicChain().theLinks[KinematicChain::Torso].M,
                                                                lineGraphPercept,
                                                                getInertialModel().orientation,
                                                                getSensorJointData().position[JointData::HeadYaw],
                                                                getSensorJointData().position[JointData::HeadPitch]
                                                               ));
}

void CameraMatrixCorrectorV3::doItAutomatically()
{
    if(!auto_cleared_data) {
        theCamMatErrorFunctionV3.clear();
        readFromRepresentation();
        auto_cleared_data = true;
    }

    if(!auto_collected){
        auto_collected = collectingData();

        if(play_collecting){
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "collectingdata.wav";
            play_collecting = false;
        }
    }

    if(auto_collected && !auto_calibrated){
        auto_calibrated = calibrate();
        if(auto_calibrated){
            derrors.clear();
            minimizer->reset();
        }

        if(play_calibrating){
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "calibrating.wav";
            play_calibrating = false;
        }
    }

    if(auto_calibrated){
        writeToRepresentation();
        getCameraMatrixOffset().saveToConfig();

        if(play_calibrated) {
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "finished.wav";
            play_calibrated = false;
        }
    }
}

void CameraMatrixCorrectorV3::writeToRepresentation()
{
  getCameraMatrixOffset().body_rot = Vector2d(cam_mat_offsets(0),cam_mat_offsets(1));
  getCameraMatrixOffset().head_rot = Vector3d(cam_mat_offsets(2),cam_mat_offsets(3),cam_mat_offsets(4));

  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    = Vector3d(cam_mat_offsets(5),cam_mat_offsets(6),cam_mat_offsets(7));
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] = Vector3d(cam_mat_offsets(8),cam_mat_offsets(9),cam_mat_offsets(10));
}

void CameraMatrixCorrectorV3::readFromRepresentation(){
  cam_mat_offsets << getCameraMatrixOffset().body_rot.x,
                     getCameraMatrixOffset().body_rot.y,
                     getCameraMatrixOffset().head_rot.x,
                     getCameraMatrixOffset().head_rot.y,
                     getCameraMatrixOffset().head_rot.z,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Top].x,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Top].y,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Top].z,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Bottom].x,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Bottom].y,
                     getCameraMatrixOffset().cam_rot[CameraInfo::Bottom].z;
}

void CameraMatrixCorrectorV3::update_bounds(){
  bounds.lower_bound << cmc_params.lower.body_x,
           cmc_params.lower.body_y,
           cmc_params.lower.head_x,
           cmc_params.lower.head_y,
           cmc_params.lower.head_z,
           cmc_params.lower.cam_x,
           cmc_params.lower.cam_y,
           cmc_params.lower.cam_z,
           cmc_params.lower.cam_top_x,
           cmc_params.lower.cam_top_y,
           cmc_params.lower.cam_top_z;

  bounds.upper_bound << cmc_params.upper.body_x,
           cmc_params.upper.body_y,
           cmc_params.upper.head_x,
           cmc_params.upper.head_y,
           cmc_params.upper.head_z,
           cmc_params.upper.cam_x,
           cmc_params.upper.cam_y,
           cmc_params.upper.cam_z,
           cmc_params.upper.cam_top_x,
           cmc_params.upper.cam_top_y,
           cmc_params.upper.cam_top_z;

  if(cmc_params.use_bounded_variable){
    theCamMatErrorFunctionV3.bounds = &bounds;
  } else {
    theCamMatErrorFunctionV3.bounds = nullptr;
  }
}
