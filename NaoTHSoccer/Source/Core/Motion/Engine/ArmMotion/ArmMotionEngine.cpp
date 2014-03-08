/**
* @file ArmMotionEngine.cpp
*
* @author <a href="mailto:guido.schillaci@informatik.hu-berlin.de">Guido Schillaci</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Definition of class ArmMotionEngine
*/

#include "ArmMotionEngine.h"

#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"


#include <PlatformInterface/Platform.h>

using namespace naoth;
using namespace std;

ArmMotionEngine::ArmMotionEngine()
   //*** :
   //*** theBlackBoard(MotionBlackBoard::getInstance()),
   //*** getMotorJointData()(MotionBlackBoard::getInstance().getMotorJointData()),
   //*** theSensorJointData(MotionBlackBoard::getInstance().theSensorJointData),
   //*** theMotionStatus(MotionBlackBoard::getInstance().theMotionStatus)
{
    //theKinematicChain.init(theJointData, Platform::getInstance().theMassConfig);
    //theKinematicChain.init(theJointData);
}

void ArmMotionEngine::execute()
{
  max_velocity_deg_in_second = getMotionRequest().armMotionRequest.max_velocity_deg_in_sec;
  MODIFY("ArmMotionEngine:max_vel_deg_in_second", max_velocity_deg_in_second);
  max_velocity = Math::fromDegrees(max_velocity_deg_in_second)*getRobotInfo().getBasicTimeStepInSecond();

  switch(getMotionRequest().armMotionRequest.id)
  {

    case ArmMotionRequest::set_left_shoulder_position:
      setLeftShoulderPosition(getMotionRequest().armMotionRequest.lShoulderPosition.x,
                              getMotionRequest().armMotionRequest.lShoulderPosition.y);
      break;
    case ArmMotionRequest::set_left_shoulder_stiffness:
      setLeftShoulderStiffness(getMotionRequest().armMotionRequest.lShoulderStiffness.x,
                               getMotionRequest().armMotionRequest.lShoulderStiffness.y);
      break;
    case ArmMotionRequest::set_left_elbow_position:
      setLeftElbowPosition(getMotionRequest().armMotionRequest.lElbowPosition.x,
                           getMotionRequest().armMotionRequest.lElbowPosition.y);
      break;
    case ArmMotionRequest::set_left_elbow_stiffness:
      setLeftElbowStiffness(getMotionRequest().armMotionRequest.lElbowStiffness.x,
                            getMotionRequest().armMotionRequest.lElbowStiffness.y);
      break;
    case ArmMotionRequest::set_left_arm_joint_position:
      setLeftArmJointPosition(getMotionRequest().armMotionRequest.lShoulderPosition.x, getMotionRequest().armMotionRequest.lShoulderPosition.y,
                              getMotionRequest().armMotionRequest.lElbowPosition.x, getMotionRequest().armMotionRequest.lElbowPosition.y);
      break;
    case ArmMotionRequest::set_left_arm_joint_stiffness:
      setLeftArmJointStiffness(getMotionRequest().armMotionRequest.lShoulderStiffness.x, getMotionRequest().armMotionRequest.lShoulderStiffness.y,
                               getMotionRequest().armMotionRequest.lElbowStiffness.x, getMotionRequest().armMotionRequest.lElbowStiffness.y);
      break;

    case ArmMotionRequest::set_right_shoulder_position:
      setRightShoulderPosition(getMotionRequest().armMotionRequest.rShoulderPosition.x,
                               getMotionRequest().armMotionRequest.rShoulderPosition.y);
      break;
    case ArmMotionRequest::set_right_shoulder_stiffness:
      setRightShoulderStiffness(getMotionRequest().armMotionRequest.rShoulderStiffness.x,
                                getMotionRequest().armMotionRequest.rShoulderStiffness.y);
      break;
    case ArmMotionRequest::set_right_elbow_position:
      setRightElbowPosition(getMotionRequest().armMotionRequest.rElbowPosition.x,
                            getMotionRequest().armMotionRequest.rElbowPosition.y);
      break;
    case ArmMotionRequest::set_right_elbow_stiffness:
      setRightElbowStiffness(getMotionRequest().armMotionRequest.rElbowStiffness.x,
                             getMotionRequest().armMotionRequest.rElbowStiffness.y);
      break;
    case ArmMotionRequest::set_right_arm_joint_position:
      setRightArmJointPosition(getMotionRequest().armMotionRequest.rShoulderPosition.x, getMotionRequest().armMotionRequest.rShoulderPosition.y,
                               getMotionRequest().armMotionRequest.rElbowPosition.x, getMotionRequest().armMotionRequest.rElbowPosition.y);
      break;
    case ArmMotionRequest::set_right_arm_joint_stiffness:
      setRightArmJointStiffness(getMotionRequest().armMotionRequest.rShoulderStiffness.x, getMotionRequest().armMotionRequest.rShoulderStiffness.y,
                                getMotionRequest().armMotionRequest.rElbowStiffness.x, getMotionRequest().armMotionRequest.rElbowStiffness.y);
      break;

    case ArmMotionRequest::set_both_arms_joint_position:
      setBothArmsJointPosition(getMotionRequest().armMotionRequest.lShoulderStiffness.x, getMotionRequest().armMotionRequest.lShoulderStiffness.y,
                               getMotionRequest().armMotionRequest.lElbowStiffness.x, getMotionRequest().armMotionRequest.lElbowStiffness.y,
                               getMotionRequest().armMotionRequest.rShoulderStiffness.x, getMotionRequest().armMotionRequest.rShoulderStiffness.y,
                               getMotionRequest().armMotionRequest.rElbowStiffness.x, getMotionRequest().armMotionRequest.rElbowStiffness.y);
      break;
    case ArmMotionRequest::set_both_arms_joint_stiffness:
      setStiffness(getMotionRequest().armMotionRequest.stiffness);
      break;

    case ArmMotionRequest::hold: hold(); break;

    case ArmMotionRequest::arms_back: armsOnBack(); break;
    case ArmMotionRequest::arms_down: armsDown(); break;

    case ArmMotionRequest::arms_none: // do nothing
    default: break;
  }//end switch


  // copy the requested state
  theMotorJointDataOld = getMotorJointData();
}//end execute 

