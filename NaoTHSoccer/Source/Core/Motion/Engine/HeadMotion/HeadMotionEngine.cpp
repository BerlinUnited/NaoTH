/**
 * @file HeadMotionEngine.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author Oliver Welter 
 *
 * Created on 27. Januar 2009, 16:42
 */

#include "HeadMotionEngine.h"


#include <algorithm>
#include <cmath>
#include <iostream>

// Tools
#include <Tools/Math/Common.h>
#include <Tools/Math/Matrix2x2.h>
#include <Tools/NaoInfo.h>
#include <Tools/CameraGeometry.h>

#include <PlatformInterface/Platform.h>

// Debug
//#include "Tools/Debug/DebugBufferedOutput.h"
//#include "Tools/Debug/DebugDrawings.h"
//#include "Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/DebugModify.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Motion/CameraMatrixCalculator/CameraMatrixCalculator.h"

using namespace naoth;

HeadMotionEngine::HeadMotionEngine()
    :
    theBlackBoard(MotionBlackBoard::getInstance()),
    theMotorJointData(MotionBlackBoard::getInstance().theMotorJointData),
    theMotionStatus(MotionBlackBoard::getInstance().theMotionStatus)
  {
    theKinematicChain.init(theJointData);
  //TODO
    //DEBUG_REQUEST_REGISTER("HeadMotionEngine:draw_search_points_on_field", "draw the projected search points on the field", false);
  }

void HeadMotionEngine::execute()
{
  // update torso
  theKinematicChain.theLinks[KinematicChain::Torso].M = theBlackBoard.theKinematicChain.theLinks[KinematicChain::Torso].M;

  switch(theBlackBoard.theHeadMotionRequest.id)
  {
    case HeadMotionRequest::look_straight_ahead: lookStraightAhead(); break;
    case HeadMotionRequest::look_at_point: lookAtPoint(); break;
    
    case HeadMotionRequest::look_at_world_point: 
      lookAtWorldPoint(theBlackBoard.theHeadMotionRequest.targetPointInTheWorld); 
      break;

    case HeadMotionRequest::look_at_point_on_the_ground: 
      gotoPointOnTheGround(theBlackBoard.theHeadMotionRequest.targetPointOnTheGround);
      break;

    case HeadMotionRequest::goto_angle: 
      gotoAngle(theBlackBoard.theHeadMotionRequest.targetJointPosition); 
      break;

    case HeadMotionRequest::hold: hold(); break;
    case HeadMotionRequest::stabilize: lookStraightAheadWithStabilization(); break;
    case HeadMotionRequest::search: search(); break;
    default: break;
  }//end switch


  // plots the function ||g(yaw, pith)-y||
  // used to control the head (cmp. look_at_world_point)
  static bool testCalc = true;

  if(!testCalc)
  {
    ofstream os("test.txt",ofstream::out);
    Vector3<double> target(2500.0, 0.0, 0.0);
    double y = -Math::pi_2;
    double p = -Math::pi_2;
    double steps = 25.0;
    double delta_y = Math::pi/steps;
    double delta_p = Math::pi/steps;
    
    for(int i = 0; i < steps; i++)
    {
      for(int j = 0; j < steps; j++)
      {
        double a = y+i*delta_y;
        double b = p+j*delta_p;
        double f = (g(a, b, target) - target).abs();
        if(j != 0)
          os << ",";

        os << f;
      }
      os << endl;
    }
    os.close();
  }//end if

  testCalc = true;
}//end execute 


void HeadMotionEngine::hold() 
{
  double stiffness = 0.0;    //it was 0.7 in former times

  theMotorJointData.stiffness[JointData::HeadYaw] = stiffness;
  theMotorJointData.stiffness[JointData::HeadPitch] = stiffness;

  theMotorJointData.position[JointData::HeadYaw] =
          theBlackBoard.theSensorJointData.position[JointData::HeadYaw];
  theMotorJointData.position[JointData::HeadPitch] =
          theBlackBoard.theSensorJointData.position[JointData::HeadPitch];
}//end hold

