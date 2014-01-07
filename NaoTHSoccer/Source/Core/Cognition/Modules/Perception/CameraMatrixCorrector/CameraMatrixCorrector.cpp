/**
* @file CameraMatrixProvider.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixProvider
*/

#include "CameraMatrixCorrector.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugDrawings3D.h>

#include "Tools/CameraGeometry.h"

#include "Motion/MorphologyProcessor/ForwardKinematics.h"


CameraMatrixCorrector::CameraMatrixCorrector()
{
  DEBUG_REQUEST_REGISTER("CameraMatrix:calibrate_camera_matrix",
    "calculates the roll and tilt offset of the camera using the goal (it. shoult be exactely 3000mm in front of the robot)", 
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrix:CamTop","",false);
  DEBUG_REQUEST_REGISTER("CameraMatrix:CamBottom","",false);

  DEBUG_REQUEST_REGISTER("CameraMatrix:reset_calibration", "set the calibration offsets of the CM to 0", false);

  DEBUG_REQUEST_REGISTER("3DViewer:Robot:Camera", "Show the robot body in the 3D viewer.", false);
}

CameraMatrixCorrector::~CameraMatrixCorrector()
{
}

void CameraMatrixCorrector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  CameraInfo::CameraID camera_to_calibrate = CameraInfo::numOfCamera;

  DEBUG_REQUEST("CameraMatrix:CamTop", camera_to_calibrate = CameraInfo::Top; );
  DEBUG_REQUEST("CameraMatrix:CamBottom", camera_to_calibrate = CameraInfo::Bottom; );

  DEBUG_REQUEST("CameraMatrix:calibrate_camera_matrix", 
    if(cameraID == camera_to_calibrate) {
      calibrate();
  });
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrix:calibrate_camera_matrix", 
    if(cameraID == camera_to_calibrate) {
      getCameraMatrixOffset().saveToConfig(); 
  });

  DEBUG_REQUEST("CameraMatrix:reset_calibration", 
    if(cameraID == camera_to_calibrate) {
      reset_calibration(); 
  });
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrix:reset_calibration", 
    if(cameraID == camera_to_calibrate) {
      getCameraMatrixOffset().saveToConfig(); 
  });


  /* TODO: this doesn't work properly
  getCameraMatrix()
       .rotateY(getCameraMatrixOffset().offset.y)
       .rotateX(getCameraMatrixOffset().offset.x);
  */

}//end execute


void CameraMatrixCorrector::reset_calibration()
{
  CameraMatrixOffset& cameraInfo = getCameraMatrixOffset();
  cameraInfo.correctionOffset[cameraID] = Vector2d();
  //cameraInfo.cameraRollOffset = 0.0;
  //cameraInfo.cameraTiltOffset = 0.0;
}

void CameraMatrixCorrector::calibrate()
{
  // calibrate the camera matrix

  if (getGoalPercept().getNumberOfSeenPosts() < 2
      || !getGoalPercept().getPost(0).positionReliable 
      || !getGoalPercept().getPost(1).positionReliable)
    return;

  Vector2<double> offset;
  double epsylon = 1e-8;

  // make only one step
  for (int i = 0; i < 1; i++)
  {
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    double dg11 = projectionError(offset.x + epsylon, offset.y);
    double dg12 = projectionError(offset.x - epsylon, offset.y);
    double dg1 = (dg11 - dg12) / (2 * epsylon);

    double dg21 = projectionError(offset.x, offset.y + epsylon);
    double dg22 = projectionError(offset.x, offset.y - epsylon);
    double dg2 = (dg21 - dg22) / (2 * epsylon);

    // Dg(x)^T*Dg(x)
    Vector2<double> dg(dg1, dg2);

    if (dg.abs() < epsylon) break;

    // g(x) - target
    double w = projectionError(offset.x, offset.y);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2<double> z_GN = dg * (-w / (dg * dg));
    offset += z_GN;
  }//end for


  // apply changes
  CameraMatrixOffset& cameraInfo = getCameraMatrixOffset();

  double lambda = 0.01;
  if (offset.abs() > lambda) {
    offset.normalize(lambda);
  }


  double maxValue = Math::fromDegrees(10.0); // maximal correction offset
  offset.x = Math::clamp(offset.x, -maxValue, maxValue);
  offset.y = Math::clamp(offset.y, -maxValue, maxValue);

  //cameraInfo.cameraRollOffset += offset.x;
  //cameraInfo.cameraTiltOffset += offset.y;
  cameraInfo.correctionOffset[cameraID] += offset;
}//end calibrate


double CameraMatrixCorrector::projectionError(double offsetX, double offsetY)
{
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateY(offsetY)
       .rotateX(offsetX);

  // project the goal posts
  const CameraInfo& cameraInfo = getCameraInfo();

  Vector2<double> leftPosition;
  CameraGeometry::imagePixelToFieldCoord(
      tmpCM, 
      cameraInfo,
      getGoalPercept().getPost(0).basePoint.x, 
      getGoalPercept().getPost(0).basePoint.y, 
      0.0,
      leftPosition);

  Vector2<double> rightPosition;
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
    

  /*
  double goal_width = (getFieldInfo().opponentGoalPostLeft-getFieldInfo().opponentGoalPostRight).abs2();
  double seen_width = (leftPosition-rightPosition).abs2();
  double error = Math::sqr(goal_width - seen_width);
  */

  return error;
}//end projectionError