void ArmMotionEngine::setLeftShoulderPosition(double shoulderPitch, double shoulderRoll) {


    getMotorJointData().position[JointData::LShoulderPitch] = shoulderPitch;
    getMotorJointData().position[JointData::LShoulderRoll] = shoulderRoll;

    return;

    double current_shoulderPitch, current_shoulderRoll;
    // current position
    current_shoulderPitch=getMotorJointData().position[JointData::LShoulderPitch];
    current_shoulderRoll=getMotorJointData().position[JointData::LShoulderRoll];

    // calculate the update step (normalize with speed if needed)
    double update_shoulderPitch = shoulderPitch - current_shoulderPitch;
    update_shoulderPitch = Math::normalize (update_shoulderPitch);

    double update_shoulderRoll = shoulderRoll - current_shoulderRoll;
    update_shoulderRoll = Math::normalize (update_shoulderRoll);

    double vel = sqrt(update_shoulderPitch*update_shoulderPitch +
                      update_shoulderRoll*update_shoulderRoll);

    // normalize
    if ( (vel > max_velocity) && (vel!=0) )
    {
      update_shoulderPitch = (update_shoulderPitch * max_velocity) / vel;
      update_shoulderRoll = (update_shoulderRoll * max_velocity) / vel;
    }

    current_shoulderPitch += update_shoulderPitch;
    current_shoulderRoll += update_shoulderRoll;

    getMotorJointData().position[JointData::LShoulderPitch] = current_shoulderPitch;
    getMotorJointData().position[JointData::LShoulderRoll] = current_shoulderRoll;
}

void ArmMotionEngine::setLeftShoulderStiffness(double shoulderPitch, double shoulderRoll) {
    getMotorJointData().stiffness[JointData::LShoulderPitch] = shoulderPitch;
    getMotorJointData().stiffness[JointData::LShoulderRoll] = shoulderRoll;
}

