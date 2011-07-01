/**
* @file ProtectFalling.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Remember the pose in fist frame, then apply stablization,
* and also decrease stiffness according to body rotation
*/

#ifndef PROTECTFALLING_H
#define PROTECTFALLING_H

#include "IKMotion.h"

class ProtectFalling: public IKMotion
{
public:

  ProtectFalling()
  : IKMotion(motion::protect_falling)
  {
  }

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
  {
    if ( motionRequest.id != getId() )
    {
      // exit at any time
      currentState = motion::stopped;
      return;
    }

    if ( currentState == motion::stopped )
    {
      // remember the pose
      keepPose = theEngine.getCurrentHipFeetPose();
      for(int i=0; i<naoth::JointData::numOfJoint; i++)
      {
        keepStiffness[i] = theBlackBoard.theSensorJointData.stiffness[i];
      }
    }

    InverseKinematic::HipFeetPose c = keepPose;

    theEngine.rotationStabilize(c.hip);

    theEngine.solveHipFeetIK(c);
    theEngine.copyLegJoints(theMotorJointData.position);
    decreaseStiffness();
    currentState = motion::running;
  }

private:
  /* decrease stiffness according to body rotation */
  void decreaseStiffness()
  {
    const Vector2d& is = theBlackBoard.theInertialPercept.data;
    double angle = max( abs(is.x), abs(is.y) );

    // limit the range
    angle = Math::clamp(angle, 0.0, Math::pi_2);

    double k = 1.5 * cos(angle) - 0.5;
    for( int i=0; i<naoth::JointData::numOfJoint; i++)
    {
      if ( k < 0 )
      {
        theMotorJointData.stiffness[i] = -1;
      }
      else
      {
        theMotorJointData.stiffness[i] = k * keepStiffness[i];
      }
    }
  }

private:
    InverseKinematic::HipFeetPose keepPose;
    double keepStiffness[naoth::JointData::numOfJoint];
};

#endif // PROTECTFALLING_H
