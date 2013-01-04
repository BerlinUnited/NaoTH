/**
 * @file InitialMotion.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * the initial motion for booting the robot
 */

#include "InitialMotion.h"

using namespace naoth;
using namespace std;

InitialMotion::InitialMotion() 
  :
  AbstractMotion(motion::init),
  initStatus(Dead),
  init_time(3000.0),
  movedTime(0)
{
  theInitJoints.position[JointData::HeadPitch] = 0;
  theInitJoints.position[JointData::HeadYaw] = 0;
  theInitJoints.position[JointData::LShoulderPitch] = Math::fromDegrees(90);
  theInitJoints.position[JointData::LShoulderRoll] = Math::fromDegrees(10);
  theInitJoints.position[JointData::LElbowRoll] = theInitJoints.max[JointData::LElbowRoll];
  theInitJoints.position[JointData::LElbowYaw] = Math::fromDegrees(-90);
  theInitJoints.position[JointData::RShoulderPitch] = Math::fromDegrees(90);
  theInitJoints.position[JointData::RShoulderRoll] = Math::fromDegrees(-10);
  theInitJoints.position[JointData::RElbowRoll] = theInitJoints.min[JointData::RElbowRoll];
  theInitJoints.position[JointData::RElbowYaw] = Math::fromDegrees(90);
  theInitJoints.position[JointData::LHipYawPitch] = 0;
  theInitJoints.position[JointData::LHipRoll] = 0;  
  theInitJoints.position[JointData::LKneePitch] = theInitJoints.max[JointData::LKneePitch];
  theInitJoints.position[JointData::LAnklePitch] = theInitJoints.min[JointData::LAnklePitch];
  theInitJoints.position[JointData::LHipPitch] = -theInitJoints.position[JointData::LKneePitch] - theInitJoints.position[JointData::LAnklePitch]; 
  theInitJoints.position[JointData::LAnkleRoll] = 0;
  theInitJoints.position[JointData::RHipYawPitch] = 0;
  theInitJoints.position[JointData::RHipRoll] = 0;
  theInitJoints.position[JointData::RKneePitch] = theInitJoints.max[JointData::RKneePitch];
  theInitJoints.position[JointData::RAnklePitch] = theInitJoints.min[JointData::RAnklePitch];
  theInitJoints.position[JointData::RHipPitch] = -theInitJoints.position[JointData::RKneePitch] - theInitJoints.position[JointData::RAnklePitch];
  theInitJoints.position[JointData::RAnkleRoll] = 0;

  // copy ...
  extendJoints = theInitJoints;
  //extendJoints.position[JointData::LShoulderRoll] = Math::fromDegrees(45);
  //extendJoints.position[JointData::RShoulderRoll] = Math::fromDegrees(-45);

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    freeStiffness[i] = -1.0;
    safeStiffness[i] = 0.3;
    maxStiffness[i] = 0.7;
  }

  safeStiffness[JointData::RElbowYaw] = -1;
  safeStiffness[JointData::RElbowRoll] = -1;
  safeStiffness[JointData::LElbowYaw] = -1;
  safeStiffness[JointData::LElbowRoll] = -1;

  freeStiffness[JointData::LHipPitch] = 0.1;
  freeStiffness[JointData::RHipPitch] = 0.1;
}

void InitialMotion::execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
{
  if ( isDanger() ) initStatus = Dead;

  switch (initStatus) 
  {
    case Dead:
      currentState = motion::running;
      dead();
      break;
      
    case Init:
      currentState = motion::running;
      increaseStiffness();
      break;

    case StiffnessReady:
      moveToExtendPose();
      break;

    case ExtendPoseReady:
      moveToInitialPose();
      break;

    case InitialPoseReady:
      freeJoint(motionRequest.id == getId());
      break;

    case Finish:
      currentState = motion::stopped;
    break;
  }//end switch
}//end execute

void InitialMotion::dead()
{
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    theMotorJointData.position[i] = theBlackBoard.theSensorJointData.position[i];
    theMotorJointData.stiffness[i] = -1;
  }

  if ( isSafe() )
  {
    initStatus = Init;
  }
}

void InitialMotion::increaseStiffness()
{
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    theMotorJointData.position[i] = theBlackBoard.theSensorJointData.position[i];
  }

  double stiffDelta = theBlackBoard.theRobotInfo.getBasicTimeStepInSecond() * 10;
  if ( setStiffness(safeStiffness, stiffDelta) )
  {
    initStatus = StiffnessReady;
    startJoints = theBlackBoard.theSensorJointData;
    startJoints.clamp();
    movedTime = 0;
    double maxAngleSpeed = Math::pi_2; // radiant per second
    init_time = getDistance(startJoints, extendJoints)/maxAngleSpeed*1000.0;
  }
}//end increaseHardness

void InitialMotion::moveToExtendPose()
{
  //const double init_time = 1000;//ms

  if (movedTime < init_time)
  {
    double t = movedTime / init_time;
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      theMotorJointData.position[i] = (1 - t) * startJoints.position[i] + t * extendJoints.position[i];
      theMotorJointData.stiffness[i] = safeStiffness[i];
    }
    movedTime += theBlackBoard.theRobotInfo.basicTimeStep;
  } else
  {
    initStatus = ExtendPoseReady;
    movedTime = 0;
  }
}//end moveToInitialPose

void InitialMotion::moveToInitialPose()
{
  const double init_time = 100;//ms

  if (movedTime < init_time)
  {
    double t = movedTime / init_time;
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      theMotorJointData.position[i] = (1 - t) * extendJoints.position[i] + t * theInitJoints.position[i];
      theMotorJointData.stiffness[i] = safeStiffness[i];
    }
    movedTime += theBlackBoard.theRobotInfo.basicTimeStep;
  } else
  {
    initStatus = InitialPoseReady;
  }
}//end moveToInitialPose

void InitialMotion::freeJoint(bool freely)
{
  double stiffDelta = theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();
  if (freely)
  {
    setStiffness(freeStiffness, stiffDelta);
  }
  else
  {
    if ( setStiffness(maxStiffness, stiffDelta*2) )
    {
      initStatus = Finish;
    }
  }
}//end freeJoint

bool InitialMotion::isSafe() const
{
  const double safeAngle = Math::fromDegrees(10);
  return abs(theBlackBoard.theInertialSensorData.data.x) < safeAngle
      && abs(theBlackBoard.theInertialSensorData.data.y) < safeAngle;
}

bool InitialMotion::isDanger() const
{
  const double dangerAngle = Math::fromDegrees(30);
  return abs(theBlackBoard.theInertialSensorData.data.x) > dangerAngle
      || abs(theBlackBoard.theInertialSensorData.data.y) > dangerAngle;
}

double InitialMotion::getDistance(const JointData& one, const JointData& another)
{
  double distance = 0.0;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    double joindDistance = fabs(Math::normalize(one.position[i] - another.position[i]));
    distance = max(joindDistance, distance);
  }

  return distance;
}//end getDistance