void ArmMotionEngine::setLeftElbowPosition(double elbowYaw, double elbowRoll) {

    double current_elbowYaw, current_elbowRoll;
    // current position
    current_elbowYaw=getMotorJointData().position[JointData::LElbowYaw];
    current_elbowRoll=getMotorJointData().position[JointData::LElbowRoll];

    // calculate the update step (normalize with speed if needed)
    double update_elbowYaw = elbowYaw - current_elbowYaw;
    update_elbowYaw = Math::normalize (update_elbowYaw);

    double update_elbowRoll = elbowRoll - current_elbowRoll;
    update_elbowRoll = Math::normalize (update_elbowRoll);

    double vel = sqrt(update_elbowYaw*update_elbowYaw+
                      update_elbowRoll*update_elbowRoll);

    // normalize
    if ( (vel > max_velocity) && (vel!=0) )
    {
      update_elbowYaw = (update_elbowYaw * max_velocity) / vel;
      update_elbowRoll = (update_elbowRoll * max_velocity) / vel;
    }

    current_elbowYaw += update_elbowYaw;
    current_elbowRoll += update_elbowRoll;

    getMotorJointData().position[JointData::LElbowYaw] = current_elbowYaw;
    getMotorJointData().position[JointData::LElbowRoll] = current_elbowRoll;
}

void ArmMotionEngine::setLeftElbowStiffness(double elbowYaw, double elbowRoll) {
    getMotorJointData().stiffness[JointData::LElbowYaw] = elbowYaw;
    getMotorJointData().stiffness[JointData::LElbowRoll] = elbowRoll;
}

void ArmMotionEngine::setLeftArmJointPosition(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll) {
    setLeftShoulderPosition(shoulderPitch, shoulderRoll);
    setLeftElbowPosition(elbowYaw, elbowRoll);
}

void ArmMotionEngine::setLeftArmJointStiffness(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll) {
    setLeftShoulderStiffness(shoulderPitch, shoulderRoll);
    setLeftElbowStiffness(elbowYaw, elbowRoll);
}

void ArmMotionEngine::setRightShoulderPosition(double shoulderPitch, double shoulderRoll) {

    double current_shoulderPitch, current_shoulderRoll;
    // current position
    current_shoulderPitch=getMotorJointData().position[JointData::RShoulderPitch];
    current_shoulderRoll=getMotorJointData().position[JointData::RShoulderRoll];

    // calculate the update step (normalize with speed if needed)
    double update_shoulderPitch = shoulderPitch - current_shoulderPitch;
    update_shoulderPitch = Math::normalize (update_shoulderPitch);

    double update_shoulderRoll = shoulderRoll - current_shoulderRoll;
    update_shoulderRoll = Math::normalize (update_shoulderRoll);

    double vel = sqrt(update_shoulderPitch*update_shoulderPitch +
                      update_shoulderRoll*update_shoulderRoll);

    // normalize
    if ( (vel > max_velocity) && (vel!=0) )
    {
      update_shoulderPitch = (update_shoulderPitch * max_velocity) / vel;
      update_shoulderRoll = (update_shoulderRoll * max_velocity) / vel;
    }

    current_shoulderPitch += update_shoulderPitch;
    current_shoulderRoll += update_shoulderRoll;

    getMotorJointData().position[JointData::RShoulderPitch] = current_shoulderPitch;
    getMotorJointData().position[JointData::RShoulderRoll] = current_shoulderRoll;
}

void ArmMotionEngine::setRightShoulderStiffness(double shoulderPitch, double shoulderRoll) {
    getMotorJointData().stiffness[JointData::RShoulderPitch] = shoulderPitch;
    getMotorJointData().stiffness[JointData::RShoulderRoll] = shoulderRoll;
}

void ArmMotionEngine::setRightElbowPosition(double elbowYaw, double elbowRoll) {

    double current_elbowYaw, current_elbowRoll;
    // current position
    current_elbowYaw=getMotorJointData().position[JointData::RElbowYaw];
    current_elbowRoll=getMotorJointData().position[JointData::RElbowRoll];

    // calculate the update step (normalize with speed if needed)
    double update_elbowYaw = elbowYaw - current_elbowYaw;
    update_elbowYaw = Math::normalize (update_elbowYaw);

    double update_elbowRoll = elbowRoll - current_elbowRoll;
    update_elbowRoll = Math::normalize (update_elbowRoll);

    double vel = sqrt(update_elbowYaw*update_elbowYaw+
                      update_elbowRoll*update_elbowRoll);

    // normalize
    if ( (vel > max_velocity) && (vel!=0) )
    {
      update_elbowYaw = (update_elbowYaw * max_velocity) / vel;
      update_elbowRoll = (update_elbowRoll * max_velocity) / vel;
    }

    current_elbowYaw += update_elbowYaw;
    current_elbowRoll += update_elbowRoll;

    getMotorJointData().position[JointData::RElbowYaw] = current_elbowYaw;
    getMotorJointData().position[JointData::RElbowRoll] = current_elbowRoll;
}

