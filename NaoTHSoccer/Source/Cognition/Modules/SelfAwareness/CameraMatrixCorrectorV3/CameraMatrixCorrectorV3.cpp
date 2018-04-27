/**
* @file CameraMatrixCorrectorV3.cpp
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
*/

#include "CameraMatrixCorrectorV3.h"
#include <array>

CameraMatrixCorrectorV3::CameraMatrixCorrectorV3()
{
  getDebugParameterList().add(&getCameraMatrixOffsetV3());

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:calibrate_camera_matrix_line_matching",
    "calculates the roll and tilt offset of the camera using field lines (it. shoult be exactely 3000mm in front of the robot)",
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:reset_calibration", "set the calibration offsets of the CM to 0", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:reset_lm_minimizer", "reset lm parameters to initial values", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:collect_calibration_data", "collect the data for calibration", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:clear_calibration_data", "clears the data used for calibration", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:enable_CamMatErrorFunctionV3_drawings", "needed to be activated for error function drawings", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:automatic_mode","try to do automatic calibration", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV3:calibrateOnlyPos","",false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:calibrateOnlyOffsets","",false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV3:calibrateSimultaneously","",false);

  theCamMatErrorFunctionV3 = registerModule<CamMatErrorFunctionV3>("CamMatErrorFunctionV3", true);

  auto_cleared_data = auto_collected = auto_calibrated_phase1 = auto_calibrated = false;
  play_calibrated = play_calibrating = play_collecting = true;
  last_error = 0;

  // sampling coordinates
  std::array<double,4> pitchs {-20, -10 , 0, 10};
  std::vector<double> yaws;
  for (int i = -88; i <= 88; i=i+8) {
      yaws.push_back(i);
  }

  for(double pitch : pitchs){
      for(double yaw : yaws){
          target_points.push_back(Vector2d(yaw, pitch));
      }
  }

  current_target = target_points.begin();

  readFromRepresentation();
}

CameraMatrixCorrectorV3::~CameraMatrixCorrectorV3()
{
    getDebugParameterList().remove(&getCameraMatrixOffsetV3());
}

void CameraMatrixCorrectorV3::execute()
{
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
  } else {
    getMotionRequest().id = motion::stand;
  }

  // enable debug drawings in manual and auto mode
  DEBUG_REQUEST("CameraMatrixV3:enable_CamMatErrorFunctionV3_drawings",
      theCamMatErrorFunctionV3->getModuleT()->plot_CalibrationData(cam_mat_offsets);
  );

  bool use_automatic_mode = false;
  DEBUG_REQUEST("CameraMatrixV3:automatic_mode",
    use_automatic_mode = true;
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:automatic_mode",
    // Note: wanted behavior because we want to save the "best" solution so far
    if(!auto_calibrated){
        writeToRepresentation();
        getCameraMatrixOffsetV3().saveToConfig();
    }

    // reset to intial
    getHeadMotionRequest().id = HeadMotionRequest::hold;
    current_target = target_points.begin();

    auto_cleared_data = auto_collected = auto_calibrated_phase1 = auto_calibrated = false;
    play_calibrated = play_calibrating = play_collecting = true;
    derrors.clear();
    derrors_pos.clear();
    derrors_offset.clear();
    last_error = 0;
    lm_minimizer.reset();
    lm_minimizer_pos.reset();
    lm_minimizer_offset.reset();
  );

  DEBUG_REQUEST("CameraMatrixV3:reset_lm_minimizer",
        lm_minimizer.reset();
        lm_minimizer_pos.reset();
        lm_minimizer_offset.reset();
  );

  if(use_automatic_mode){
      if(!auto_calibrated){
          doItAutomatically();
      }
  } else { // manual mode
      DEBUG_REQUEST("CameraMatrixV3:clear_calibration_data",
        (theCamMatErrorFunctionV3->getModuleT())->clear();
      );

      DEBUG_REQUEST("CameraMatrixV3:collect_calibration_data",
        collectingData();
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:collect_calibration_data",
        // reset to initial
        getHeadMotionRequest().id = HeadMotionRequest::hold;
        current_target = target_points.begin();
      );

      DEBUG_REQUEST("CameraMatrixV3:calibrate_camera_matrix_line_matching",
          calibrate();
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:calibrate_camera_matrix_line_matching",
          writeToRepresentation();
          getCameraMatrixOffsetV3().saveToConfig();
      );

      DEBUG_REQUEST("CameraMatrixV3:reset_calibration",
            reset_calibration();
            lm_minimizer.reset();
            lm_minimizer_pos.reset();
            lm_minimizer_offset.reset();
            derrors.clear();
            derrors_offset.clear();
            derrors_pos.clear();
            last_error = 0;
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV3:reset_calibration",
          writeToRepresentation();
          getCameraMatrixOffsetV3().saveToConfig();
      );
  }

  last_frame_info = getFrameInfo();
}//end execute

