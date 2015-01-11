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
#include <Tools/Debug/DebugImageDrawings.h>
#include "Tools/Debug/DebugModify.h"

#include "Tools/CameraGeometry.h"
#include <Tools/NaoInfo.h>

#include "Motion/MorphologyProcessor/ForwardKinematics.h"


CameraMatrixCorrector::CameraMatrixCorrector()
{
  DEBUG_REQUEST_REGISTER("CameraMatrix:calibrate_camera_matrix",
    "calculates the roll and tilt offset of the camera using the goal (it. shoult be exactely 3000mm in front of the robot)", 
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrix:calibrate_camera_matrix1965", "...", false);

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
      calibrate(&CameraMatrixCorrector::projectionError);
  });
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrix:calibrate_camera_matrix", 
    if(cameraID == camera_to_calibrate) {
      getCameraMatrixOffset().saveToConfig();
  });


  DEBUG_REQUEST("CameraMatrix:calibrate_camera_matrix1965", 
    if(cameraID == camera_to_calibrate) {
      calibrate(&CameraMatrixCorrector::horizonError);
  });
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrix:calibrate_camera_matrix1965", 
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
  // REALLY?! WHY? - remove it, if there is nor answer found until 01.10.2014
  getCameraMatrix()
       .rotateY(getCameraMatrixOffset().offset.y)
       .rotateX(getCameraMatrixOffset().offset.x);
  */

}//end execute


void CameraMatrixCorrector::reset_calibration()
{
  getCameraMatrixOffset().correctionOffset[cameraID] = Vector2d();
}

void CameraMatrixCorrector::calibrate(ErrorFunction errorFunction)
{
  // calibrate the camera matrix

  if (getGoalPercept().getNumberOfSeenPosts() < 2
      || !getGoalPercept().getPost(0).positionReliable 
      || !getGoalPercept().getPost(1).positionReliable)
    return;

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
  MODIFY("CameraMatrixCorrector:lambda", lambda);
  if (offset.abs() > lambda) {
    offset.normalize(lambda);
  }

  getCameraMatrixOffset().correctionOffset[cameraID] += offset;
}//end calibrate


double CameraMatrixCorrector::projectionError(double offsetX, double offsetY)
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
}//end projectionError


double CameraMatrixCorrector::horizonError(double offsetX, double offsetY)
{
  Vector2d offset(offsetX, offsetY);

  CameraMatrix tmpCM = CameraGeometry::calculateCameraMatrix(
    getKinematicChain(),
    NaoInfo::robotDimensions.cameraTransform[cameraID].offset,
    NaoInfo::robotDimensions.cameraTransform[cameraID].rotationY,
    getCameraMatrixOffset().correctionOffset[cameraID] + offset
  );

  // estimate the hirizon based on the CM
  Vector2d a,b;
  CameraGeometry::calculateArtificialHorizon(tmpCM, getCameraInfo(), a, b);
  Math::LineSegment horizon_cm(a,b);
  

  // estimate the hirizon based on the seen goal
  const GoalPercept::GoalPost& postOne = getGoalPercept().getPost(0);
  const GoalPercept::GoalPost& postTwo = getGoalPercept().getPost(1);

  double hi1 = Vector2d(postOne.basePoint - postOne.topPoint).abs();
  double hi2 = Vector2d(postTwo.basePoint - postTwo.topPoint).abs();

  double h = getCameraMatrix().translation.z;
  double hR = getFieldInfo().goalHeight + 50;

  double hH1 = h*hi1/hR;
  double hH2 = h*hi2/hR;

  Vector2d hP1 = postOne.basePoint + Vector2d(postOne.topPoint - postOne.basePoint).normalize(hH1);
  Vector2d hP2 = postTwo.basePoint + Vector2d(postTwo.topPoint - postTwo.basePoint).normalize(hH2);

  CANVAS_PX(cameraID);
  LINE_PX(ColorClasses::blue, (int)(hP1.x +0.5), (int)(hP1.y +0.5), (int)(hP2.x +0.5), (int)(hP2.y +0.5));

  

  double error =  Math::sqr(horizon_cm.Line::minDistance(hP1)) + 
                  Math::sqr(horizon_cm.Line::minDistance(hP2));
    
  return error;
}//end horizonError

void CameraMatrixCorrector::calibrate1965()
{
  if (getGoalPercept().getNumberOfSeenPosts() < 2
      || !getGoalPercept().getPost(0).positionReliable 
      || !getGoalPercept().getPost(1).positionReliable)
    return;

  const GoalPercept::GoalPost& postOne = getGoalPercept().getPost(0);
  const GoalPercept::GoalPost& postTwo = getGoalPercept().getPost(1);

  double hi1 = Vector2d(postOne.basePoint - postOne.topPoint).abs();
  double hi2 = Vector2d(postTwo.basePoint - postTwo.topPoint).abs();

  double h = getCameraMatrix().translation.z;
  double hR = getFieldInfo().goalHeight + 50;

  double hH1 = h*hi1/hR;
  double hH2 = h*hi2/hR;

  Vector2d hP1 = postOne.basePoint + Vector2d(postOne.topPoint - postOne.basePoint).normalize(hH1);
  Vector2d hP2 = postTwo.basePoint + Vector2d(postTwo.topPoint - postTwo.basePoint).normalize(hH2);

  CANVAS_PX(cameraID);
  LINE_PX(ColorClasses::blue, (int)(hP1.x +0.5), (int)(hP1.y +0.5), (int)(hP2.x +0.5), (int)(hP2.y +0.5));
}