void ArmMotionEngine::setRightElbowStiffness(double elbowYaw, double elbowRoll) {
    getMotorJointData().stiffness[JointData::RElbowYaw] = elbowYaw;
    getMotorJointData().stiffness[JointData::RElbowRoll] = elbowRoll;
}

void ArmMotionEngine::setRightArmJointPosition(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll) {
    setRightShoulderPosition(shoulderPitch, shoulderRoll);
    setRightElbowPosition(elbowYaw, elbowRoll);
}

void ArmMotionEngine::setRightArmJointStiffness(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll) {
    setRightShoulderStiffness(shoulderPitch, shoulderRoll);
    setRightElbowStiffness(elbowYaw, elbowRoll);
}

void ArmMotionEngine::setBothArmsJointPosition(double left_shoulderPitch, double left_shoulderRoll, double left_elbowYaw, double left_elbowRoll,
                              double right_shoulderPitch, double right_shoulderRoll, double right_elbowYaw, double right_elbowRoll) {
    setLeftArmJointPosition(left_shoulderPitch, left_shoulderRoll, left_elbowYaw, left_elbowRoll);
    setRightArmJointPosition(right_shoulderPitch, right_shoulderRoll, right_elbowYaw, right_elbowRoll);
}

void ArmMotionEngine::setStiffness(double stiffness)
{
  getMotorJointData().stiffness[JointData::RShoulderPitch] = stiffness;
  getMotorJointData().stiffness[JointData::RShoulderRoll] = stiffness;
  getMotorJointData().stiffness[JointData::RElbowYaw] = stiffness;
  getMotorJointData().stiffness[JointData::RElbowRoll] = stiffness;
  getMotorJointData().stiffness[JointData::LShoulderPitch] = stiffness;
  getMotorJointData().stiffness[JointData::LShoulderRoll] = stiffness;
  getMotorJointData().stiffness[JointData::LElbowYaw] = stiffness;
  getMotorJointData().stiffness[JointData::LElbowRoll] = stiffness;
}

void ArmMotionEngine::hold() 
{
  double stiffness = 0.7;    //it was 0.7 in former times

  getMotorJointData().stiffness[JointData::RShoulderPitch] = stiffness;
  getMotorJointData().stiffness[JointData::RShoulderRoll] = stiffness;
  getMotorJointData().stiffness[JointData::RElbowYaw] = stiffness;
  getMotorJointData().stiffness[JointData::RElbowRoll] = stiffness;
  getMotorJointData().stiffness[JointData::LShoulderPitch] = stiffness;
  getMotorJointData().stiffness[JointData::LShoulderRoll] = stiffness;
  getMotorJointData().stiffness[JointData::LElbowYaw] = stiffness;
  getMotorJointData().stiffness[JointData::LElbowRoll] = stiffness;

  getMotorJointData().position[JointData::RShoulderPitch] =
          getSensorJointData().position[JointData::RShoulderPitch];
  getMotorJointData().position[JointData::RShoulderRoll] =
          getSensorJointData().position[JointData::RShoulderRoll];
  getMotorJointData().position[JointData::RElbowYaw] =
          getSensorJointData().position[JointData::RElbowYaw];
  getMotorJointData().position[JointData::RElbowRoll] =
          getSensorJointData().position[JointData::RElbowRoll];
  getMotorJointData().position[JointData::LShoulderPitch] =
          getSensorJointData().position[JointData::LShoulderPitch];
  getMotorJointData().position[JointData::LShoulderRoll] =
          getSensorJointData().position[JointData::LShoulderRoll];
  getMotorJointData().position[JointData::LElbowYaw] =
          getSensorJointData().position[JointData::LElbowYaw];
  getMotorJointData().position[JointData::LElbowRoll] =
          getSensorJointData().position[JointData::LElbowRoll];
}


