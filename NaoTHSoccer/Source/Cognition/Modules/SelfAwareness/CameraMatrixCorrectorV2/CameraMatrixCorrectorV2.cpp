/**
* @file CameraMatrixProvider.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixProvider
*/

#include "CameraMatrixCorrectorV2.h"

CameraMatrixCorrectorV2::CameraMatrixCorrectorV2()
{
  getDebugParameterList().add(&getCameraMatrixOffset());

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:calibrate_camera_matrix_line_matching",
    "calculates the roll and tilt offset of the camera using field lines (it. shoult be exactely 3000mm in front of the robot)",
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:reset_calibration", "set the calibration offsets of the CM to 0", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:collect_calibration_data", "collect the data for calibration", false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV2:clear_calibration_data", "clears the data used for calibration", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:enable_CamMatErrorFunction_drawings", "needed to be activated for error function drawings", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:automatic_mode","try to do automatic calibration", true);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:calibrateOnlyPos","",false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV2:calibrateOnlyOffsets","",false);

  theCamMatErrorFunction = registerModule<CamMatErrorFunction>("CamMatErrorFunction", true);
  last_idx_yaw   = 0;
  last_idx_pitch = 0;
  damping_cam_offsets = 0.005;
  damping_pose = 0.01;

  head_state      = look_left;
  last_head_state = initial;

  auto_cleared_data = auto_collected = auto_calibrated = false;
  last_error = 0;
}

CameraMatrixCorrectorV2::~CameraMatrixCorrectorV2()
{
}

void CameraMatrixCorrectorV2::execute()
{
  // sit down if auto calibrated
  if(auto_calibrated){
    getMotionRequest().id = motion::sit;
  } else {
    getMotionRequest().id = motion::stand;
  }

  // enable debug drawings in manual and auto mode
  DEBUG_REQUEST("CameraMatrixV2:enable_CamMatErrorFunction_drawings",
      theCamMatErrorFunction->getModuleT()->plot_CalibrationData();
  );

  bool use_automatic_mode = false;
  DEBUG_REQUEST("CameraMatrixV2:automatic_mode",
    use_automatic_mode = true;
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:automatic_mode",
    // Note: wanted behavior because we want to save the "best" solution so far
    if(!auto_calibrated){
        getCameraMatrixOffset().saveToConfig();
    }

    // reset to intial
    head_state = look_left;
    last_head_state = initial;
    getHeadMotionRequest().id = HeadMotionRequest::hold;

    auto_cleared_data = auto_collected = auto_calibrated = false;
    derrors.clear();
    last_error = 0;
  );

  if(use_automatic_mode){
      if(!auto_calibrated){
          doItAutomatically();
      }
  } else { // manual mode
      DEBUG_REQUEST("CameraMatrixV2:clear_calibration_data",
        (theCamMatErrorFunction->getModuleT())->clear();
      );

      bool collect_data = false;
      DEBUG_REQUEST("CameraMatrixV2:collect_calibration_data",
        collect_data = true;
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:collect_calibration_data",
        // reset to initial
        head_state = look_left;
        last_head_state = initial;
        getHeadMotionRequest().id = HeadMotionRequest::hold;
      );

      if(collect_data){
          movingHead();
          collectingData();
      }

      MODIFY("CameraMatrixV2:OffsetRollTop",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Top].x);
      MODIFY("CameraMatrixV2:OffsetTiltTop",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Top].y);
      MODIFY("CameraMatrixV2:OffsetRollBottom",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Bottom].x);
      MODIFY("CameraMatrixV2:OffsetTiltBottom",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Bottom].y);

      MODIFY("CameraMatrixV2:Body:Roll",  getCameraMatrixOffset().body_rot.x);
      MODIFY("CameraMatrixV2:Body:Pitch", getCameraMatrixOffset().body_rot.y);

      MODIFY("CameraMatrixV2:Head:Roll",  getCameraMatrixOffset().head_rot.x);
      MODIFY("CameraMatrixV2:Head:Pitch", getCameraMatrixOffset().head_rot.y);
      MODIFY("CameraMatrixV2:Head:Yaw",   getCameraMatrixOffset().head_rot.z);

      MODIFY("CameraMatrixV2:Cam:Top:Roll",  getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Top].x);
      MODIFY("CameraMatrixV2:Cam:Top:Pitch", getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Top].y);
      MODIFY("CameraMatrixV2:Cam:Top:Yaw",   getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Top].z);

      MODIFY("CameraMatrixV2:Cam:Bottom:Roll",  getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Bottom].x);
      MODIFY("CameraMatrixV2:Cam:Bottom:Pitch", getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Bottom].y);
      MODIFY("CameraMatrixV2:Cam:Bottom:Yaw",   getCameraMatrixOffset().cam_rot[naoth::CameraInfo::Bottom].z);

      MODIFY("CameraMatrixV2:GlobalPosition:x",        theCamMatErrorFunction->getModuleT()->globalPosition.x);
      MODIFY("CameraMatrixV2:GlobalPosition:y",        theCamMatErrorFunction->getModuleT()->globalPosition.y);
      MODIFY("CameraMatrixV2:GlobalPosition:z_angle",  theCamMatErrorFunction->getModuleT()->globalPosition.z);

      DEBUG_REQUEST("CameraMatrixV2:calibrate_camera_matrix_line_matching",
          calibrate();
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:calibrate_camera_matrix_line_matching",
          getCameraMatrixOffset().saveToConfig();
      );

      DEBUG_REQUEST("CameraMatrixV2:reset_calibration",
            reset_calibration();
            derrors.clear();
            last_error = 0;
      );

      DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:reset_calibration",
          getCameraMatrixOffset().saveToConfig();
      );
  }

  last_frame_info = getFrameInfo();
}//end execute

