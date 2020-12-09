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

  theCamMatErrorFunction = registerModule<CamMatErrorFunction>("CamMatErrorFunction", true);
  last_idx_yaw   = 0;
  last_idx_pitch = 0;
  damping = 0.1;

  head_state      = look_left;
  last_head_state = initial;
}

CameraMatrixCorrectorV2::~CameraMatrixCorrectorV2()
{
}

void CameraMatrixCorrectorV2::execute()
{
  DEBUG_REQUEST("CameraMatrixV2:clear_calibration_data",
    (theCamMatErrorFunction->getModuleT())->calibrationData.clear();
  );

  bool collect_data = false;
  DEBUG_REQUEST("CameraMatrixV2:collect_calibration_data",
    collect_data = true;
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:collect_calibration_data",
    head_state = look_left;
    last_head_state = initial;
    getHeadMotionRequest().id = HeadMotionRequest::hold;
  );

  if(collect_data){
      // head control states

      double yaw = 0;
      double pitch = 0;
      bool   target_reached = false;

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
      target_reached = (fabs(Math::toDegrees(getSensorJointData().position[JointData::HeadYaw]) - yaw) < 2) && (fabs(Math::toDegrees(getSensorJointData().position[JointData::HeadPitch]) - pitch) < 2);

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
          }
      }

      // collecting data
      if(last_head_state != initial) {
          CamMatErrorFunction::CalibrationData& c_data = (theCamMatErrorFunction->getModuleT())->calibrationData;

          int current_index_yaw   = static_cast<int>((Math::toDegrees(getSensorJointData().position[JointData::HeadYaw])/15.0) + 0.5);
          int current_index_pitch = static_cast<int>((Math::toDegrees(getSensorJointData().position[JointData::HeadPitch])/5.0) + 0.5);

          std::pair<int,int> index;
          index.first  = current_index_yaw;
          index.second = current_index_pitch;

          if(last_idx_pitch != current_index_pitch || last_idx_yaw != current_index_yaw) {
              std::vector<CamMatErrorFunction::CalibrationDataSample>& data = c_data[index];

              LineGraphPercept lineGraphPercept(getLineGraphPercept());

              if(head_state == look_right_up || head_state == look_left_up
                 || last_head_state == look_right_up || last_head_state == look_left_up){
                  lineGraphPercept.edgelsInImageTop.clear();
              }

              data.push_back(CamMatErrorFunction::CalibrationDataSample(getKinematicChain().theLinks[KinematicChain::Torso].M,
                                                                        lineGraphPercept,getInertialModel(),
                                                                        getSensorJointData().position[JointData::HeadYaw],
                                                                        getSensorJointData().position[JointData::HeadPitch]
                                                                      ));

    //          c_data[index].chestPose        = getKinematicChain().theLinks[KinematicChain::Torso].M;
    //          c_data[index].lineGraphPercept = getLineGraphPercept();
    //          c_data[index].headYaw          = getSensorJointData().position[JointData::HeadYaw];
    //          c_data[index].headPitch        = getSensorJointData().position[JointData::HeadPitch];
    //          c_data[index].inertialModel    = getInertialModel();

              last_idx_pitch = current_index_pitch;
              last_idx_yaw   = current_index_yaw;
          }
      }
  }

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

  DEBUG_REQUEST("CameraMatrixV2:enable_CamMatErrorFunction_drawings",
      theCamMatErrorFunction->getModuleT()->plot_CalibrationData();
  );

  DEBUG_REQUEST("CameraMatrixV2:calibrate_camera_matrix_line_matching",
      calibrate();
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:calibrate_camera_matrix_line_matching",
      getCameraMatrixOffset().saveToConfig();
  );

  DEBUG_REQUEST("CameraMatrixV2:reset_calibration",
      reset_calibration();
  );

  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:reset_calibration",
      getCameraMatrixOffset().saveToConfig();
  );

}//end execute


void CameraMatrixCorrectorV2::reset_calibration()
{
  getCameraMatrixOffset().body_rot = Vector2d();
  getCameraMatrixOffset().head_rot = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    = Vector3d();
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] = Vector3d();
}

void CameraMatrixCorrectorV2::calibrate()
{
  // calibrate the camera matrix
  Eigen::Matrix<double, 11, 1> offset;

  offset = gn_minimizer.minimizeOneStep(*(theCamMatErrorFunction->getModuleT()),1e-4);

  MODIFY("CameraMatrixV2:damping_factor", damping);

  getCameraMatrixOffset().body_rot = getCameraMatrixOffset().body_rot * (1-damping) + (getCameraMatrixOffset().body_rot + Vector2d(offset(0),offset(1)))           * damping;
  getCameraMatrixOffset().head_rot = getCameraMatrixOffset().head_rot * (1-damping) + (getCameraMatrixOffset().head_rot + Vector3d(offset(2),offset(3),offset(4))) * damping;
  getCameraMatrixOffset().cam_rot[CameraInfo::Top]    = getCameraMatrixOffset().cam_rot[CameraInfo::Top]    * (1-damping) + (getCameraMatrixOffset().cam_rot[CameraInfo::Top]    + Vector3d(offset(5),offset(6),offset(7)))  * damping;
  getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] = getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] * (1-damping) + (getCameraMatrixOffset().cam_rot[CameraInfo::Bottom] + Vector3d(offset(8),offset(9),offset(10))) * damping;
}//end calibrate
