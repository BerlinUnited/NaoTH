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
#include <fstream>

// Tools
#include <Tools/CameraGeometry.h>
#include <Tools/Math/Matrix2x2.h>
#include <Tools/NaoInfo.h>

#include "Tools/CameraGeometry.h"

using namespace naoth;
using namespace std;

HeadMotionEngine::HeadMotionEngine():
    last_id(HeadMotionRequest::numOfHeadMotion)
{
  theKinematicChain.init(theJointData);

  DEBUG_REQUEST_REGISTER("HeadMotionEngine:export_g", "exports g for plotting the function", false);
  DEBUG_REQUEST_REGISTER("HeadMotionEngine:draw_search_points_on_field", "draw the projected search points on the field", false);

  getDebugParameterList().add(&params);
}

HeadMotionEngine::~HeadMotionEngine()
{
  getDebugParameterList().remove(&params);
}

void HeadMotionEngine::execute()
{
  // HACK: update torso
  theKinematicChain.theLinks[KinematicChain::Torso].M = getKinematicChainSensor().theLinks[KinematicChain::Torso].M;

  switch(getHeadMotionRequest().id)
  {
    case HeadMotionRequest::look_straight_ahead:
      lookStraightAhead(); break;
    case HeadMotionRequest::look_at_point:
      lookAtPoint(); break;
    case HeadMotionRequest::look_at_world_point:
      lookAtWorldPoint(getHeadMotionRequest().targetPointInTheWorld); break;
    case HeadMotionRequest::look_at_point_on_the_ground:
      gotoPointOnTheGround(getHeadMotionRequest().targetPointOnTheGround); break;
    case HeadMotionRequest::goto_angle:
      gotoAngle(getHeadMotionRequest().targetJointPosition);  break;
    case HeadMotionRequest::hold:
      hold(); break;
    case HeadMotionRequest::stabilize:
      lookStraightAheadWithStabilization(); break;
    case HeadMotionRequest::search:
      search(); break;
    default: break;
  }//end switch

  DEBUG_REQUEST_ON_DEACTIVE("HeadMotionEngine:export_g", export_g(););

  bool target_changed = (last_motion_target - motion_target).abs() > params.at_target_threshold;
  if(last_id != getHeadMotionRequest().id
     || target_changed){
	  //TODO vielleicht ist es sinnvoller die gelenkwinkel zu filtern anstatt die geschwindigkeit
      absolute_velocity_buffer.clear();
  }

  last_id = getHeadMotionRequest().id;
  last_motion_target = motion_target;

  static Vector2d last_yaw_pitch;
  Vector2d current_yaw_pitch(getSensorJointData().position[JointData::HeadYaw],getSensorJointData().position[JointData::HeadPitch]);
  absolute_velocity_buffer.add((last_yaw_pitch - current_yaw_pitch) / getRobotInfo().getBasicTimeStepInSecond());
  last_yaw_pitch = current_yaw_pitch;

  getMotionStatus().head_target_reached = (motion_target - current_yaw_pitch).abs() < params.at_target_threshold;

  bool at_rest = absolute_velocity_buffer.getAverage().abs() < (getMotionStatus().currentMotion == motion::walk ? params.at_rest_threshold_walking : params.at_rest_threshold);

  getMotionStatus().head_got_stuck = (!getMotionStatus().head_target_reached)
                                     && at_rest
                                     && absolute_velocity_buffer.isFull();

  PLOT("HeadMotion:target_changed", target_changed);
  PLOT("HeadMotion:absolute_avg_velocity", absolute_velocity_buffer.getAverage().abs());
  PLOT("HeadMotion:at_rest", at_rest);
  PLOT("HeadMotion:target_reached", getMotionStatus().head_target_reached);
  PLOT("HeadMotion:got_stuck", getMotionStatus().head_got_stuck);
}//end execute

void HeadMotionEngine::hold()
{
  double stiffness = 0.0;    //it was 0.7 in former times

  getMotorJointData().stiffness[JointData::HeadYaw] = stiffness;
  getMotorJointData().stiffness[JointData::HeadPitch] = stiffness;

  getMotorJointData().position[JointData::HeadYaw] = getSensorJointData().position[JointData::HeadYaw];
  getMotorJointData().position[JointData::HeadPitch] = getSensorJointData().position[JointData::HeadPitch];
}

