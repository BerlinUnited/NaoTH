/**
* @file CameraMatrixProvider.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixProvider
*/

#include "CameraMatrixCorrectorV2.h"

#include "Tools/CameraGeometry.h"

#include "Motion/MorphologyProcessor/ForwardKinematics.h"


CameraMatrixCorrectorV2::CameraMatrixCorrectorV2()
{
  DEBUG_REQUEST_REGISTER("CameraMatrixV2:calibrate_camera_matrix_line_matching",
    "calculates the roll and tilt offset of the camera using field lines (it. shoult be exactely 3000mm in front of the robot)",
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:CamTop","",false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV2:CamBottom","",false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:reset_calibration", "set the calibration offsets of the CM to 0", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:collect_calibration_data", "collect the data for calibration", false);

  theCamMatErrorFunction = registerModule<CamMatErrorFunction>("CamMatErrorFunction", true);
  last_index = 0;
}

CameraMatrixCorrectorV2::~CameraMatrixCorrectorV2()
{
}

void CameraMatrixCorrectorV2::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  theCamMatErrorFunction->getModuleT()->cameraID = cameraID;

  CameraInfo::CameraID camera_to_calibrate = CameraInfo::numOfCamera;

  DEBUG_REQUEST("CameraMatrixV2:CamTop", camera_to_calibrate = CameraInfo::Top; );
  DEBUG_REQUEST("CameraMatrixV2:CamBottom", camera_to_calibrate = CameraInfo::Bottom; );

  DEBUG_REQUEST("CameraMatrixV2:collect_calibration_data",
    //= std::floor(Math::toDegrees(getSensorJointData().position[naoth::HeadYaw] + 90)/18.0 + 0.5);
    CamMatErrorFunction::CalibrationData& c_data = (theCamMatErrorFunction->getModuleT())->calibrationData;

    int current_index = static_cast<int>(((Math::toDegrees(getSensorJointData().position[JointData::HeadYaw]) + 90.0)/18.0) + 0.5);

    if(last_index != current_index && current_index >= 0 && current_index <= (int) c_data.size()) {
        c_data[current_index].headPose   = getKinematicChain().theLinks[KinematicChain::Head].M;;
        c_data[current_index].lineGraphPercept = getLineGraphPercept();
        last_index = current_index;
    }

  );

  MODIFY("CameraMatrixV2:OffsetRollTop",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Top].x);
  MODIFY("CameraMatrixV2:OffsetTiltTop",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Top].y);
  MODIFY("CameraMatrixV2:OffsetRollBottom",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Bottom].x);
  MODIFY("CameraMatrixV2:OffsetTiltBottom",getCameraMatrixOffset().correctionOffset[naoth::CameraInfo::Bottom].y);

  MODIFY("CameraMatrixV2:Top:Head:OffsetRollTop",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Top].head_rot.x);
  MODIFY("CameraMatrixV2:Top:Head:OffsetPitchTop",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Top].head_rot.y);
  MODIFY("CameraMatrixV2:Top:Head:OffsetYawTop",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Top].head_rot.z);
  MODIFY("CameraMatrixV2:Top:Cam:OffsetRollTop",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Top].cam_rot.x);
  MODIFY("CameraMatrixV2:Top:Cam:OffsetPitchTop",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Top].cam_rot.y);
  MODIFY("CameraMatrixV2:Top:Cam:OffsetYawTop",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Top].cam_rot.z);

  MODIFY("CameraMatrixV2:Bottom:Head:OffsetRollBottom",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Bottom].head_rot.x);
  MODIFY("CameraMatrixV2:Bottom:Head:OffsetPitchBottom",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Bottom].head_rot.y);
  MODIFY("CameraMatrixV2:Bottom:Head:OffsetYawBottom",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Bottom].head_rot.z);
  MODIFY("CameraMatrixV2:Bottom:Cam:OffsetRollBottom",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Bottom].cam_rot.x);
  MODIFY("CameraMatrixV2:Bottom:Cam:OffsetPitchBottom",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Bottom].cam_rot.y);
  MODIFY("CameraMatrixV2:Bottom:Cam:OffsetYawBottom",getCameraMatrixOffset().correctionOffsets[naoth::CameraInfo::Bottom].cam_rot.z);

  DEBUG_REQUEST("CameraMatrixV2:calibrate_camera_matrix_line_matching",
    if(cameraID == camera_to_calibrate) {
      calibrate();
  });
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:calibrate_camera_matrix_line_matching",
    if(cameraID == camera_to_calibrate) {
      getCameraMatrixOffset().saveToConfig();
  });

  DEBUG_REQUEST("CameraMatrixV2:reset_calibration",
    if(cameraID == camera_to_calibrate) {
      reset_calibration();
  });
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:reset_calibration",
    if(cameraID == camera_to_calibrate) {
      getCameraMatrixOffset().saveToConfig();
  });

}//end execute


void CameraMatrixCorrectorV2::reset_calibration()
{
  getCameraMatrixOffset().correctionOffset[cameraID] = Vector2d();
  getCameraMatrixOffset().correctionOffsets[cameraID].head_rot = Vector3d();
  getCameraMatrixOffset().correctionOffsets[cameraID].cam_rot  = Vector3d();
}

void CameraMatrixCorrectorV2::calibrate()
{
  // calibrate the camera matrix
  Eigen::Matrix<double, 6, 1> offset;

  offset = gn_minimizer.minimizeOneStep(*(theCamMatErrorFunction->getModuleT()),1e-4);

  getCameraMatrixOffset().correctionOffsets[cameraID].head_rot += Vector3d(offset(0),offset(1),offset(2));
  getCameraMatrixOffset().correctionOffsets[cameraID].cam_rot  += Vector3d(offset(3),offset(4),offset(5));
}//end calibrate
