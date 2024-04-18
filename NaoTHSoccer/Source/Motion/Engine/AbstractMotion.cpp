/**
 * @file AbstractMotion.cpp
 * 
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "AbstractMotion.h"

using namespace naoth;

AbstractMotion::AbstractMotion(motion::MotionID id, MotionLock& lock)
: theId(id),
  lock(lock),
  currentState(motion::stopped)
{
  //assert(lock.state == motion::stopped || id == motion::dead);
  // occupy lock
  lock.id = id;
  lock.state = currentState;

  init();
}

bool AbstractMotion::setStiffness(
  naoth::MotorJointData& theMotorJointData,
  const naoth::SensorJointData& theSensorJointData,
  double* stiffness,
  double delta, 
  JointData::JointID begin, JointData::JointID end)
{
  int readyJointNum = 0;
  for (int i = begin; i < end; i++)
  {
    // difference between requested and last sensed stiffness
    double diff = stiffness[i] - theSensorJointData.stiffness[i];

    // NOTE: set the stiffness directly to the joints that are considered not critical
    if(i == JointData::HeadPitch || 
       i == JointData::HeadYaw   || 
       i == JointData::LHand     || 
       i == JointData::RHand     || 
       i == JointData::LWristYaw || 
       i == JointData::RWristYaw) 
    {
      readyJointNum++;
      theMotorJointData.stiffness[i] = stiffness[i];
    }
    // set the stiffness directly if the deviation is small (less than 0.01) or delta < 0
    else if (fabs(diff) < delta + 0.01 || delta < 0) {
      readyJointNum++;
      theMotorJointData.stiffness[i] = stiffness[i];
    } else {
      // limit the change in stiffness
      // ACHTUNG: the change of the stiffness depends on the sensory feedback
      diff = Math::clamp(diff, -delta, delta);
      theMotorJointData.stiffness[i] = theSensorJointData.stiffness[i] + diff;
    }

    // ensure that we always get a valid stiffness, i.e, -1 or [0,1]
    if (theMotorJointData.stiffness[i] < 0) { 
      theMotorJointData.stiffness[i] = (diff > 0) ? 0 : -1; // -1 is the special case
    } else {
      theMotorJointData.stiffness[i] = std::min(theMotorJointData.stiffness[i], 1.0);
    }
  }

  return readyJointNum == (end - begin);
}//end setStiffness