void HeadMotionEngine::gotoPointOnTheGround(const Vector2d& target)
{
  Vector3d pointOnTheGround(target.x,target.y,0.0);

  Vector2d centerOnField;
  //TODO: handle the case if the projection is not possible
  CameraGeometry::imagePixelToFieldCoord(
    getCameraMatrix(),
    getCameraInfo(),
    static_cast<double>(getCameraInfo().getOpticalCenterX()),
    static_cast<double>(getCameraInfo().getOpticalCenterY()),
    0.0,
    centerOnField);

  std::vector<Vector3d > points;
  points.push_back(Vector3d(centerOnField.x,centerOnField.y,0.0));
  points.push_back(pointOnTheGround);

  trajectoryHeadMove(points);
}

// move the head to the position target = (yaw, pitch)
void HeadMotionEngine::gotoAngle(const Vector2d& target)
{
  Vector2d headPos(getMotorJointData().position[JointData::HeadYaw],
                   getMotorJointData().position[JointData::HeadPitch]);

  moveByAngle(target-headPos);
}

void HeadMotionEngine::moveByAngle(const Vector2d& target)
{
  double max_velocity_deg_in_second = params.max_velocity_deg_in_second_slow;
  // calculate depending on the walking speed
  if(getMotionStatus().currentMotion == motion::walk)
  {
    double walking_speed = getMotionStatus().plannedMotion.hip.translation.abs();

    if(walking_speed > params.cutting_velocity)
    {
      max_velocity_deg_in_second = params.max_velocity_deg_in_second_slow;
    } else {
      double t = walking_speed/params.cutting_velocity;
      max_velocity_deg_in_second = (1.0 - t)*params.max_velocity_deg_in_second_slow + t*params.max_velocity_deg_in_second_fast;
    }
  }
  else
  {
    max_velocity_deg_in_second = params.max_velocity_deg_in_second_slow;
  }

  max_velocity_deg_in_second = min(getHeadMotionRequest().velocity, max_velocity_deg_in_second);

  MODIFY("HeadMotionEngine:gotoAngle:max_velocity_deg_in_second", max_velocity_deg_in_second);
  double max_velocity = Math::fromDegrees(max_velocity_deg_in_second)*getRobotInfo().getBasicTimeStepInSecond();

  double stiffness = 0.7;
  MODIFY("HeadMotionEngine:gotoAngle:hardness", stiffness);

  // current position
  Vector2d headPos(
    getMotorJointData().position[JointData::HeadYaw],
    getMotorJointData().position[JointData::HeadPitch]);

  // used by target_reached and got_stuck
  motion_target = headPos + target;

  // calculate the update step (normalize with speed if needed)
  Vector2d update(
    Math::normalize(target.x),
    Math::normalize(target.y));

  if (update.abs() > max_velocity) {
    update = update.normalize(max_velocity);
  }

  // limit the acceleration
  //static Vector2<double> position(headPos);
  /*
  static Vector2<double> position(headPos);
  static Vector2<double> velocity;

  double delta_t = theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();
  Vector2<double> new_position = position + velocity*delta_t;
  Vector2<double> new_velocity = velocity + acceleration*delta_t;


  //Vector2<double> acceleration = update-last_requested_change;
  PLOT("HeadMotionEngine:acc_x", acceleration.x);
  PLOT("HeadMotionEngine:acc_y", acceleration.y);

  double maxAcceleration = 1.0;
  MODIFY("HeadMotionEngine:gotoAngle:maxAcceleration", maxAcceleration);
  if (new_acceleration.abs() > maxAcceleration)
  {
    new_acceleration = new_acceleration.normalize(maxAcceleration);
  }

  // update the state
  velocity = new_velocity;
  acceleration = new_acceleration;

  // apply changes
  headPos += velocity;
  */

  // little filter
  // todo: this filter is unstable, make it a PID or so before use
  double maxAcceleration = 0.1;
  MODIFY("HeadMotionEngine:gotoAngle:maxAcceleration", maxAcceleration);

  static Vector2d velocity;
  velocity = velocity*(1.0-maxAcceleration) + update*maxAcceleration;

  headPos += velocity;

  // set the stiffness
  getMotorJointData().stiffness[JointData::HeadYaw] = stiffness;
  getMotorJointData().stiffness[JointData::HeadPitch] = stiffness;

  // set the joints
  getMotorJointData().position[JointData::HeadYaw] = headPos.x;
  getMotorJointData().position[JointData::HeadPitch] = headPos.y;
}//end moveByAngle

