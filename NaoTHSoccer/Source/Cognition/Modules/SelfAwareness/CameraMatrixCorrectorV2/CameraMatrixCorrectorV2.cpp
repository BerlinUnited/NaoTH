/**
* @file CameraMatrixProvider.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixProvider
*/

#include "CameraMatrixCorrectorV2.h"

#include "Tools/CameraGeometry.h"
#include <Tools/NaoInfo.h>

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

}

CameraMatrixCorrectorV2::~CameraMatrixCorrectorV2()
{
}

void CameraMatrixCorrectorV2::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  CameraInfo::CameraID camera_to_calibrate = CameraInfo::numOfCamera;

  DEBUG_REQUEST("CameraMatrixV2:CamTop", camera_to_calibrate = CameraInfo::Top; );
  DEBUG_REQUEST("CameraMatrixV2:CamBottom", camera_to_calibrate = CameraInfo::Bottom; );

  /*DEBUG_REQUEST("CameraMatrixV2:calibrate_camera_matrix",
    if(cameraID == camera_to_calibrate) {
      calibrate(&CameraMatrixCorrectorV2::projectionError);
  });
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrixV2:calibrate_camera_matrix",
    if(cameraID == camera_to_calibrate) {
      getCameraMatrixOffset().saveToConfig();
  });*/

  DEBUG_REQUEST("CameraMatrixV2:calibrate_camera_matrix_line_matching",
    if(cameraID == camera_to_calibrate) {
      calibrate(&CameraMatrixCorrectorV2::lineMatchingError);
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

double CameraMatrixCorrectorV2::lineMatchingError(double offsetX, double offsetY)
{
    double total_sum = 0;

    Vector2d offset(offsetX, offsetY);

    CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrix(
      getKinematicChain(),
      NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
      NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
      getCameraMatrixOffset().correctionOffset[cameraID] + offset
    );

    const CameraInfo& cameraInfo = getCameraInfo();

    std::vector<Vector2d> edgelProjections;
    edgelProjections.resize(getScanLineEdgelPercept().pairs.size());

    // project edgels pairs to field
    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
    {
      const EdgelT<double>& edgelOne = getScanLineEdgelPercept().pairs[i];

      CameraGeometry::imagePixelToFieldCoord(
        tmpCM, cameraInfo,
        edgelOne.point.x,
        edgelOne.point.y,
        0.0,
        edgelProjections[i]);
    }

    // for all Egels in edgels
    for(std::vector<Vector2d>::const_iterator iter = edgelProjections.begin(); iter != edgelProjections.end(); ++iter){

        const Vector2d& seen_point_relative = *iter;

        Pose2D   robotPose;
        Vector2d seen_point_g = robotPose * seen_point_relative;

        int line_idx = getFieldInfo().fieldLinesTable.getNearestLine(seen_point_g, LinesTable::all_lines);

        // there is no such line
        if(line_idx == -1) {
          continue;
        }

        // get the line
        const Math::LineSegment& line = getFieldInfo().fieldLinesTable.getLines()[line_idx];

        total_sum += line.minDistance(seen_point_g);
    }

    return total_sum;
}

void CameraMatrixCorrectorV2::calibrate(ErrorFunction errorFunction)
{
  // calibrate the camera matrix

  Vector2d offset;
  double epsylon = 1e-4;

  // make only one step
  for (int i = 0; i < 1; i++)
  {
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    double dg11 = (*this.*errorFunction)(offset.x + epsylon, offset.y);
    double dg12 = (*this.*errorFunction)(offset.x - epsylon, offset.y);
    double dg1 = (dg11 - dg12) / (2 * epsylon);

    double dg21 = (*this.*errorFunction)(offset.x, offset.y + epsylon);
    double dg22 = (*this.*errorFunction)(offset.x, offset.y - epsylon);
    double dg2 = (dg21 - dg22) / (2 * epsylon);

    // Dg(x)^T*Dg(x)
    Vector2d dg(dg1, dg2);

    if (dg.abs() < epsylon) break;

    // g(x) - target
    double w = (*this.*errorFunction)(offset.x, offset.y);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2d z_GN = dg * (-w / (dg * dg));
    offset += z_GN;
  }//end for


  double lambda = 0.005;
  MODIFY("CameraMatrixCorrectorV2:lambda", lambda);
  if (offset.abs() > lambda) {
    offset.normalize(lambda);
  }

  getCameraMatrixOffset().correctionOffset[cameraID] += offset;
}//end calibrate


/*double CameraMatrixCorrectorV2::projectionError(double offsetX, double offsetY)
{
  Vector2d offset(offsetX, offsetY);

  CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrix(
    getKinematicChain(),
    NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
    NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
    getCameraMatrixOffset().correctionOffset[cameraID] + offset
  );

  // project the goal posts
  const CameraInfo& cameraInfo = getCameraInfo();

  Vector2d leftPosition;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM, 
      cameraInfo,
      getGoalPercept().getPost(0).basePoint.x, 
      getGoalPercept().getPost(0).basePoint.y, 
      0.0,
      leftPosition);

  Vector2d rightPosition;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM, 
      cameraInfo,
      getGoalPercept().getPost(1).basePoint.x, 
      getGoalPercept().getPost(1).basePoint.y, 
      0.0,
      rightPosition);

  
  double error = 
    (getFieldInfo().opponentGoalPostLeft  - leftPosition).abs() +
    (getFieldInfo().opponentGoalPostRight - rightPosition).abs();
    
  return error;
}//end projectionError*/