void CameraMatrixCorrectorV2::reset_calibration()
{
  getCameraMatrixOffset().body_rot = Vector2d();
  getCameraMatrixOffset().head_rot = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] = Vector3d();

  theCamMatErrorFunction->getModuleT()->globalPosition = Vector3d();
}

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV2::calibrate()
{
    DEBUG_REQUEST("CameraMatrixV2:calibrateOnlyPos",
                  return calibrateOnlyPose();
                  );

    DEBUG_REQUEST("CameraMatrixV2:calibrateOnlyOffsets",
                  return calibrateOnlyOffsets();
                  );

  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  // calibrate the camera matrix
  Eigen::Matrix<double, 14, 1> epsilon = 1e-4*Eigen::Matrix<double, 14, 1>::Constant(1);
  epsilon(11) = 1e2;
  epsilon(12) = 1e2;
  epsilon(13) = Math::pi_4/2;

  std::tuple<Eigen::Matrix<double, 14, 1>, double> r_val = gn_minimizer.minimizeOneStep(*(theCamMatErrorFunction->getModuleT()),epsilon);

  Eigen::Matrix<double, 14, 1> offset = std::get<0>(r_val);
  double error = std::get<1>(r_val);
  double de_dt = (error-last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors.add(de_dt);
  }
  last_error = error;

  PLOT("CameraMatrixV2:error", error);
  PLOT("CameraMatrixV2:de/dt", de_dt);
  PLOT("CameraMatrixV2:avg_derror", derrors.getAverage());

  // TODO: remove damping in gauss newtwon (prefered in the one step) or this one
  MODIFY("CameraMatrixV2:damping:calibrate_only_pose:angle", damping_pose);
  double damping_translation = damping_pose;
  MODIFY("CameraMatrixV2:damping:calibrate_only_pose:location", damping_translation);
  MODIFY("CameraMatrixV2:damping:calibrate_only_offsets", damping_cam_offsets);

  // update offsets
  getCameraMatrixOffset().body_rot += Vector2d(offset(0),offset(1)) * damping_cam_offsets;
  getCameraMatrixOffset().head_rot += Vector3d(offset(2),offset(3),offset(4)) * damping_cam_offsets;
  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    += Vector3d(offset(5),offset(6),offset(7))  * damping_cam_offsets;
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] += Vector3d(offset(8),offset(9),offset(10)) * damping_cam_offsets;
  (theCamMatErrorFunction->getModuleT())->globalPosition.x += offset(11) * damping_pose; // scaling
  (theCamMatErrorFunction->getModuleT())->globalPosition.y += offset(12) * damping_pose; // scaling
  (theCamMatErrorFunction->getModuleT())->globalPosition.z += offset(13) * damping_pose; // scaling

  return (    (derrors.getAverage() > -50) // average error decreases by less than this per second
           && (derrors.getAverage() < 0)   // but it decreases
           && (de_dt < 0)                  // and the last step was an improvement
           && derrors.isFull() );          // and we have a full history
}//end calibrate

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV2::calibrateOnlyPose()
{
  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  // calibrate the camera matrix
  Eigen::Matrix<double, 3, 1> offset;
  Eigen::Matrix<double, 3, 1> epsilon;

  double epsilon_pos = 1e2;
  double epsilon_angle = Math::pi_4/2;

  MODIFY("CameraMatrixV2:epsilon:pos", epsilon_pos);
  MODIFY("CameraMatrixV2:epsilon:angle", epsilon_angle);

  epsilon << epsilon_pos, epsilon_pos, epsilon_angle;

  std::tuple<Eigen::Matrix<double, 3, 1>, double > r_val = gn_minimizer.minimizeOneStep(*(theCamMatErrorFunction->getModuleT()),epsilon);

  offset = std::get<0>(r_val);
  double error = std::get<1>(r_val);
  double de_dt = (error-last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors.add(de_dt);
  }
  last_error = error;

  PLOT("CameraMatrixV2:error", error);
  PLOT("CameraMatrixV2:de/dt", de_dt);
  PLOT("CameraMatrixV2:avg_derror", derrors.getAverage());

  // TODO: remove damping in gauss newtwon (prefered in the one step) or this one
  MODIFY("CameraMatrixV2:damping:calibrate_only_pose:angle", damping_pose);
  double damping_translation = damping_pose;
  MODIFY("CameraMatrixV2:damping:calibrate_only_pose:location", damping_translation);

  // update offsets
  (theCamMatErrorFunction->getModuleT())->globalPosition.x += offset(0) * damping_translation;
  (theCamMatErrorFunction->getModuleT())->globalPosition.y += offset(1) * damping_translation;
  (theCamMatErrorFunction->getModuleT())->globalPosition.z += offset(2) * damping_pose;

  return (    (derrors.getAverage() > -50) // average error decreases by less than this per second
           && (derrors.getAverage() < 0)   // but it decreases
           && (de_dt < 0)                  // and the last step was an improvement
           && derrors.isFull() );          // and we have a full history
}//end calibrate

