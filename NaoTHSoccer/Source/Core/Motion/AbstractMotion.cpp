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

bool AbstractMotion::setHardness(double* hardness, double delta)
{
  int readyJointNum = 0;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    double d = hardness[i] - theBlackBoard.theSensorJointData.hardness[i];
    if (fabs(d) < delta || i == JointData::HeadPitch || i==JointData::HeadYaw )
    {
      readyJointNum++;
      theMotorJointData.hardness[i] = hardness[i];
//      cout<<"ready "<<i<<"("<<hardness[i]<<")"<<" "<<theMotorJointData.hardness[i]<<" = "<<theBlackBoard.theSensorJointData.hardness[i]<<" + " <<d<<endl;
    } else
    {
      d = Math::clamp(d, -delta, delta);
      theMotorJointData.hardness[i] = theBlackBoard.theSensorJointData.hardness[i] + d;

      if (theMotorJointData.hardness[i] < 0) // -1 is the special case
      {
        if ( d < 0 )
        {
          theMotorJointData.hardness[i] = -1;
        }
        else
        {
          theMotorJointData.hardness[i] = 0;
        }
      }
    }
  }//end for

  return readyJointNum == JointData::numOfJoint;
}