void CameraMatrixCorrectorV3::reset_calibration()
{
  getCameraMatrixOffsetV3().body_rot = Vector2d();
  getCameraMatrixOffsetV3().head_rot = Vector3d();
  getCameraMatrixOffsetV3().cam_rot[CameraInfo::Top]    = Vector3d();
  getCameraMatrixOffsetV3().cam_rot[CameraInfo::Bottom] = Vector3d();

  cam_mat_offsets = Eigen::Matrix<double, 14, 1>::Zero();

  //theCamMatErrorFunctionV3->getModuleT()->globalPosition = Vector3d();
}

bool CameraMatrixCorrectorV3::calibrate(){
    DEBUG_REQUEST("CameraMatrixV3:calibrateSimultaneously",
                  return calibrateSimultaneously();
                  );

    DEBUG_REQUEST("CameraMatrixV3:calibrateOnlyPos",
                  return calibrateOnlyPose();
                  );

    DEBUG_REQUEST("CameraMatrixV3:calibrateOnlyOffsets",
                  return calibrateOnlyOffsets();
                  );

    return false;
}

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV3::calibrateSimultaneously() {
  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  // calibrate the camera matrix
  Eigen::Matrix<double, 14, 1> epsilon = 1e-4*Eigen::Matrix<double, 14, 1>::Constant(1);
  epsilon(11) = 1e2;
  epsilon(12) = 1e2;
  epsilon(13) = Math::pi_4/2;

  double error;

  Eigen::Matrix<double, 14, 1> offset = lm_minimizer.minimizeOneStep(*(theCamMatErrorFunctionV3->getModuleT()), cam_mat_offsets, epsilon, error);

  double de_dt = (error-last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors.add(de_dt);
  }
  last_error = error;

  PLOT("CameraMatrixV3:error", error);
  PLOT("CameraMatrixV3:de/dt", de_dt);
  PLOT("CameraMatrixV3:avg_derror", derrors.getAverage());

  // update offsets
  cam_mat_offsets += offset;

  return (    (derrors.getAverage() > -50) // average error decreases by less than this per second
           && derrors.isFull() );          // and we have a full history
}//end calibrate

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV3::calibrateOnlyPose()
{
  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  // calibrate the camera matrix
  Eigen::Matrix<double, 14, 1> epsilon = Eigen::Matrix<double, 14, 1>::Constant(std::numeric_limits<double>::quiet_NaN());

  double epsilon_pos = 1e2;
  double epsilon_angle = Math::pi_4/2;

  MODIFY("CameraMatrixV3:epsilon:pos", epsilon_pos);
  MODIFY("CameraMatrixV3:epsilon:angle", epsilon_angle);

  epsilon.block<3,1>(11,0) << epsilon_pos, epsilon_pos, epsilon_angle;
  double error;

  Eigen::Matrix<double, 14, 1> offset = lm_minimizer_pos.minimizeOneStep(*(theCamMatErrorFunctionV3->getModuleT()), cam_mat_offsets, epsilon, error);

  double de_dt = (error-last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors_pos.add(de_dt);
  }
  last_error = error;

  PLOT("CameraMatrixV3:error", error);
  PLOT("CameraMatrixV3:de/dt", de_dt);
  PLOT("CameraMatrixV3:avg_derror_pose", derrors_pos.getAverage());

  // update offsets
  cam_mat_offsets += offset;

  return (    (derrors_pos.getAverage() > -50) // average error decreases by less than this per second
           && derrors_pos.isFull() );          // and we have a full history
}//end calibrate

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV3::calibrateOnlyOffsets()
{
  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  // calibrate the camera matrix
  Eigen::Matrix<double, 14, 1> epsilon = Eigen::Matrix<double, 14, 1>::Constant(1e-4);
  epsilon.block<3,1>(11,0) << std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN();
  double error;

  Eigen::Matrix<double, 14, 1> offset = lm_minimizer_offset.minimizeOneStep(*(theCamMatErrorFunctionV3->getModuleT()), cam_mat_offsets, epsilon, error);

  double de_dt = (error-last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors_offset.add(de_dt);
  }
  last_error = error;

  PLOT("CameraMatrixV3:error", error);
  PLOT("CameraMatrixV3:de/dt", de_dt);
  PLOT("CameraMatrixV3:avg_derror_offset", derrors_offset.getAverage());

  // update offsets
  cam_mat_offsets += offset;

  return (    (derrors_offset.getAverage() > -50) // average error decreases by less than this per second
           && derrors_offset.isFull() );          // and we have a full history
}//end calibrate

