/**
* @file MotionSymbols.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MotionSymbols
*/

// debug


#include "MotionSymbols.h"

using namespace naoth;
using namespace std;

void MotionSymbols::registerSymbols(xabsl::Engine& engine)
{
  // enum type for head motion
  for(int i = 0; i < HeadMotionRequest::numOfHeadMotion; i++)
  {
    string str("head.motion_type.");
    str.append(HeadMotionRequest::getName((HeadMotionRequest::HeadMotionID)i));
    engine.registerEnumElement("head.motion_type", str.c_str(), i);
  }//end for

  // head motion request
  engine.registerEnumeratedOutputSymbol("head.motion_type", "head.motion_type", &setHeadMotionRequest, &getHeadMotionRequestId);
  engine.registerEnumeratedInputSymbol("head.executed_motion_type","head.motion_type", &getHeadMotionStatus);

  engine.registerDecimalOutputSymbol("head.point_in_image.x", &getHeadMotionRequest().targetPointInImage.x);
  engine.registerDecimalOutputSymbol("head.point_in_image.y", &getHeadMotionRequest().targetPointInImage.y);

  engine.registerDecimalOutputSymbol("head.point_in_world.x", &getHeadMotionRequest().targetPointInTheWorld.x);
  engine.registerDecimalOutputSymbol("head.point_in_world.y", &getHeadMotionRequest().targetPointInTheWorld.y);
  engine.registerDecimalOutputSymbol("head.point_in_world.z", &getHeadMotionRequest().targetPointInTheWorld.z);

  engine.registerDecimalOutputSymbol("head.point_on_grond.x", &getHeadMotionRequest().targetPointOnTheGround.x);
  engine.registerDecimalOutputSymbol("head.point_on_grond.y", &getHeadMotionRequest().targetPointOnTheGround.y);

  engine.registerDecimalOutputSymbol("head.search.center.x", &getHeadMotionRequest().searchCenter.x);
  engine.registerDecimalOutputSymbol("head.search.center.y", &getHeadMotionRequest().searchCenter.y);
  engine.registerDecimalOutputSymbol("head.search.center.z", &getHeadMotionRequest().searchCenter.z);
  engine.registerDecimalOutputSymbol("head.search.size.x", &getHeadMotionRequest().searchSize.x);
  engine.registerDecimalOutputSymbol("head.search.size.y", &getHeadMotionRequest().searchSize.y);
  engine.registerDecimalOutputSymbol("head.search.size.z", &getHeadMotionRequest().searchSize.z);
  engine.registerBooleanOutputSymbol("head.search.direction", &getHeadMotionRequest().searchDirection);

  engine.registerDecimalInputSymbol("head.pitch", &getHeadPitchAngle);
  engine.registerDecimalInputSymbol("head.yaw", &getHeadYawAngle);

  engine.registerEnumElement("head.camera", "head.camera.Top", naoth::CameraInfo::Top);
  engine.registerEnumElement("head.camera", "head.camera.Bottom", naoth::CameraInfo::Bottom);
  engine.registerEnumeratedOutputSymbol("head.camera.id", "head.camera", &setCameraId, &getCameraId);

  //arm motion
  // enum type for motion
  for(int i = 0; i <= ArmMotionRequest::numOfArmMotion; i++)
  {
    string str("arm.type.");
    str.append(ArmMotionRequest::getName((ArmMotionRequest::ArmMotionID)i));
    engine.registerEnumElement("arm.type", str.c_str(), i);
  }//end for

  engine.registerEnumeratedOutputSymbol("arm.type", "arm.type", &setArmRequestId, &getArmRequestId);


  // enum type for motion
  for(int i = 0; i <= motion::num_of_motions; i++)
  {
    string str("motion.type.");
    str.append(motion::getName((motion::MotionID)i));
    engine.registerEnumElement("motion.type", str.c_str(), i);
  }//end for

  // motion request
  engine.registerEnumeratedOutputSymbol("motion.type", "motion.type", &setMotionRequestId, &getMotionRequestId);
  engine.registerBooleanOutputSymbol("motion.execute_immediately", &getMotionRequest().forced);

  // walk type
  engine.registerDecimalOutputSymbol("motion.walk_speed.x", &setWalkSpeedX, &getWalkSpeedX);
  engine.registerDecimalOutputSymbol("motion.walk_speed.y", &setWalkSpeedY, &getWalkSpeedY);
  engine.registerDecimalOutputSymbol("motion.walk_speed.rot", &setWalkSpeedRot, &getWalkSpeedRot);

  // walk style
  for(int i = 0; i < num_of_styles; i++)
  {
    string str("motion.walk.style.");
    str.append(getWalkStyleName((WalkStyle)i));
    engine.registerEnumElement("motion.walk.style", str.c_str(), i);
  }//end for
  engine.registerEnumeratedOutputSymbol("motion.walk.style", "motion.walk.style", (int*)&walkStyle);

  // walk offset
  engine.registerDecimalOutputSymbol("motion.walk.offset.x", &getMotionRequest().walkRequest.offset.translation.x);
  engine.registerDecimalOutputSymbol("motion.walk.offset.y", &getMotionRequest().walkRequest.offset.translation.y);
  engine.registerDecimalOutputSymbol("motion.walk.offset.rot", &setWalkOffsetRot, &getWalkOffsetRot);

  // walk coordinates origin
  for(int i = 0; i < WalkRequest::numOfCoordinate; i++)
  {
    string str("motion.walk.coordinate.");
    str.append(WalkRequest::getCoordinateName((WalkRequest::Coordinate)i));
    engine.registerEnumElement("motion.walk.coordinate", str.c_str(), i);
  }//end for
  engine.registerEnumeratedOutputSymbol("motion.walk.coordinate", "motion.walk.coordinate", (int*)&getMotionRequest().walkRequest.coordinate);
  engine.registerBooleanOutputSymbol("motion.walk.stop_with_stand",&getMotionRequest().standardStand);
  engine.registerDecimalOutputSymbol("motion.standHeight",&getMotionRequest().standHeight);

  // step control
  for(int i = 0; i <= none; i++)
  {
    string str("motion.walk.step_control.foot.");
    str.append(getStepControlFootName((StepControlFoot)i));
    engine.registerEnumElement("motion.walk.step_control.foot", str.c_str(), i);
  }//end for
  engine.registerEnumeratedOutputSymbol("motion.walk.step_control.foot", "motion.walk.step_control.foot", (int*)&stepControlFoot);
  engine.registerDecimalOutputSymbol("motion.walk.step_control.target.x", &stepControlRequestTarget.translation.x);
  engine.registerDecimalOutputSymbol("motion.walk.step_control.target.y", &stepControlRequestTarget.translation.y);
  engine.registerDecimalOutputSymbol("motion.walk.step_control.target.rot", &stepControlRequestTarget.rotation);
  engine.registerDecimalOutputSymbol("motion.walk.step_control.time", &stepControlRequestTime);
  engine.registerDecimalOutputSymbol("motion.walk.step_control.speed_direction", &stepControlRequestSpeedDirection);
  engine.registerDecimalOutputSymbol("motion.walk.step_control.scale", &stepControlScale);

  // motion status
  engine.registerEnumeratedInputSymbol("executed_motion.type","motion.type", &getMotionStatusId);
  engine.registerDecimalInputSymbol("executed_motion.time", &getMotionStatusTime);
  engine.registerBooleanInputSymbol("executed_motion.step_control.left_movable", &getMotionStatusLeftMovable);
  engine.registerBooleanInputSymbol("executed_motion.step_control.right_movable", &getMotionStatusRightMovable);

  // universal enum type for direction
  engine.registerEnumElement("direction", "direction.right", -1);
  engine.registerEnumElement("direction", "direction.left", 1);

  // kick
  engine.registerDecimalOutputSymbol("motion.kick.point.x", &getMotionRequest().kickRequest.kickPoint.x);
  engine.registerDecimalOutputSymbol("motion.kick.point.y", &getMotionRequest().kickRequest.kickPoint.y);
  engine.registerDecimalOutputSymbol("motion.kick.point.z", &getMotionRequest().kickRequest.kickPoint.z);
  engine.registerDecimalOutputSymbol("motion.kick.direction", &setKickDirection, &getKickDirection);
  engine.registerBooleanOutputSymbol("motion.kick.finish", &getMotionRequest().kickRequest.finishKick);
  engine.registerEnumeratedOutputSymbol("motion.kick.foot", "direction", (int*)&getMotionRequest().kickRequest.kickFoot);

  // dribble
  engine.registerBooleanOutputSymbol("motion.dribble", &dribble, &dribbleG);
  engine.registerBooleanInputSymbol("motion.dribble.action_done", &dribbleG);
}//end registerSymbols

