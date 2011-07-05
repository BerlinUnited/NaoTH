/**
* @file MotionSymbols.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MotionSymbols
*/

//DEBUG
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

#include "Tools/Math/Matrix.h"


#include "MotionSymbols.h"

#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Motion/Request/MotionID.h"

using namespace naoth;

void MotionSymbols::registerSymbols(xabsl::Engine& engine)
{
  // register the enum type for head motion
  for(int i = 0; i < HeadMotionRequest::numOfHeadMotion; i++)
  {
    string str("head.motion_type.");
    str.append(HeadMotionRequest::getName((HeadMotionRequest::HeadMotionID)i));
    engine.registerEnumElement("head.motion_type", str.c_str(), i);
  }//end for

  // head motion request
  engine.registerEnumeratedOutputSymbol("head.motion_type", "head.motion_type", &setHeadMotionRequest, &getHeadMotionRequestId);
  engine.registerEnumeratedInputSymbol("head.executed_motion_type","head.motion_type", &getHeadMotionStatus);

  engine.registerDecimalOutputSymbol("head.point_in_image.x", &headMotionRequest.targetPointInImage.x);
  engine.registerDecimalOutputSymbol("head.point_in_image.y", &headMotionRequest.targetPointInImage.y);

  engine.registerDecimalOutputSymbol("head.point_in_world.x", &headMotionRequest.targetPointInTheWorld.x);
  engine.registerDecimalOutputSymbol("head.point_in_world.y", &headMotionRequest.targetPointInTheWorld.y);
  engine.registerDecimalOutputSymbol("head.point_in_world.z", &headMotionRequest.targetPointInTheWorld.z);

  engine.registerDecimalOutputSymbol("head.point_on_grond.x", &headMotionRequest.targetPointOnTheGround.x);
  engine.registerDecimalOutputSymbol("head.point_on_grond.y", &headMotionRequest.targetPointOnTheGround.y);

  engine.registerDecimalOutputSymbol("head.search.center.x", &headMotionRequest.searchCenter.x);
  engine.registerDecimalOutputSymbol("head.search.center.y", &headMotionRequest.searchCenter.y);
  engine.registerDecimalOutputSymbol("head.search.center.z", &headMotionRequest.searchCenter.z);
  engine.registerDecimalOutputSymbol("head.search.size.x", &headMotionRequest.searchSize.x);
  engine.registerDecimalOutputSymbol("head.search.size.y", &headMotionRequest.searchSize.y);
  engine.registerDecimalOutputSymbol("head.search.size.z", &headMotionRequest.searchSize.z);
  engine.registerBooleanOutputSymbol("head.search.direction", &headMotionRequest.searchDirection);


  // enum type for camera
  engine.registerEnumElement("head.camera.id", "head.camera.id.top", CameraInfo::Top);
  engine.registerEnumElement("head.camera.id", "head.camera.id.bottom", CameraInfo::Bottom);

  // register the ...
  engine.registerEnumeratedOutputSymbol("head.camera.id", "head.camera.id", &setCameraID, &getCameraID);




  // enum type for motion
  for(int i = 0; i <= motion::num_of_motions; i++)
  {
    string str("motion.type.");
    str.append(motion::getName((motion::MotionID)i));
    engine.registerEnumElement("motion.type", str.c_str(), i);
  }//end for

  // motion request
  engine.registerEnumeratedOutputSymbol("motion.type", "motion.type", &setMotionRequestId, &getMotionRequestId);

  engine.registerBooleanOutputSymbol("motion.execute_immediately", &motionRequest.forced);

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


  // walk coordinates origin
  for(int i = 0; i < WalkRequest::numOfCoordinate; i++)
  {
    string str("motion.walk.coordinate.");
    str.append(WalkRequest::getCoordinateName((WalkRequest::Coordinate)i));
    engine.registerEnumElement("motion.walk.coordinate", str.c_str(), i);
  }//end for
  engine.registerEnumeratedOutputSymbol("motion.walk.coordinate", "motion.walk.coordinate", (int*)&motionRequest.walkRequest.coordinate);
  engine.registerBooleanOutputSymbol("motion.walk.stop_with_stand",&motionRequest.standardStand);
  engine.registerDecimalOutputSymbol("motion.standHeight",&motionRequest.standHeight);

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

  // motion status
  engine.registerEnumeratedInputSymbol("executed_motion.type","motion.type", &getMotionStatusId);
  engine.registerDecimalInputSymbol("executed_motion.time", &getMotionStatusTime);
  

  // universal enum type for direction
  engine.registerEnumElement("direction", "direction.right", -1);
  engine.registerEnumElement("direction", "direction.left", 1);

  // kick
  engine.registerDecimalOutputSymbol("motion.kick.point.x", &motionRequest.kickRequest.kickPoint.x);
  engine.registerDecimalOutputSymbol("motion.kick.point.y", &motionRequest.kickRequest.kickPoint.y);
  engine.registerDecimalOutputSymbol("motion.kick.point.z", &motionRequest.kickRequest.kickPoint.z);
  engine.registerDecimalOutputSymbol("motion.kick.direction", &setKickDirection, &getKickDirection);
  engine.registerBooleanOutputSymbol("motion.kick.finish", &motionRequest.kickRequest.finishKick);
  engine.registerEnumeratedOutputSymbol("motion.kick.foot", "direction", (int*)&motionRequest.kickRequest.kickFoot);

  // relativeToLeftFootX
  engine.registerDecimalInputSymbol("relativeToLeftFootX", &relativeToLeftFootX);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToLeftFootX", "relativeToLeftFootX.x", &relativeToLeftFootXx);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToLeftFootX", "relativeToLeftFootX.y", &relativeToLeftFootXy);
  // relativeToLeftFootY
  engine.registerDecimalInputSymbol("relativeToLeftFootY", &relativeToLeftFootY);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToLeftFootY", "relativeToLeftFootY.x", &relativeToLeftFootYx);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToLeftFootY", "relativeToLeftFootY.y", &relativeToLeftFootYy);
  // relativeToLeftFootA
  engine.registerDecimalInputSymbol("relativeToLeftFootA", &relativeToLeftFootA);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToLeftFootA", "relativeToLeftFootA.a", &relativeToLeftFootAa);
  // relativeToRightFootX
  engine.registerDecimalInputSymbol("relativeToRightFootX", &relativeToRightFootX);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToRightFootX", "relativeToRightFootX.x", &relativeToRightFootXx);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToRightFootX", "relativeToRightFootX.y", &relativeToRightFootXy);
  // relativeToRightFootY
  engine.registerDecimalInputSymbol("relativeToRightFootY", &relativeToRightFootY);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToRightFootY", "relativeToRightFootY.x", &relativeToRightFootYx);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToRightFootY", "relativeToRightFootY.y", &relativeToRightFootYy);
  // relativeToRightFootA
  engine.registerDecimalInputSymbol("relativeToRightFootA", &relativeToRightFootA);
  engine.registerDecimalInputSymbolDecimalParameter("relativeToRightFootA", "relativeToRightFootA.a", &relativeToRightFootAa);

  engine.registerBooleanOutputSymbol("motion.plan.avoid_obstacle", &setAvoidObstacle, &getAvoidObstacle);
  isAvoidObstacle = false;

  engine.registerDecimalInputSymbol("motion.head.pitch", &getHeadPitchAngle);
  engine.registerDecimalInputSymbol("motion.head.yaw", &getHeadYawAngle);
  engine.registerDecimalInputSymbol("motion.head.speed", &headSpeed);

}//end registerSymbols