void HeadMotionEngine::gotoPointOnTheGround(const Vector2<double>& target)
{
  Vector3<double> pointOnTheGround(target.x,target.y,0.0);
  
  Vector2<double> centerOnField;
  //TODO: handle the case if the projection is not possible
  CameraGeometry::imagePixelToFieldCoord(
    theBlackBoard.theCameraMatrix, 
    Platform::getInstance().theCameraInfo,
    (double)Platform::getInstance().theCameraInfo.opticalCenterX, 
    (double)Platform::getInstance().theCameraInfo.opticalCenterY, 
    0.0,
    centerOnField);
  
  vector<Vector3<double> > points;
  

  points.push_back(Vector3<double>(centerOnField.x,centerOnField.y,0.0));
  points.push_back(pointOnTheGround);

 
  if ( trajectoryHeadMove(points) )
  {
    //pointOnTheGround = target;
  }//end if
  
}//end gotoPointOnTheGround


/*
 * move the head to the position target = (yaw, pitch)
 */
void HeadMotionEngine::gotoAngle(const Vector2<double>& target) 
{
  // current position
  Vector2<double> headPos;
  headPos.x = theBlackBoard.theMotorJointData.position[JointData::HeadYaw];
  headPos.y = theBlackBoard.theMotorJointData.position[JointData::HeadPitch];

  moveByAngle(target-headPos);
}//end gotoAngle


void HeadMotionEngine::moveByAngle(const Vector2<double>& target) 
{
  static Vector2<double> last_requested_change;

  double max_velocity_deg_in_second = 90.0;
  //TODO
  //MODIFY("HeadMotionEngine:gotoAngle:max_velocity_deg_in_second", max_velocity_deg_in_second);
  double max_velocity = Math::fromDegrees(max_velocity_deg_in_second)*theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();

  double stiffness = 0.7;
  //TODO
  //MODIFY("HeadMotionEngine:gotoAngle:hardness", hardness);

  // current position
  Vector2<double> headPos(
    theBlackBoard.theMotorJointData.position[JointData::HeadYaw],
    theBlackBoard.theMotorJointData.position[JointData::HeadPitch]);

  // calculate the update step (normalize with speed if needed)
  Vector2<double> update(
    Math::normalize(target.x),
    Math::normalize(target.y));
  
  if (update.abs() > max_velocity)
  {
    update = update.normalize(max_velocity);
  }


  // limit the acceleration
  Vector2<double> acceleration = update-last_requested_change;
  //TODO
  //PLOT("HeadMotionEngine:acc_x", acceleration.x);
  //PLOT("HeadMotionEngine:acc_y", acceleration.y);
  double maxAcceleration = 1.0;
  //MODIFY("HeadMotionEngine:gotoAngle:maxAcceleration", maxAcceleration);
  if (acceleration.abs() > maxAcceleration)
  {
    acceleration = acceleration.normalize(maxAcceleration);
  }
  update = last_requested_change + acceleration;


  headPos += update;

  last_requested_change = update;

  theMotorJointData.stiffness[JointData::HeadYaw] = stiffness;
  theMotorJointData.stiffness[JointData::HeadPitch] = stiffness;

  theMotorJointData.position[JointData::HeadYaw] = headPos.x;
  theMotorJointData.position[JointData::HeadPitch] = headPos.y;
}//end moveByAngle


// needed by lookAtWorldPoint
Vector3<double> HeadMotionEngine::g(double yaw, double pitch, const Vector3<double>& pointInWorld)
{
  theJointData.position[JointData::HeadYaw] = theBlackBoard.theMotorJointData.position[JointData::HeadYaw] + yaw;
  theJointData.position[JointData::HeadPitch] = theBlackBoard.theMotorJointData.position[JointData::HeadPitch] + pitch;
  theKinematicChain.theLinks[KinematicChain::Neck].updateFromMother();
  theKinematicChain.theLinks[KinematicChain::Head].updateFromMother();

  CameraMatrix cameraMatrix;
  CameraMatrixCalculator::calculateCameraMatrix(cameraMatrix,
    theBlackBoard.theHeadMotionRequest.cameraID,
    theKinematicChain);

  // the point in the image which should point to the pointInWorld
  Vector2<double> projectionPointInImage(
    Platform::getInstance().theCameraInfo.opticalCenterX,
    Platform::getInstance().theCameraInfo.opticalCenterY);

  Vector3<double> direction = CameraGeometry::imagePixelToCameraCoords(
    cameraMatrix, 
    Platform::getInstance().theCameraInfo, 
    projectionPointInImage.x, 
    projectionPointInImage.y);
  
  double length = (pointInWorld - cameraMatrix.translation).abs();

  return cameraMatrix.translation + direction.normalize(length);
}//end g