bool ArmMotionEngine::armsDown()
{
  static double target[JointData::numOfJoint];
  target[JointData::RShoulderRoll] = Math::fromDegrees(-10);
  target[JointData::LShoulderRoll] = Math::fromDegrees(10);
  target[JointData::RShoulderPitch] = Math::fromDegrees(90);
  target[JointData::LShoulderPitch] = Math::fromDegrees(90);
  target[JointData::RElbowRoll] = Math::fromDegrees(0);
  target[JointData::LElbowRoll] = Math::fromDegrees(0);
  target[JointData::RElbowYaw] = Math::fromDegrees(90);
  target[JointData::LElbowYaw] = Math::fromDegrees(-90);

  for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++) {
    getMotorJointData().stiffness[i] = theArmMotionParams.armStiffness;
  }

  double diffMax = 0.0;
  diffMax = max(diffMax, fabs(target[JointData::RElbowRoll] - theMotorJointDataOld.position[JointData::RElbowRoll]));
  diffMax = max(diffMax, fabs(target[JointData::LElbowRoll] - theMotorJointDataOld.position[JointData::LElbowRoll]));

  bool result = false;

  if( diffMax <= 0.02 )
  {
    result = moveToJoints(target);
  }
  else
  {
    target[JointData::RShoulderPitch] = Math::fromDegrees(119);
    target[JointData::LShoulderPitch] = Math::fromDegrees(119);
    target[JointData::RElbowYaw] = Math::fromDegrees(-25);
    target[JointData::LElbowYaw] = Math::fromDegrees(25);
    moveToJoints(target);
  }

  return result;
}//end armsDown


bool ArmMotionEngine::armsOnBack()
{
  // hack
  static double target[JointData::numOfJoint];
  target[JointData::RShoulderRoll] = Math::fromDegrees(-10);
  target[JointData::LShoulderRoll] = Math::fromDegrees(10);
  target[JointData::RShoulderPitch] = Math::fromDegrees(119);
  target[JointData::LShoulderPitch] = Math::fromDegrees(119);
  target[JointData::RElbowRoll] = Math::fromDegrees(60);
  target[JointData::LElbowRoll] = Math::fromDegrees(-60);
  target[JointData::RElbowYaw] = Math::fromDegrees(-25);
  target[JointData::LElbowYaw] = Math::fromDegrees(25);

  for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++) {
    getMotorJointData().stiffness[i] = theArmMotionParams.armStiffness;
  }

  double diffMax = 0.0;
  diffMax = max(diffMax, fabs(target[JointData::RShoulderPitch] - theMotorJointDataOld.position[JointData::RShoulderPitch]));
  diffMax = max(diffMax, fabs(target[JointData::LShoulderPitch] - theMotorJointDataOld.position[JointData::LShoulderPitch]));
  diffMax = max(diffMax, fabs(target[JointData::RElbowYaw] - theMotorJointDataOld.position[JointData::RElbowYaw]));
  diffMax = max(diffMax, fabs(target[JointData::LElbowYaw] - theMotorJointDataOld.position[JointData::LElbowYaw]));

  bool result = false;

  if( diffMax <= 0.02 )
  {
    target[JointData::RShoulderRoll] = 0.0;
    target[JointData::LShoulderRoll] = 0.0;
    result = moveToJoints(target);
  }
  else
  {
    target[JointData::RElbowRoll] = 0.0;
    target[JointData::LElbowRoll] = 0.0;
    moveToJoints(target);
  }

  return result;
}//end armsOnBack


bool ArmMotionEngine::moveToJoints(const double (&target)[JointData::numOfJoint])
{
  double max_deviation = 0.0;
  double max_speed = Math::fromDegrees(theArmMotionParams.maxJointSpeed) * getRobotInfo().getBasicTimeStepInSecond();

  for (int i = JointData::RShoulderRoll; i <= JointData::LElbowYaw; i++)
  {
    double s = target[i] - theMotorJointDataOld.position[i];
    max_deviation = std::max(max_deviation, std::fabs(s));

    // HACK: move the Yaw joints faster than the others (needed by the armsOnBack motion)
    if(i == JointData::RElbowYaw || i == JointData::LElbowYaw) {
      s = Math::clamp(s, -max_speed*4, max_speed*4);
    } else {
      s = Math::clamp(s, -max_speed, max_speed);
    }
    
    getMotorJointData().position[i] = theMotorJointDataOld.position[i] + s;
  }

  return max_deviation < max_speed;
}//end moveToJoints