void MotionSymbols::execute()
{
     //Modell-Hack
   double curHeadPitch = sensorJointData.position[JointData::HeadPitch];
   double curHeadYaw   = sensorJointData.position[JointData::HeadYaw];

   double curTime      = frameInfo.getTimeInSeconds();

   //pick randomly one percept out of the buffer, the current information are not yet added to the buffer
   int idx = Math::random(headMotionBuffer.getNumberOfEntries());

   double timeDelta = curTime - headMotionBuffer.getEntry(idx).time;
   ASSERT(timeDelta > 0);
   double appHeadYawSpeed = (curHeadYaw - headMotionBuffer.getEntry(idx).headYaw) / timeDelta;
   double appHeadPitchSpeed = (curHeadPitch - headMotionBuffer.getEntry(idx).headPitch) / timeDelta;

   double normAppHeadYawSpeed = Math::normalize(curHeadYaw - headMotionBuffer.getEntry(idx).headYaw) / timeDelta;
   double normAppHeadPitchSpeed = Math::normalize(curHeadPitch - headMotionBuffer.getEntry(idx).headPitch) / timeDelta;


   Vector2<double> appHeadVelocity(appHeadYawSpeed, appHeadPitchSpeed);
   Vector2<double> normAppHeadVelocity(normAppHeadYawSpeed, normAppHeadPitchSpeed);
    //insertMotion
    HeadMotion headMotion;
    headMotion.time  = curTime;
    headMotion.headPitch  = curHeadPitch;
    headMotion.headYaw    = curHeadYaw;
    headMotionBuffer.add(headMotion);

    headSpeed = Math::toDegrees(appHeadVelocity.abs());
    PLOT("AngleVelocity~Head", headSpeed);

    // set walk character according walk style
    if ( theInstance->motionRequest.id == motion::walk )
    {
      switch(walkStyle)
      {
        case stable:
          theInstance->motionRequest.walkRequest.character = 0;
          break;
        case normal:
          theInstance->motionRequest.walkRequest.character = 0.5;
          break;
        case fast:
          theInstance->motionRequest.walkRequest.character = 1;
          break;
        default: ASSERT(false); break;
      }
    }

    // step control
    if (stepControlFoot !=  none)
    {
      WalkRequest::StepControlRequest& req = theInstance->motionRequest.walkRequest.stepControl;
      req.stepID = theInstance->motionStatus.stepControl.stepID;
      req.target.translation = theInstance->stepControlRequestTarget.translation;
      req.target.rotation = Math::fromDegrees(theInstance->stepControlRequestTarget.rotation);
      req.time = theInstance->stepControlRequestTime;
      req.speedDirection = Math::fromDegrees(theInstance->stepControlRequestSpeedDirection);
      req.moveLeftFoot = (stepControlFoot == left);
    }
}//end update


