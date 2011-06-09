/**
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/
#include "DeadMotion.h"

DeadMotion::DeadMotion():
AbstractMotion(motion::dead)
{
  stiffness_increase = theBlackBoard.theFrameInfo.getBasicTimeStepInSecond() * 5;

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    freeStiffness[i] = -1.0;
  }
}

void DeadMotion::execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
{
  if(motionRequest.id != getId())
  {
    // restore hardness
    if ( setStiffness(oldStiffness, stiffness_increase) )
    {
      currentState = stopped;
    }
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      theMotorJointData.position[i] = theBlackBoard.theSensorJointData.position[i];
    }

    return;
  }else if(currentState == stopped) // executed the first time
  {
    // store hardness
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      oldStiffness[i] = theBlackBoard.theSensorJointData.stiffness[i];
    }//end for
  }

  // set joint free
  setStiffness(freeStiffness, 10);

  currentState = running;
}//end execute