// returns true, if the trajectory is starting again from beginning
bool CameraMatrixCorrectorV3::collectingData()
{
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

    (theCamMatErrorFunctionV3->getModuleT())->add(CamMatErrorFunctionV3::CalibrationDataSample(getKinematicChain().theLinks[KinematicChain::Torso].M,
                                                                lineGraphPercept,
                                                                getInertialModel(),
                                                                getSensorJointData().position[JointData::HeadYaw],
                                                                getSensorJointData().position[JointData::HeadPitch]
                                                               ));
}

void CameraMatrixCorrectorV3::doItAutomatically()
{
    if(!auto_cleared_data) {
        (theCamMatErrorFunctionV3->getModuleT())->clear();
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

    if(auto_collected && !auto_calibrated_phase1){
        auto_calibrated_phase1 = calibrateSimultaneously();
        if(auto_calibrated_phase1){
            derrors_offset.clear();
            lm_minimizer_offset.reset();
        }

        if(play_calibrating){
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "calibrating.wav";
            play_calibrating = false;
        }
    }

    if(auto_calibrated_phase1 && !auto_calibrated){
        auto_calibrated = calibrateOnlyOffsets();
    }

    if(auto_calibrated){
        writeToRepresentation();
        getCameraMatrixOffsetV3().saveToConfig();

        if(play_calibrated) {
            getSoundPlayData().mute = false;
            getSoundPlayData().soundFile = "finished.wav";
            play_calibrated = false;
        }
    }
}

void CameraMatrixCorrectorV3::writeToRepresentation()
{
  getCameraMatrixOffsetV3().body_rot += Vector2d(cam_mat_offsets(0),cam_mat_offsets(1));
  getCameraMatrixOffsetV3().head_rot += Vector3d(cam_mat_offsets(2),cam_mat_offsets(3),cam_mat_offsets(4));

  getCameraMatrixOffsetV3().cam_rot[CameraInfo::Top]    += Vector3d(cam_mat_offsets(5),cam_mat_offsets(6),cam_mat_offsets(7));
  getCameraMatrixOffsetV3().cam_rot[CameraInfo::Bottom] += Vector3d(cam_mat_offsets(8),cam_mat_offsets(9),cam_mat_offsets(10));

  getCameraMatrixOffsetV3().global_pose.translation += Vector2d(cam_mat_offsets(11),cam_mat_offsets(12));
  getCameraMatrixOffsetV3().global_pose.rotation    += cam_mat_offsets(13);
}

void CameraMatrixCorrectorV3::readFromRepresentation(){
  cam_mat_offsets << getCameraMatrixOffsetV3().body_rot.x,
                     getCameraMatrixOffsetV3().body_rot.y,
                     getCameraMatrixOffsetV3().head_rot.x,
                     getCameraMatrixOffsetV3().head_rot.y,
                     getCameraMatrixOffsetV3().head_rot.z,
                     getCameraMatrixOffsetV3().cam_rot[CameraInfo::Top].x,
                     getCameraMatrixOffsetV3().cam_rot[CameraInfo::Top].y,
                     getCameraMatrixOffsetV3().cam_rot[CameraInfo::Top].z,
                     getCameraMatrixOffsetV3().cam_rot[CameraInfo::Bottom].x,
                     getCameraMatrixOffsetV3().cam_rot[CameraInfo::Bottom].y,
                     getCameraMatrixOffsetV3().cam_rot[CameraInfo::Bottom].z,
                     0, //x
                     0, //y
                     0; //z
}
