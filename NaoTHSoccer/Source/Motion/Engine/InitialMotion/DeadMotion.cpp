/**
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/
#include "DeadMotion.h"

using namespace naoth;

DeadMotion::DeadMotion()
  :
  AbstractMotion(motion::dead, getMotionLock())
{
  stiffness_increase = getRobotInfo().getBasicTimeStepInSecond() * 5;

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    freeStiffness[i] = -1.0;
  }
}

void DeadMotion::execute()
{
  if(getMotionRequest().id != getId())
  {
    // restore hardness
    if ( setStiffness(getMotorJointData(), getSensorJointData(), oldStiffness, stiffness_increase) )
    {
      setCurrentState(motion::stopped);
    }
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      getMotorJointData().position[i] = getSensorJointData().position[i];
    }

    return;
  }else if( isStopped() ) // executed the first time
  {
    // store hardness
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      oldStiffness[i] = getSensorJointData().stiffness[i];
    }//end for
  }

  // set joint free
  setStiffness(getMotorJointData(), getSensorJointData(), freeStiffness, 10);

  setCurrentState(motion::running);
}//end execute
