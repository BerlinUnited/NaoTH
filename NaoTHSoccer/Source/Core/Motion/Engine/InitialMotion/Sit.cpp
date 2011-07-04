/**
 * @file Sit.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Sit.h"
#include "PlatformInterface/Platform.h"

using namespace naoth;

Sit::Sit()
  :
  AbstractMotion(motion::sit),
  sitStatus(Move),
  movedTime(0)
{
  theSitJoints.position[JointData::LHipYawPitch] = 0;
  theSitJoints.position[JointData::LHipRoll] = 0;
  theSitJoints.position[JointData::LKneePitch] = theSitJoints.max[JointData::LKneePitch];
  theSitJoints.position[JointData::LAnklePitch] = theSitJoints.min[JointData::LAnklePitch];
  theSitJoints.position[JointData::LHipPitch] = -theSitJoints.position[JointData::LKneePitch] - theSitJoints.position[JointData::LAnklePitch];
  theSitJoints.position[JointData::LAnkleRoll] = 0;
  theSitJoints.position[JointData::RHipYawPitch] = 0;
  theSitJoints.position[JointData::RHipRoll] = 0;
  theSitJoints.position[JointData::RKneePitch] = theSitJoints.max[JointData::RKneePitch];
  theSitJoints.position[JointData::RAnklePitch] = theSitJoints.min[JointData::RAnklePitch];
  theSitJoints.position[JointData::RHipPitch] = -theSitJoints.position[JointData::RKneePitch] - theSitJoints.position[JointData::RAnklePitch];
  theSitJoints.position[JointData::RAnkleRoll] = 0;

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    freeStiffness[i] = -1.0;
    safeStiffness[i] = 0.3;
    maxStiffness[i] = 0.7;
  }

  freeStiffness[JointData::LHipPitch] = 0.1;
  freeStiffness[JointData::RHipPitch] = 0.1;
}

void Sit::execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
{
  switch (sitStatus)
  {
    case Move:
      moveToSitPose();
      currentState = motion::running;
      break;

    case SitPoseReady:
      freeJoint(motionRequest.id == getId());
      break;

    case Finish:
      currentState = motion::stopped;
    break;
  }//end switch
}//end execute

void Sit::moveToSitPose()
{
  const double sit_time = 3000;//ms

  if ( isStopped() )
  {
    startJoints = theBlackBoard.theSensorJointData;
  }

  if (movedTime < sit_time)
  {
    double t = movedTime / sit_time;
    for (int i = JointData::RHipYawPitch; i < JointData::numOfJoint; i++) // control legs only
    {
      theMotorJointData.position[i] = (1 - t) * startJoints.position[i] + t * theSitJoints.position[i];
      theMotorJointData.stiffness[i] = safeStiffness[i];
    }
    movedTime += theBlackBoard.theRobotInfo.basicTimeStep;
  } else
  {
    sitStatus = SitPoseReady;
  }
}//end moveToInitialPose

void Sit::freeJoint(bool freely)
{
  double stiffDelta = theBlackBoard.theRobotInfo.getBasicTimeStepInSecond();
  if (freely)
  {
    setStiffness(freeStiffness, stiffDelta, JointData::RHipYawPitch, JointData::numOfJoint);
  }
  else
  {
    if ( setStiffness(maxStiffness, stiffDelta*10, JointData::RHipYawPitch, JointData::numOfJoint) )
    {
      sitStatus = Finish;
    }
  }
}//end freeJoint