void HeadMotionEngine::lookAtWorldPoint(const Vector3<double>& target)
{
  double y = 0.0;
  double p = 0.0;
  Vector2<double> x(y,p);
  double epsylon = 1e-8;

  // perform only one GN-Step
  for(int i = 0; i < 1; i++)
  {
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    Vector3<double> dg11 = g(x.x+epsylon, x.y,  target);
    Vector3<double> dg12 = g(x.x-epsylon, x.y,  target);
    Vector3<double> dg1 = (dg11 - dg12) / (2*epsylon);

    Vector3<double> dg21 = g(x.x, x.y+epsylon,  target);
    Vector3<double> dg22 = g(x.x, x.y-epsylon,  target);
    Vector3<double> dg2 = (dg21 - dg22) / (2*epsylon);

    // Dg(x)^T*Dg(x)
    double dgTdg11 = dg1*dg1;
    double dgTdg22 = dg2*dg2;
    double dgTdg12 = dg1*dg2;
    Matrix2x2<double> DgTDg(Vector2<double>(dgTdg11, dgTdg12), Vector2<double>(dgTdg12, dgTdg22));

    // g(x) - target
    Vector3<double> w = g(x.x, x.y, target);
    w -= target;

    // Dg(x)^T*(g(x) - target)
    Vector2<double> DgTw(dg1*w, dg2*w);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2<double> z_GN = (-(DgTDg.invert()*DgTw));
    x += z_GN;
  }//end for

  moveByAngle(x);
}//end lookAtWorldPoint

void HeadMotionEngine::lookAtWorldPointSimple(const Vector3<double>& target)
{
  Vector3<double> vector = target - theBlackBoard.theCameraMatrix.translation;

	//camera height for nao -535 for webcam -370
  //double neckHeigth = theBlackBoard.theCameraMatrix.translation.z;

  double yaw = atan2(vector.y, vector.x);
  
  // distence to the projection on the ground 
  double d = sqrt(vector.y*vector.y + vector.x*vector.x);
  
  double cameraTilt = 0;
  if(theBlackBoard.theHeadMotionRequest.cameraID == CameraInfo::Bottom)
    cameraTilt = Math::fromDegrees(40.0);

  double pitch = Math::pi_2 - atan2(d, -vector.z) - cameraTilt;

  Vector2<double> targetPosition(yaw, pitch);
  gotoAngle(targetPosition);
}//end lookAtWorldPointSimple


void HeadMotionEngine::lookAtPoint()
{
  double x = theBlackBoard.theHeadMotionRequest.targetPointInImage.x;
  double y = theBlackBoard.theHeadMotionRequest.targetPointInImage.y;
  //double f = RobotInfo::getInstance().theCameraInfo.focalLength;
    
  double dev_x = Platform::getInstance().theCameraInfo.opticalCenterX - x;
  double dev_y = y - Platform::getInstance().theCameraInfo.opticalCenterY;
   
  Vector3<double> startingPointCamera(54,0,68);

  //double offsetX = startingPointCamera.x;
  //double offsetY = startingPointCamera.z;
 
  double divAngleX = atan2(dev_x, Platform::getInstance().theCameraInfo.focalLength);
  double divAngleY = atan2(dev_y, Platform::getInstance().theCameraInfo.focalLength);

  Vector2<double> update(divAngleX, divAngleY);

  moveByAngle(update);
}//end simpleLookAtPoint


/*
 * search for something :)
 */
void HeadMotionEngine::search() 
{
  vector<Vector3<double> > points;

  const Vector3<double>& center = theBlackBoard.theHeadMotionRequest.searchCenter;
  const Vector3<double>& size = theBlackBoard.theHeadMotionRequest.searchSize;

  points.push_back(Vector3<double>(center.x-size.x, center.y-size.y, center.z-size.z));
  points.push_back(Vector3<double>(center.x-size.x, center.y+size.y, center.z-size.z));
  points.push_back(Vector3<double>(center.x+size.x, center.y+size.y, center.z+size.z));
  points.push_back(Vector3<double>(center.x+size.x, center.y-size.y, center.z+size.z));

  if (!theBlackBoard.theHeadMotionRequest.searchDirection)
  {
      std::reverse(points.begin(), points.end());
  }

  if ( trajectoryHeadMove(points) )
  {
    theMotionStatus.headMotion = HeadMotionRequest::numOfHeadMotion;
  }
  else
  {
    theMotionStatus.headMotion = HeadMotionRequest::search;
  }
}//end search

