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
  assert(lock.state == motion::stopped);
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
    double d = stiffness[i] - theSensorJointData.stiffness[i];
    if (fabs(d) < delta || i == JointData::HeadPitch || i==JointData::HeadYaw )
    {
      readyJointNum++;
      theMotorJointData.stiffness[i] = stiffness[i];
    } else
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

  return readyJointNum == (end - begin);
}//end setStiffness