// returns true if the averaged reduction per second decreases under a heuristic value
bool CameraMatrixCorrectorV2::calibrateOnlyOffsets()
{
  double dt = getFrameInfo().getTimeInSeconds()-last_frame_info.getTimeInSeconds();

  // calibrate the camera matrix
  Eigen::Matrix<double, 11, 1> offset;
  Eigen::Matrix<double, 11, 1> epsilon = Eigen::Matrix<double, 11, 1>::Constant(1e-4);

  std::tuple<Eigen::Matrix<double, 11, 1>, double > r_val = gn_minimizer.minimizeOneStep(*(theCamMatErrorFunction->getModuleT()),epsilon);

  offset = std::get<0>(r_val);
  double error = std::get<1>(r_val);
  double de_dt = (error-last_error)/dt; // improvement per second

  if(last_error != 0){ //ignore the jump from zero to initial error value
    derrors.add(de_dt);
  }
  last_error = error;

  PLOT("CameraMatrixV2:error", error);
  PLOT("CameraMatrixV2:de/dt", de_dt);
  PLOT("CameraMatrixV2:avg_derror", derrors.getAverage());

  MODIFY("CameraMatrixV2:damping:calibrate_only_offsets", damping_cam_offsets);

  // update offsets
  getCameraMatrixOffset().body_rot += Vector2d(offset(0),offset(1)) * damping_cam_offsets;
  getCameraMatrixOffset().head_rot += Vector3d(offset(2),offset(3),offset(4)) * damping_cam_offsets;
  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    += Vector3d(offset(5),offset(6),offset(7))  * damping_cam_offsets;
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] += Vector3d(offset(8),offset(9),offset(10)) * damping_cam_offsets;

  return (    (derrors.getAverage() > -50) // average error decreases by less than this per second
           && (derrors.getAverage() < 0)   // but it decreases
           && (de_dt < 0)                  // and the last step was an improvement
           && derrors.isFull() );          // and we have a full history
}//end calibrate