// needed by lookAtWorldPoint
Vector3d HeadMotionEngine::g(double yaw, double pitch, const Vector3d& pointInWorld)
{
  theJointData.position[JointData::HeadYaw]   = getMotorJointData().position[JointData::HeadYaw]   + yaw;
  theJointData.position[JointData::HeadPitch] = getMotorJointData().position[JointData::HeadPitch] + pitch;
  theKinematicChain.theLinks[KinematicChain::Neck].updateFromMother();
  theKinematicChain.theLinks[KinematicChain::Head].updateFromMother();

  CameraMatrix cameraMatrix = CameraGeometry::calculateCameraMatrixFromChestPose(
              theKinematicChain.theLinks[KinematicChain::Torso].M,
              NaoInfo::robotDimensions.cameraTransform[getHeadMotionRequest().cameraID].offset,
              NaoInfo::robotDimensions.cameraTransform[getHeadMotionRequest().cameraID].rotationY,
              getCameraMatrixOffset().body_rot,
              getCameraMatrixOffset().head_rot,
              getCameraMatrixOffset().cam_rot[getHeadMotionRequest().cameraID],
              theJointData.position[JointData::HeadYaw],
              theJointData.position[JointData::HeadPitch],
              getInertialModel().orientation
  );

  cameraMatrix.timestamp = getSensorJointData().timestamp;

  // the point in the image which should point to the pointInWorld
  Vector2d projectionPointInImage(
        getCameraInfo().getOpticalCenterX(),
        getCameraInfo().getOpticalCenterY());

  // projectionPointInImage --> head coordinates
  Vector3d direction = cameraMatrix.rotation * CameraGeometry::imagePixelToCameraCoords(
    getCameraInfo(),
    projectionPointInImage.x,
    projectionPointInImage.y);

  double length = (pointInWorld - cameraMatrix.translation).abs();

  return cameraMatrix.translation + direction.normalize(length);
}//end g

void HeadMotionEngine::lookAtWorldPoint(const Vector3d& origTarget)
{
  // HACK: transform the head motion request to hip from the support foot coordinates
  const Pose3D& lFoot = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M;
  const Pose3D& rFoot = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M;
  Vector3d target(origTarget);

  // left foot is the support foot
  if(getHeadMotionRequest().coordinate == HeadMotionRequest::LFoot) {
    target = lFoot*target;
  }

  if(getHeadMotionRequest().coordinate == HeadMotionRequest::RFoot) {
    target = rFoot*target;
  }

  double y = 0.0;
  double p = 0.0;
  Vector2d x(y,p);
  double epsylon = 1e-8;

  // perform only one GN-Step
  for(int i = 0; i < 1; i++)
  {
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    Vector3d dg11 = g(x.x+epsylon, x.y,  target);
    Vector3d dg12 = g(x.x-epsylon, x.y,  target);
    Vector3d dg1 = (dg11 - dg12) / (2*epsylon);

    Vector3d dg21 = g(x.x, x.y+epsylon,  target);
    Vector3d dg22 = g(x.x, x.y-epsylon,  target);
    Vector3d dg2 = (dg21 - dg22) / (2*epsylon);

    // Dg(x)^T*Dg(x)
    double dgTdg11 = dg1*dg1;
    double dgTdg22 = dg2*dg2;
    double dgTdg12 = dg1*dg2;
    Matrix2x2<double> DgTDg(Vector2d(dgTdg11, dgTdg12), Vector2d(dgTdg12, dgTdg22));

    // g(x) - target
    Vector3d w = g(x.x, x.y, target);
    w -= target;

    // Dg(x)^T*(g(x) - target)
    Vector2d DgTw(dg1*w, dg2*w);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    if(DgTDg.det() <= 1e-13)
    {
      // debug output
      std::cerr << "bad matrix" << std::endl;
    }
    Vector2d z_GN = (-(DgTDg.invert()*DgTw));
    x += z_GN;
  }//end for

  moveByAngle(x);
}//end lookAtWorldPoint

void HeadMotionEngine::lookAtWorldPointSimple(const Vector3d& target)
{
  Vector3d vector = target - getCameraMatrix().translation;

  //camera height for nao -535 for webcam -370
  //double neckHeigth = theBlackBoard.theCameraMatrix.translation.z;

  double yaw = atan2(vector.y, vector.x);

  // distence to the projection on the ground
  double d = sqrt(vector.y*vector.y + vector.x*vector.x);

  double cameraTilt = 0;
  if(getHeadMotionRequest().cameraID == CameraInfo::Bottom)
    cameraTilt = Math::fromDegrees(40.0);

  double pitch = Math::pi_2 - atan2(d, -vector.z) - cameraTilt;

  Vector2d targetPosition(yaw, pitch);
  gotoAngle(targetPosition);
}//end lookAtWorldPointSimple