void MotionSymbols::execute()
{
  // reset provided stuff
  // TODO: maybe better just to create new once?
  getMotionRequest().reset();
  getHeadMotionRequest().reset();
}

void MotionSymbols::updateOutputSymbols()
{
    // set walk character according walk style
  if ( getMotionRequest().id == motion::walk )
  {
    switch(walkStyle)
    {
      case stable:
        getMotionRequest().walkRequest.character = 0.3;
        break;
      case normal:
        getMotionRequest().walkRequest.character = 0.7;
        break;
      case fast:
        getMotionRequest().walkRequest.character = 1;
        break;
      default: ASSERT(false); break;
    }
  }

  // step control
  if (stepControlFoot != none)
  {
    WalkRequest::StepControlRequest& req = getMotionRequest().walkRequest.stepControl;
    req.stepID = getMotionStatus().stepControl.stepID;
    req.target.translation = stepControlRequestTarget.translation;
    req.target.rotation = Math::fromDegrees(stepControlRequestTarget.rotation);
    req.time = (int)(stepControlRequestTime);
    req.speedDirection = Math::fromDegrees(stepControlRequestSpeedDirection);
    req.moveLeftFoot = (stepControlFoot == left);
    req.scale = stepControlScale;
  }
}

MotionSymbols* MotionSymbols::theInstance = NULL;


