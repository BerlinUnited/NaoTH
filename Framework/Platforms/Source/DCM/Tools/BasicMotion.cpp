/**
 * @file InitialMotion.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * the initial motion for booting the robot
 */

#include "BasicMotion.h"
#include "Tools/Math/Common.h"

using namespace naoth;

BasicMotion::BasicMotion(
  naoth::MotorJointData& theMotorJointData,
  const naoth::JointData& theStartJointData)
  :
  initStatus(Init),
  init_time(3000.0),
  basicTimeStepInSecond(0.01),
  theMotorJointData(theMotorJointData),
  theStartJointData(theStartJointData) // make a copy
{
  const double LElbowRollMax = Math::fromDegrees(-0.5);
  const double RElbowRollMin = Math::fromDegrees(0.5);
  const double LKneePitchMax = Math::fromDegrees(121.04);
  const double LAnklePitchMin = Math::fromDegrees(-68.15);
  const double RKneePitchMax = Math::fromDegrees(121.47);
  const double RAnklePitchMin = Math::fromDegrees(-67.97);

  theInitJoints.position[JointData::HeadPitch]      = 0;
  theInitJoints.position[JointData::HeadYaw]        = 0;
  theInitJoints.position[JointData::LShoulderPitch] = Math::fromDegrees(90);
  theInitJoints.position[JointData::LShoulderRoll]  = Math::fromDegrees(10);
  theInitJoints.position[JointData::LElbowRoll]     = LElbowRollMax;
  theInitJoints.position[JointData::LElbowYaw]      = Math::fromDegrees(-90);;
  theInitJoints.position[JointData::RShoulderPitch] = Math::fromDegrees(90);
  theInitJoints.position[JointData::RShoulderRoll]  = Math::fromDegrees(-10);
  theInitJoints.position[JointData::RElbowRoll]     = RElbowRollMin;
  theInitJoints.position[JointData::RElbowYaw]      = Math::fromDegrees(90);
  theInitJoints.position[JointData::LHipYawPitch]   = 0;
  theInitJoints.position[JointData::LHipRoll]       = 0;  
  theInitJoints.position[JointData::LKneePitch]     = LKneePitchMax;
  theInitJoints.position[JointData::LAnklePitch]    = LAnklePitchMin;
  theInitJoints.position[JointData::LHipPitch]      = -theInitJoints.position[JointData::LKneePitch] - theInitJoints.position[JointData::LAnklePitch]; 
  theInitJoints.position[JointData::LAnkleRoll]     = 0;
  theInitJoints.position[JointData::RHipYawPitch]   = 0;
  theInitJoints.position[JointData::RHipRoll]       = 0;
  theInitJoints.position[JointData::RKneePitch]     = RKneePitchMax;
  theInitJoints.position[JointData::RAnklePitch]    = RAnklePitchMin;
  theInitJoints.position[JointData::RHipPitch]      = -theInitJoints.position[JointData::RKneePitch] - theInitJoints.position[JointData::RAnklePitch];
  theInitJoints.position[JointData::RAnkleRoll]     = 0;

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    freeStiffness[i] = -1.0;
    safeStiffness[i] = 0.3;
    maxStiffness[i] = 0.7;
  }//end for

  // prevents the robot from falling down
  freeStiffness[JointData::LHipPitch] = 0.1;
  freeStiffness[JointData::RHipPitch] = 0.1;

  // copy the starting values
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    theMotorJointData.position[i] = theStartJointData.position[i];
    theMotorJointData.stiffness[i] = theStartJointData.stiffness[i];
  }

  double maxAngleSpeed = Math::pi_4; // radiant per second
  init_time = getDistance(theStartJointData, theInitJoints)/maxAngleSpeed*1000.0;

  // don't do anything
  if(init_time < 300)
  {
    initStatus = InitialPoseReady;
  }
}

void BasicMotion::execute()
{
  switch (initStatus) 
  {
    case Init:
      if ( setStiffness(safeStiffness, basicTimeStepInSecond*10 ) )
      {
        initStatus = StiffnessReady;
      }
      break;

    case StiffnessReady:
      moveToInitialPose();
      break;

    case InitialPoseReady:
      if(setStiffness(freeStiffness, basicTimeStepInSecond))
      {
        initStatus = Finish;
      }
      break;

    case Finish:
    break;
  }//end switch
}//end execute


void BasicMotion::moveToInitialPose()
{
  static double movedTime = 0;

  if (movedTime < init_time)
  {
    double t = movedTime / init_time;
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      theMotorJointData.position[i] = (1 - t) * theStartJointData.position[i] + t * theInitJoints.position[i];
      theMotorJointData.stiffness[i] = safeStiffness[i];
    }
    movedTime += basicTimeStepInSecond*1000;
  }
  else
  {
    initStatus = InitialPoseReady;
    movedTime = 0;
  }
}//end moveToInitialPose


bool BasicMotion::setStiffness(double* stiffness, double delta)
{
  int readyJointNum = 0;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    double d = stiffness[i] - theMotorJointData.stiffness[i];
    if (fabs(d) < delta || i == JointData::HeadPitch || i==JointData::HeadYaw )
    {
      readyJointNum++;
      theMotorJointData.stiffness[i] = stiffness[i];
    }
    else
    {
      d = Math::clamp(d, -delta, delta);
      theMotorJointData.stiffness[i] = theMotorJointData.stiffness[i] + d;

      if (theMotorJointData.stiffness[i] < 0) // -1 is the special case
      {
        if ( d < 0 )
        {
          theMotorJointData.stiffness[i] = -1;
        }
        else
        {
          theMotorJointData.stiffness[i] = 0;
        }
      }
    }
  }//end for

  // all joints ready
  return readyJointNum == JointData::numOfJoint;
}//end setStiffness

double BasicMotion::getDistance(const JointData& one, const JointData& another)
{
  double distance = 0.0;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    double joindDistance = fabs(Math::normalize(one.position[i] - another.position[i]));
    distance = max(joindDistance, distance);
  }

  return distance;
}//end getDistance