MotionSymbols* MotionSymbols::theInstance = NULL;

// enum setter und getter
void MotionSymbols::setHeadMotionRequest(int value){ theInstance->headMotionRequest.id = (HeadMotionRequest::HeadMotionID)value; }
int MotionSymbols::getHeadMotionRequestId(){ return (int)(theInstance->headMotionRequest.id); }

int MotionSymbols::getHeadMotionStatus(){ return (int)(theInstance->motionStatus.headMotion); }


void MotionSymbols::setMotionRequestId(int value){ theInstance->motionRequest.id = (motion::MotionID)value; }
int MotionSymbols::getMotionRequestId(){ return (int)(theInstance->motionRequest.id); }

int MotionSymbols::getMotionStatusId(){ return (int)(theInstance->motionStatus.currentMotion); }

double MotionSymbols::getMotionStatusTime(){ return theInstance->frameInfo.getTimeSince(theInstance->motionStatus.time); }

void MotionSymbols::setCameraID(int value){ theInstance->headMotionRequest.cameraID = (CameraInfo::CameraID)value; }
int MotionSymbols::getCameraID(){ return (int)(theInstance->headMotionRequest.cameraID); }


void MotionSymbols::setWalkSpeedRot(double rot)
{
  theInstance->motionRequest.walkRequest.target.rotation = Math::fromDegrees(rot);
}//end setWalkSpeedRot

void MotionSymbols::setWalkSpeedX(double x)
{
  theInstance->motionRequest.walkRequest.target.translation.x = x;
}//end setWalkSpeedX

void MotionSymbols::setWalkSpeedY(double y)
{
  theInstance->motionRequest.walkRequest.target.translation.y = y;
}//end setWalkSpeedY

void MotionSymbols::setKickDirection(double alpha)
{
  theInstance->motionRequest.kickRequest.kickDirection = Math::fromDegrees(alpha);
}//end setKickDirection

double MotionSymbols::getWalkSpeedRot()
{
  return Math::toDegrees(theInstance->motionRequest.walkRequest.target.rotation);
}//end getWalkSpeedRot

double MotionSymbols::getWalkSpeedX()
{
  return theInstance->motionRequest.walkRequest.target.translation.x;
}//end getWalkSpeedX

double MotionSymbols::getWalkSpeedY()
{
  return theInstance->motionRequest.walkRequest.target.translation.y;
}//end getWalkSpeedY

double MotionSymbols::getKickDirection()
{
  return Math::toDegrees(theInstance->motionRequest.kickRequest.kickDirection);
}//end getKickDirection

double MotionSymbols::relativeToLeftFootX()
{
  const Kinematics::Link& l = theInstance->kinematicChain.theLinks[KinematicChain::LFoot];
  return relativeToX(theInstance->relativeToLeftFootXx, theInstance->relativeToLeftFootXy, l.R, l.p);
}

double MotionSymbols::relativeToLeftFootY()
{
  const Kinematics::Link& l = theInstance->kinematicChain.theLinks[KinematicChain::LFoot];
  return relativeToY(theInstance->relativeToLeftFootYx, theInstance->relativeToLeftFootYy, l.R, l.p);
}

double MotionSymbols::relativeToLeftFootA()
{
  const Kinematics::Link& l = theInstance->kinematicChain.theLinks[KinematicChain::LFoot];
  return relativeToA(theInstance->relativeToLeftFootAa, l.R);
}