void HeadMotionEngine::randomSearch()
{
    static Vector3<double> randomPoint(0, 0, 0);
    vector<Vector3<double> > points;
    points.push_back(randomPoint);
    if ( trajectoryHeadMove(points) ){
        static double lastDir = 0;
        double minAng = lastDir+Math::pi_2;
        double maxAng = minAng+Math::pi;
        lastDir = Math::random(minAng, maxAng);
        randomPoint += (Vector3<double>(cos(lastDir), sin(lastDir), 0)).normalize(1000);
        randomPoint.x = Math::clamp(randomPoint.x, 50.0, 500.0);
        randomPoint.y = Math::clamp(randomPoint.y, -500.0,500.0);
        randomPoint.z = Math::clamp(randomPoint.y, 0.0,500.0);
    }//end if
}//end randomSearch

/*
 * move the head in the specified trajectory
 */
bool HeadMotionEngine::trajectoryHeadMove(const vector<Vector3<double> >& points)
{
  // current state of the head motion
  // indicates the last visited point in the points list
  static int headMotionState;
  // number of cycles since the headMotionState was visited
  static double cycle = 0.0;

  // maximum ground speed for head motion
  double max_speed = 1000.0; // 1m/s
  //TODO
  //MODIFY("HeadMotionEngine:trajectoryHeadMove:max_speed", max_speed);

  // return value
  // is true if the end of the trajectory is reached
  bool finished = false;
  int MAXSTATE = points.size();

  // this may happens if the trajectory changes
  if(headMotionState >= MAXSTATE)
  {
    headMotionState = 0;
    cycle = 0.0;
  }//end if

//  Vector2<double> headTo2D = CameraGeometry::imagePixelToFieldCoord(
//    theBlackBoard.theCameraMatrix, RobotInfo::getInstance().theCameraInfo,
//    RobotInfo::getInstance().theCameraInfo.opticalCenterX,
//    RobotInfo::getInstance().theCameraInfo.opticalCenterY, points[headMotionState].z);
//  Vector3<double> headTo(headTo2D.x, headTo2D.y, points[headMotionState].z);
//
//  static int stopCount = 0;
//  static double lastHeadYaw = 0;
//  static double lastHeadPitch = 0;
//  const double minSpeed = 0.01; // radian per cycle
//  if ( fabs(lastHeadYaw-theBlackBoard.theSensorJointData.position[JointData::HeadYaw]) < minSpeed
//          && fabs(lastHeadPitch-theBlackBoard.theSensorJointData.position[JointData::HeadPitch]) < minSpeed )
//  {
//      stopCount++;
//  }
//  else{
//      stopCount = 0;
//  }
//  lastHeadYaw = theBlackBoard.theSensorJointData.position[JointData::HeadYaw];
//  lastHeadPitch = theBlackBoard.theSensorJointData.position[JointData::HeadPitch];

  cycle++;
  int nextMotionState = (headMotionState+1)%MAXSTATE;
  double s = cycle / ( (points[nextMotionState]-points[headMotionState]).abs() / (max_speed*theBlackBoard.theRobotInfo.getBasicTimeStepInSecond() ));

  if ( s > 1 )
  {
    headMotionState = nextMotionState;
    cycle = 0;
    if ( headMotionState == 0)
    {
        finished = true;
    }
  }
  else
  {
    Vector3<double> lookTo = points[nextMotionState] * s + points[headMotionState] * (1-s);
    lookAtWorldPoint(lookTo);

  //TODO
  /*
    DEBUG_REQUEST("HeadMotionEngine:draw_search_points_on_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 20);
      CIRCLE(lookTo.x, lookTo.y, 30);
    );
  */
  }

/*
  DEBUG_REQUEST("HeadMotionEngine:draw_search_points_on_field",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 20);

    for(int i = 0; i < MAXSTATE; i++)
    {
        CIRCLE(points[i].x, points[i].y, 20);
    }
  );*/

  return finished;
}//end trajectoryHeadMove


void HeadMotionEngine::lookStraightAhead()
{
  const Pose3D& cameraTrans = Platform::getInstance().theCameraInfo.transformation[theBlackBoard.theHeadMotionRequest.cameraID];
  Vector2<double> target(0.0, -cameraTrans.rotation.getYAngle());
  gotoAngle(target);
}//end lookStraightAhead


void HeadMotionEngine::lookStraightAheadWithStabilization()
{
  Vector2<double> target(0.0, -theBlackBoard.theInertialSensorData.data.y);
  gotoAngle(target);
}//end lookStraightAheadWithStabilization
