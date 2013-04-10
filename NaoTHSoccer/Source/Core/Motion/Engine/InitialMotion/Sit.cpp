/**
 * @file Sit.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Sit.h"

using namespace naoth;

Sit::Sit()
  :
  AbstractMotion(motion::sit, getMotionLock()),
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

void Sit::execute()
{
  switch (sitStatus)
  {
    case Move:
      moveToSitPose();
      setCurrentState(motion::running);
      break;

    case SitPoseReady:
      freeJoint(getMotionRequest().id == getId());
      break;

    case Finish:
      setCurrentState(motion::stopped);
    break;
  }//end switch
}//end execute

void Sit::moveToSitPose()
{
  const double sit_time = 3000;//ms

  if ( isStopped() )
  {
    startJoints = getSensorJointData();
  }

  if (movedTime < sit_time)
  {
    double t = movedTime / sit_time;
    for (int i = JointData::RHipYawPitch; i < JointData::numOfJoint; i++) // control legs only
    {
      getMotorJointData().position[i] = (1 - t) * startJoints.position[i] + t * theSitJoints.position[i];
      getMotorJointData().stiffness[i] = safeStiffness[i];
    }
    movedTime += getRobotInfo().basicTimeStep;
  } else
  {
    sitStatus = SitPoseReady;
  }
}//end moveToInitialPose

void Sit::freeJoint(bool freely)
{
  double stiffDelta = getRobotInfo().getBasicTimeStepInSecond();
  if (freely)
  {
    setStiffness(
      getMotorJointData(), getSensorJointData(), 
      freeStiffness, stiffDelta, JointData::RHipYawPitch, JointData::numOfJoint);
  }
  else
  {
    if ( setStiffness(
          getMotorJointData(), getSensorJointData(), 
          maxStiffness, stiffDelta*10, JointData::RHipYawPitch, JointData::numOfJoint) )
    {
      sitStatus = Finish;
    }
  }
}//end freeJoint