double MotionSymbols::relativeToRightFootX()
{
  const Kinematics::Link& l = theInstance->kinematicChain.theLinks[KinematicChain::RFoot];
  return relativeToX(theInstance->relativeToRightFootXx, theInstance->relativeToRightFootXy, l.R, l.p);
}

double MotionSymbols::relativeToRightFootY()
{
  const Kinematics::Link& l = theInstance->kinematicChain.theLinks[KinematicChain::RFoot];
  return relativeToY(theInstance->relativeToRightFootYx, theInstance->relativeToRightFootYy, l.R, l.p);
}

double MotionSymbols::relativeToRightFootA()
{
  const Kinematics::Link& l = theInstance->kinematicChain.theLinks[KinematicChain::RFoot];
  return relativeToA(theInstance->relativeToRightFootAa, l.R);
}

double MotionSymbols::relativeToX(double x, double y, const RotationMatrix& R, const Vector3<double>& p)
{
  return R[0].x * ( x - p.x ) + R[1].x *  ( y - p.y );
}

double MotionSymbols::relativeToY(double x, double y, const RotationMatrix& R, const Vector3<double>& p)
{
  return R[0].y * ( x - p.x ) + R[1].y * ( y - p.y );
}

double MotionSymbols::relativeToA(double a, const RotationMatrix& R)
{
  return Math::toDegrees(Math::normalize(Math::fromDegrees(a) - R.getZAngle()));
}

bool MotionSymbols::avoidObstacle()
{
  WalkRequest& wr = theInstance->motionRequest.walkRequest;
  double dist = wr.target.translation.abs();
  double dir = 0;
  double targetDir = wr.target.translation.angle();
  bool avoid = false;

  unsigned int myNum = theInstance->playerInfo.gameData.playerNumber;
  const PlayersModel& players = theInstance->playersModel;
  unsigned int currentFrameNumber = theInstance->frameInfo.getFrameNumber();

  const double radiusOfRobot = 550;//500;
  // teammates
  for( vector<PlayersModel::Player>::const_iterator iter=players.teammates.begin();
    iter!=players.teammates.end(); ++iter)
  {
    if ( iter->number == myNum ) continue;

    if ( iter->frameInfoWhenWasSeen.getFrameNumber() != currentFrameNumber ) continue;

    double d = iter->pose.translation.abs();
    if ( d < dist )
    {
      double ang = Math::normalizeAngle(iter->pose.translation.angle() - targetDir);
      if (abs(ang) < Math::pi_2)
      {
        double block = abs(d * tan(ang));
        if (block < radiusOfRobot)
        {
          dist = d;
          dir = ang;
          avoid = true;
        }
      }
    }
  }

  // opponents
  for( vector<PlayersModel::Player>::const_iterator iter=players.opponents.begin();
    iter!=players.opponents.end(); ++iter)
  {
    if ( iter->frameInfoWhenWasSeen.getFrameNumber() != currentFrameNumber ) continue;

    double d = iter->pose.translation.abs();
    if (d < dist)
    {
      double ang = Math::normalizeAngle(iter->pose.translation.angle() - targetDir);
      if (abs(ang) < Math::pi_2)
      {
        double block = abs(d * tan(ang));
        if (block < radiusOfRobot)
        {
          dist = d;
          dir = ang;
          avoid = true;
        }
      }
    }
  }

  // todo: avoid goals

  if ( avoid )
  {
    double openAng = atan2(radiusOfRobot,dist);

    ASSERT(openAng<=Math::pi_2);
    ASSERT(openAng>=0);
    ASSERT(abs(dir)<=openAng);
    //openAng = Math::clamp(openAng, 0, Math::fromDegrees(45));
    if ( dir > 0 )
    {
      ASSERT(dir-openAng<0);
      wr.target.translation.rotate(dir-openAng);
    }
    else
    {
      ASSERT(dir+openAng>=0);
      wr.target.translation.rotate(dir+openAng);
    }
    wr.target.translation.normalize(dist);
    
  }

  return avoid;
}

void MotionSymbols::setAvoidObstacle(bool b)
{
  theInstance->isAvoidObstacle = b;
  if ( theInstance->isAvoidObstacle )
  {
    theInstance->isAvoidObstacle = avoidObstacle();
  }
}

bool MotionSymbols::getAvoidObstacle()
{
  return theInstance->isAvoidObstacle;
}

double MotionSymbols::getHeadPitchAngle()
{
  return Math::toDegrees(theInstance->sensorJointData.position[JointData::HeadPitch]);
}

double MotionSymbols::getHeadYawAngle()
{
  return Math::toDegrees(theInstance->sensorJointData.position[JointData::HeadYaw]);
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