void MotionSymbols::setHeadMotionRequest(int value) { 
  theInstance->getHeadMotionRequest().id = (HeadMotionRequest::HeadMotionID)value; 
}

int MotionSymbols::getHeadMotionRequestId() { 
  return (int)(theInstance->getHeadMotionRequest().id); 
}

int MotionSymbols::getHeadMotionStatus() { 
  return (int)(theInstance->getMotionStatus().headMotion); 
}

void MotionSymbols::setCameraId(int value) { 
  assert(value >= 0 && value < static_cast<int>(naoth::CameraInfo::numOfCamera));
  theInstance->getHeadMotionRequest().cameraID = static_cast<naoth::CameraInfo::CameraID>(value);
}

int MotionSymbols::getCameraId() { 
  return static_cast<int>(theInstance->getHeadMotionRequest().cameraID); 
}

void MotionSymbols::setArmRequestId(int value) { 
  theInstance->getMotionRequest().armMotionRequest.id = (ArmMotionRequest::ArmMotionID)value; 
}

int MotionSymbols::getArmRequestId() { 
  return (int)(theInstance->getMotionRequest().armMotionRequest.id); 
}

void MotionSymbols::setMotionRequestId(int value) { 
  theInstance->getMotionRequest().id = (motion::MotionID)value; 
}

int MotionSymbols::getMotionRequestId() { 
  return (int)(theInstance->getMotionRequest().id); 
}

int MotionSymbols::getMotionStatusId() { 
  return (int)(theInstance->getMotionStatus().currentMotion); 
}

double MotionSymbols::getMotionStatusTime() { 
  return theInstance->getFrameInfo().getTimeSince(theInstance->getMotionStatus().time); 
}

bool MotionSymbols::getMotionStatusLeftMovable()
{ 
  return theInstance->getMotionStatus().stepControl.moveableFoot == MotionStatus::StepControlStatus::BOTH 
      || theInstance->getMotionStatus().stepControl.moveableFoot == MotionStatus::StepControlStatus::LEFT; 
}