void HeadMotionEngine::lookAtPoint()
{
  double dev_x = getCameraInfo().getOpticalCenterX() - getHeadMotionRequest().targetPointInImage.x;
  double dev_y = getHeadMotionRequest().targetPointInImage.y - getCameraInfo().getOpticalCenterY();

  double divAngleX = atan2(dev_x, getCameraInfo().getFocalLength());
  double divAngleY = atan2(dev_y, getCameraInfo().getFocalLength());

  Vector2d update(divAngleX, divAngleY);

  moveByAngle(update);
}

// search for something :)
void HeadMotionEngine::search()
{
  const Vector3d& center = getHeadMotionRequest().searchCenter;
  const Vector3d& size = getHeadMotionRequest().searchSize;

  std::vector<Vector3d > points;
  points.push_back(Vector3d(center.x-size.x, center.y-size.y, center.z-size.z));
  points.push_back(Vector3d(center.x-size.x, center.y+size.y, center.z-size.z));
  points.push_back(Vector3d(center.x+size.x, center.y+size.y, center.z+size.z));
  points.push_back(Vector3d(center.x+size.x, center.y-size.y, center.z+size.z));

  if (!getHeadMotionRequest().searchDirection) {
      std::reverse(points.begin(), points.end());
  }

  if ( trajectoryHeadMove(points) ) {
    getMotionStatus().headMotion = HeadMotionRequest::numOfHeadMotion;
  } else {
    getMotionStatus().headMotion = HeadMotionRequest::search;
  }
}

void HeadMotionEngine::randomSearch()
{
    static Vector3<double> randomPoint(0, 0, 0);
    std::vector<Vector3d > points;
    points.push_back(randomPoint);
    if (trajectoryHeadMove(points))
    {
        static double lastDir = 0;
        double minAng = lastDir+Math::pi_2;
        double maxAng = minAng+Math::pi;
        lastDir = Math::random(minAng, maxAng);
        randomPoint += (Vector3d(cos(lastDir), sin(lastDir), 0)).normalize(1000);
        randomPoint.x = Math::clamp(randomPoint.x, 50.0, 500.0);
        randomPoint.y = Math::clamp(randomPoint.y, -500.0,500.0);
        randomPoint.z = Math::clamp(randomPoint.y, 0.0,500.0);
    }
}

// move the head in the specified trajectory
bool HeadMotionEngine::trajectoryHeadMove(const std::vector<Vector3d >& points)
{
  // current state of the head motion
  // indicates the last visited point in the points list
  static int headMotionState;
  // number of cycles since the headMotionState was visited
  static double cycle = 0.0;

  // maximum ground speed for head motion
  double max_speed = 1000.0; // 1m/s
  MODIFY("HeadMotionEngine:trajectoryHeadMove:max_speed", max_speed);

  // return value
  // is true if the end of the trajectory is reached
  bool finished = false;
  int MAXSTATE = static_cast<int> (points.size());

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
  double dist = (points[nextMotionState]-points[headMotionState]).abs();
  double s = (dist == 0)?1:cycle / ( dist / (max_speed*getRobotInfo().getBasicTimeStepInSecond() ));

  if ( s >= 1 )
  {
    headMotionState = nextMotionState;
    cycle = 0;
    if ( headMotionState == 0)
    {
        finished = true;
    }
    //std::cout << headMotionState << std::endl;
  }
  else
  {
    Vector3d lookTo = points[nextMotionState] * s + points[headMotionState] * (1-s);

    lookAtWorldPoint(lookTo);

    DEBUG_REQUEST("HeadMotionEngine:draw_search_points_on_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 20);
      CIRCLE(lookTo.x, lookTo.y, 30);
    );
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
  const Pose3D& cameraTrans = NaoInfo::robotDimensions.cameraTransformation[getHeadMotionRequest().cameraID];
  gotoAngle(Vector2d(0.0, -cameraTrans.rotation.getYAngle()));
}

void HeadMotionEngine::lookStraightAheadWithStabilization()
{
  motion_target = Vector2d(0.0, -getInertialModel().orientation.y);
  gotoAngle(motion_target);
}

// exports g for plotting the function ||g(yaw, pith)-y||
// used to control the head (cmp. look_at_world_point)
void HeadMotionEngine::export_g(){
    std::ofstream os("test.txt",std::ofstream::out);
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
      os << std::endl;
    }
    os.close();
}
