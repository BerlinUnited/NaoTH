/**
 * @file AbstractMotion.cpp
 * 
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "AbstractMotion.h"

using namespace naoth;
using namespace motion;

AbstractMotion::AbstractMotion(MotionID id)
: currentState(stopped),
  theMotorJointData(MotionBlackBoard::getInstance().theMotorJointData),
  theBlackBoard(MotionBlackBoard::getInstance()),
  theId(id),
  finished(false)
{
  init();
}

bool AbstractMotion::setStiffness(double* stiffness, double delta)
{
  int readyJointNum = 0;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    double d = stiffness[i] - theBlackBoard.theSensorJointData.stiffness[i];
    if (fabs(d) < delta || i == JointData::HeadPitch || i==JointData::HeadYaw )
    {
      readyJointNum++;
      theMotorJointData.stiffness[i] = stiffness[i];
    } else
    {
      d = Math::clamp(d, -delta, delta);
      theMotorJointData.stiffness[i] = theBlackBoard.theSensorJointData.stiffness[i] + d;

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

  return readyJointNum == JointData::numOfJoint;
}
