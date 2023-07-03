/**
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
*/
#include "FallMotion.h"

using namespace naoth;

FallMotion::FallMotion()
  :
  AbstractMotion(motion::falling, getMotionLock())
{
  stiffness_increase = getRobotInfo().getBasicTimeStepInSecond() * 5;

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    // retain some stiffness in the head joints in order to turn the head a little during falling.
    if (JointData::getJointID(i) == JointData::HeadPitch || JointData::getJointID(i) == JointData::HeadYaw){
      freeStiffness[i] = 0.3;
      getMotorJointData().position[i] = 0.0;
    }
    else{
      freeStiffness[i] = -1.0;
    }
 
  }
}

void FallMotion::execute()
{
  if(getMotionRequest().id != getId())
  {
    // gradually restore hardness
    if ( setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, stiffness_increase) )
    {
      setCurrentState(motion::stopped);
    }

    // copy sensor positions
    for (size_t i = 0; i < JointData::numOfJoint; i++)
    {
      getMotorJointData().position[i] = getSensorJointData().position[i];
    }

    return;
  }
  else if( isStopped() ) // executed the first time
  {
    // store hardness
      for (size_t i = 0; i < JointData::numOfJoint; i++)
    {
      oldStiffness[i] = getSensorJointData().stiffness[i];
    }
  }

  // set joint free
  setStiffness(getMotorJointData(), getSensorJointData(), freeStiffness, 10);

  setCurrentState(motion::running);
}//end execute
