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
  DEBUG_REQUEST_REGISTER("CameraMatrixV2:calibrate_camera_matrix",
    "calculates the roll and tilt offset of the camera using the goal (it. shoult be exactely 3000mm in front of the robot)", 
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:calibrate_camera_matrix_line_matching",
    "calculates the roll and tilt offset of the camera using field lines (it. shoult be exactely 3000mm in front of the robot)",
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:CamTop","",false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV2:CamBottom","",false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:reset_calibration", "set the calibration offsets of the CM to 0", false);

  DEBUG_REQUEST_REGISTER("CameraMatrixV2:debug_drawings:draw_projected_edgels",
    "",
    false);
  DEBUG_REQUEST_REGISTER("CameraMatrixV2:debug_drawings:draw_matching_global",
                         "",
                         false);

  theCamMatErrorFunction = registerModule<CamMatErrorFunction>("CamMatErrorFunction", true);
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
}

void CameraMatrixCorrectorV2::calibrate()
{
  // calibrate the camera matrix
  Eigen::Matrix<double, 2, 1> offset;

  offset = gn_minimizer.minimizeOneStep(theCamMatErrorFunction->getModuleT(),1e-4);

  getCameraMatrixOffset().correctionOffset[cameraID] += Vector2d(offset(0),offset(1));
}//end calibrate
