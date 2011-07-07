/**
* @file CameraMatrixProvider.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixProvider
*/

#include "CameraMatrixProvider.h"

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/CameraGeometry.h"
#include "Tools/Math/Line.h"

#include "Motion/MorphologyProcessor/ForwardKinematics.h"
#include "Motion/CameraMatrixCalculator/CameraMatrixCalculator.h"

#include "PlatformInterface/Platform.h"

CameraMatrixProvider::CameraMatrixProvider()
{
  udpateTime = getFrameInfo().getTime();

  // this is a HACK!!!!!!!!!!
  getKinematicChain().init(getSensorJointData());

  DEBUG_REQUEST_REGISTER("CameraMatrix:calibrate_camera_matrix",
    "calculates the roll and tilt offset of the camera using the goal (it. shoult be exactely 3000mm in front of the robot)", 
    false);

  DEBUG_REQUEST_REGISTER("CameraMatrix:reset_calibration", "set the calibration offsets of the CM to 0", false);

  DEBUG_REQUEST_REGISTER("3DViewer:Robot:Camera", "Show the robot body in the 3D viewer.", false);
}

CameraMatrixProvider::~CameraMatrixProvider()
{
}

void CameraMatrixProvider::execute()
{
  double deltaTime = ( getFrameInfo().getTime() - udpateTime ) * 0.001;
  udpateTime = getFrameInfo().getTime();
  
  DEBUG_REQUEST("CameraMatrix:calibrate_camera_matrix", calibrate(); );
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrix:calibrate_camera_matrix", Platform::getInstance().theCameraInfo.saveToConfig(); );

  DEBUG_REQUEST("CameraMatrix:reset_calibration", reset_calibration(); );
  DEBUG_REQUEST_ON_DEACTIVE("CameraMatrix:reset_calibration", Platform::getInstance().theCameraInfo.saveToConfig(); );

  // calculate the kinematic chain
  Kinematics::ForwardKinematics::calculateKinematicChainAll(
    getAccelerometerData(),
    getInertialPercept(),
    getKinematicChain(),
    theFSRPos,
    deltaTime);

  getKinematicChain().updateCoM();

  // calculate the camera matrix
  CameraMatrixCalculator::calculateCameraMatrix(
            getCameraMatrix(),
            getImage().cameraInfo.cameraID,
            getKinematicChain());

  // 
  getCameraMatrix().valid = true;

  MODIFY("CameraMatrix:translation:x", getCameraMatrix().translation.x);
  MODIFY("CameraMatrix:translation:y", getCameraMatrix().translation.y);
  MODIFY("CameraMatrix:translation:z", getCameraMatrix().translation.z);
  double correctionAngleX = 0.0;
  double correctionAngleY = 0.0;
  double correctionAngleZ = 0.0;
  MODIFY("CameraMatrix:correctionAngle:x", correctionAngleX);
  MODIFY("CameraMatrix:correctionAngle:y", correctionAngleY);
  MODIFY("CameraMatrix:correctionAngle:z", correctionAngleZ);

  getCameraMatrix().rotation.rotateX(correctionAngleX);
  getCameraMatrix().rotation.rotateY(correctionAngleY);
  getCameraMatrix().rotation.rotateZ(correctionAngleZ);

  // estimate the horizon
  Vector2<double> p1, p2;
  CameraGeometry::calculateArtificialHorizon(getCameraMatrix(), getImage().cameraInfo, p1, p2);
  getCameraMatrix().horizon = Math::LineSegment(p1, p2);


  DEBUG_REQUEST("3DViewer:Robot:Camera",
    const CameraInfo& ci = Platform::getInstance().theCameraInfo;
    DebugDrawings3D::getInstance().addCamera(getCameraMatrix(),ci.focalLength, ci.resolutionWidth, ci.resolutionHeight););

}//end execute


void CameraMatrixProvider::reset_calibration()
{
  CameraInfoParameter& cameraInfo = Platform::getInstance().theCameraInfo;
  cameraInfo.cameraRollOffset = 0.0;
  cameraInfo.cameraTiltOffset = 0.0;
}

void CameraMatrixProvider::calibrate()
{
  // calibrate the camera matrix
  // currently it is in test-mode, the correction parameter
  // are stored as static members of CameraMatrixCalculator

  if (getGoalPercept().getNumberOfSeenPosts() < 2)
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
  CameraInfoParameter& cameraInfo = Platform::getInstance().theCameraInfo;

  double lambda = 0.01;
  if (offset.abs() > lambda)
    offset.normalize(lambda);

  cameraInfo.cameraRollOffset += offset.x;
  cameraInfo.cameraTiltOffset += offset.y;


  double maxValue = Math::fromDegrees(10.0); // maximal correction
  cameraInfo.cameraRollOffset = Math::clamp(cameraInfo.cameraRollOffset, -maxValue, maxValue);
  cameraInfo.cameraTiltOffset = Math::clamp(cameraInfo.cameraTiltOffset, -maxValue, maxValue);
}//end calibrate


double CameraMatrixProvider::projectionError(double offsetX, double offsetY)
{
  CameraMatrix tmpCM(getCameraMatrix());

  tmpCM.rotateX(offsetX)
       .rotateY(offsetY);

  // project the goal posts
  const CameraInfoParameter& cameraInfo = Platform::getInstance().theCameraInfo;

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

  return error;
}//end projectionError