// returns true, if the trajectory is starting again from beginning
bool CameraMatrixCorrectorV2::movingHead()
{
    double yaw = 0;
    double pitch = 0;
    bool   target_reached = false;
    bool   finished = false;

    switch (head_state){
    case look_left:
        yaw = 88;
        pitch = 0;
        break;
    case look_right:
        yaw = -88;
        pitch = 0;
        break;
    case look_left_down:
        yaw = 88;
        pitch = 10;
        break;
    case look_right_down:
        yaw = -88;
        pitch = 10;
        break;
    case look_left_up:
        yaw = 88;
        pitch = -20;
        break;
    case look_right_up:
        yaw = -88;
        pitch = -20;
        break;
    default:
        break;
    }

    getHeadMotionRequest().id = HeadMotionRequest::goto_angle;
    getHeadMotionRequest().targetJointPosition.x = Math::fromDegrees(yaw);
    getHeadMotionRequest().targetJointPosition.y = Math::fromDegrees(pitch);
    getHeadMotionRequest().velocity = 10;
    MODIFY("CameraMatrixV2:collecting_velocity", getHeadMotionRequest().velocity);

    // state transitions
    target_reached =    (fabs(Math::toDegrees(getSensorJointData().position[JointData::HeadYaw]) - yaw) < 2)
                     && (fabs(Math::toDegrees(getSensorJointData().position[JointData::HeadPitch]) - pitch) < 2);
    if(target_reached){
        if(head_state == look_left && last_head_state == initial){
            head_state = look_right;
            last_head_state = look_left;
        } else if (head_state == look_right && last_head_state == look_left){
            head_state = look_right_down;
            last_head_state = look_right;
        } else if (head_state == look_right_down && last_head_state == look_right){
            head_state = look_right;
            last_head_state = look_right_down;
        } else if (head_state == look_right && last_head_state == look_right_down){
            head_state = look_left;
            last_head_state = look_right;
        } else if (head_state == look_left && last_head_state == look_right){
            head_state = look_left_down;
            last_head_state = look_left;
        } else if (head_state == look_left_down && last_head_state == look_left){
            head_state = look_right_down;
            last_head_state = look_left_down;
        } else if (head_state == look_right_down && last_head_state == look_left_down){
            head_state = look_left_down;
            last_head_state = look_right_down;
        } else if (head_state == look_left_down && last_head_state == look_right_down){
            head_state = look_left;
            last_head_state = look_left_down;
        } else if (head_state == look_left && last_head_state == look_left_down){
            head_state = look_left_up;
            last_head_state = look_left;
        } else if (head_state == look_left_up && last_head_state == look_left){
            head_state = look_right_up;
            last_head_state = look_left_up;
        } else if (head_state == look_right_up && last_head_state == look_left_up){
            head_state = look_right;
            last_head_state = look_right_up;
        } else if (head_state == look_right && last_head_state == look_right_up){
            head_state = look_right_up;
            last_head_state = look_right;
        } else if (head_state == look_right_up && last_head_state == look_right){
            head_state = look_left_up;
            last_head_state = look_right_up;
        } else if (head_state == look_left_up && last_head_state == look_right_up){
            head_state = look_left;
            last_head_state = look_left_up;
        } else if (head_state == look_left && last_head_state == look_left_up){
            head_state = look_right;
            last_head_state = look_left;
            finished = true;
        }
    }
    return finished;
}

void CameraMatrixCorrectorV2::collectingData()
{
    if(last_head_state != initial) {
        int current_index_yaw   = static_cast<int>((Math::toDegrees(getSensorJointData().position[JointData::HeadYaw])/15.0) + 0.5);
        int current_index_pitch = static_cast<int>((Math::toDegrees(getSensorJointData().position[JointData::HeadPitch])/5.0) + 0.5);

        if(last_idx_pitch != current_index_pitch || last_idx_yaw != current_index_yaw) {

            LineGraphPercept lineGraphPercept(getLineGraphPercept());

            // TODO: ignore upper image if the center is above the horizon
            // HACK: ignore it if headpitch is smaller than -15
            //if(head_state == look_right_up || head_state == look_left_up
            //   || last_head_state == look_right_up || last_head_state == look_left_up){
            if(Math::toDegrees(getSensorJointData().position[JointData::HeadPitch]) < -15) {
                lineGraphPercept.edgelsInImageTop.clear();
            }

            (theCamMatErrorFunction->getModuleT())->add(CamMatErrorFunction::CalibrationDataSample(getKinematicChain().theLinks[KinematicChain::Torso].M,
                                                                        lineGraphPercept,
                                                                        getInertialModel(),
                                                                        getSensorJointData().position[JointData::HeadYaw],
                                                                        getSensorJointData().position[JointData::HeadPitch]
                                                                       ));

            last_idx_pitch = current_index_pitch;
            last_idx_yaw   = current_index_yaw;
        }
    }
}

void CameraMatrixCorrectorV2::doItAutomatically()
{
    if(!auto_cleared_data) {
        (theCamMatErrorFunction->getModuleT())->clear();
        reset_calibration();
        auto_cleared_data = true;
    }

    if(!auto_collected){
        auto_collected = movingHead();
        collectingData();
    }

    if(auto_collected && !auto_calibrated){
        auto_calibrated = calibrate();
    }

    if(auto_calibrated){
        getCameraMatrixOffset().saveToConfig();
    }
}