bool MotionSymbols::getMotionStatusRightMovable()
{ 
  return theInstance->getMotionStatus().stepControl.moveableFoot == MotionStatus::StepControlStatus::BOTH 
      || theInstance->getMotionStatus().stepControl.moveableFoot == MotionStatus::StepControlStatus::RIGHT; 
}

void MotionSymbols::setWalkOffsetRot(double rot) {
  theInstance->getMotionRequest().walkRequest.offset.rotation = Math::fromDegrees(rot);
}

double MotionSymbols::getWalkOffsetRot() {
  return Math::toDegrees(theInstance->getMotionRequest().walkRequest.offset.rotation);
}

void MotionSymbols::setWalkSpeedRot(double rot) {
  theInstance->getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(rot);
}

void MotionSymbols::setWalkSpeedX(double x) {
  theInstance->getMotionRequest().walkRequest.target.translation.x = x;
}

void MotionSymbols::setWalkSpeedY(double y) {
  theInstance->getMotionRequest().walkRequest.target.translation.y = y;
}

void MotionSymbols::setKickDirection(double alpha) {
  theInstance->getMotionRequest().kickRequest.kickDirection = Math::fromDegrees(alpha);
}

double MotionSymbols::getWalkSpeedRot() {
  return Math::toDegrees(theInstance->getMotionRequest().walkRequest.target.rotation);
}

double MotionSymbols::getWalkSpeedX() {
  return theInstance->getMotionRequest().walkRequest.target.translation.x;
}

double MotionSymbols::getWalkSpeedY() {
  return theInstance->getMotionRequest().walkRequest.target.translation.y;
}

double MotionSymbols::getKickDirection() {
  return Math::toDegrees(theInstance->getMotionRequest().kickRequest.kickDirection);
}

double MotionSymbols::getHeadPitchAngle() {
  return Math::toDegrees(theInstance->getSensorJointData().position[JointData::HeadPitch]);
}

double MotionSymbols::getHeadYawAngle() {
  return Math::toDegrees(theInstance->getSensorJointData().position[JointData::HeadYaw]);
}

string MotionSymbols::getWalkStyleName(WalkStyle i)
{
  switch(i)
  {
  case stable: return "stable"; break;
  case normal: return "normal"; break;
  case fast: return "fast"; break;
  default: ASSERT(false); break;
  }
  return "unknown";
}

string MotionSymbols::getStepControlFootName(StepControlFoot i)
{
  switch(i)
  {
  case left: return "left"; break;
  case right: return "right"; break;
  case none: return "none"; break;
  default: ASSERT(false); break;
  }
  return "unknown";
}

bool MotionSymbols::dribbleG() 
{
  return ((theInstance->actionPerformed) == (int)(theInstance->getMotionStatus().stepControl.stepID));
}

void MotionSymbols::dribble(bool /*dummy*/)
{
  const Vector2d& ball = theInstance->getBallModel().positionPreviewInRFoot;
  Pose2D& motionTarget = theInstance->getMotionRequest().walkRequest.target;
  
  double offsetX = 190;
  double offsetY = 20;

  if(ball.x - offsetX < 30 && fabs(ball.y - offsetY) < 30 )
  {
    theInstance->stepControlRequestTarget.translation.x = 500;
    theInstance->stepControlRequestTarget.translation.y = 100;
    theInstance->stepControlRequestTarget.rotation = 0;

    theInstance->stepControlRequestTime = 300;
    theInstance->stepControlRequestSpeedDirection = 90;
    theInstance->stepControlFoot = right;

    theInstance->getMotionRequest().walkRequest.coordinate = WalkRequest::RFoot;
    theInstance->walkStyle = fast;

    theInstance->getMotionRequest().id = motion::walk;
    theInstance->actionPerformed = theInstance->getMotionStatus().stepControl.stepID;
  }
  else
  {
    motionTarget.translation.x = ball.x - offsetX;    motionTarget.translation.y = ball.y - offsetY;    motionTarget.rotation = (ball.abs() > 250) ? ball.angle() : 0;
    theInstance->getMotionRequest().walkRequest.coordinate = WalkRequest::RFoot;    theInstance->walkStyle = stable;

    theInstance->getMotionRequest().id = motion::walk;
  }
}