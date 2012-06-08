/**
* @file StandMotion.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*
* the stand motion goes from current pose to the default stand pose.
* running - this motion is cyclic and there is allways running when the motion
*           request is set to stand
* stopped - the motion stopps when the motion request is not stand and the 
*           default stand pose has been reached
* 
*/

#ifndef _StandMotion_H
#define _StandMotion_H

#include "IKMotion.h"
#include "Tools/Debug/DebugBufferedOutput.h"

class StandMotion : public IKMotion
{
public:

  StandMotion()
  : IKMotion(motion::stand),
    totalTime(0),
    time(0),
    height(-1000),
    standardStand(true),
    stiffness(0.7)
  {
  }

  void calculateTrajectory(const MotionRequest& motionRequest)
  {
    // standing
    if ( currentState != motion::running
        || abs(height - motionRequest.standHeight) > 1
        || standardStand != motionRequest.standardStand ) {
      standardStand = motionRequest.standardStand;
      // init pose
      height = motionRequest.standHeight;
      double comHeight = (motionRequest.standHeight < 0.0) ? theParameters.walk.comHeight : motionRequest.standHeight;
      comHeight = Math::clamp(comHeight, 160.0, 270.0); // valid range
      startPose = theEngine.getCurrentCoMFeetPose();
      targetPose = getStandPose(comHeight, standardStand);

      targetPose.localInCoM();
      startPose.localInCoM();

      double speed = theParameters.stand.speed;
      double distLeft = (targetPose.feet.left.translation - startPose.feet.left.translation).abs();
      double distRight = (targetPose.feet.right.translation - startPose.feet.right.translation).abs();
      double distMax = (distLeft > distRight) ? distLeft : distRight;
      totalTime = distMax / speed;
      time = 0;

      // set stiffness
      for( int i=naoth::JointData::RShoulderRoll; i<naoth::JointData::numOfJoint; i++)
      {
        theMotorJointData.stiffness[i] = stiffness;
      }
    }
  }//end calculateTrajectory

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
  {
    if ( time > totalTime && motionRequest.id != getId() )
    {
      currentState = motion::stopped;
      return;
    }

    calculateTrajectory(motionRequest);

    time += theBlackBoard.theRobotInfo.basicTimeStep;
    double k = min(time / totalTime, 1.0);

    InverseKinematic::CoMFeetPose p = theEngine.interpolate(startPose, targetPose, k);
    bool solved = false;
    InverseKinematic::HipFeetPose c = theEngine.controlCenterOfMass(p, solved, false);

    if(theParameters.stand.enableStabilization)
      theEngine.rotationStabilize(c.hip, c.feet.left, c.feet.right);

    theEngine.solveHipFeetIK(c);
    theEngine.copyLegJoints(theMotorJointData.position);
    theEngine.autoArms(c, theMotorJointData.position);


    PLOT("Stand:hip:x",c.hip.translation.x);
    PLOT("Stand:hip:y",c.hip.translation.y);
    PLOT("Stand:hip:z",c.hip.translation.z);


    //if(theParameters.stand.stabilizeNeural)
    //  theEngine.feetStabilize(theMotorJointData.position);

    turnOffStiffnessWhenJointIsOutOfRange();
    currentState = motion::running;
  }//end execute

private:
  void turnOffStiffnessWhenJointIsOutOfRange()
  {
    const double* pos = theMotorJointData.position;
    double* stiff = theMotorJointData.stiffness;
    // Knee pitch
    if ( pos[naoth::JointData::LKneePitch] > theMotorJointData.max[naoth::JointData::LKneePitch] )
    {
      stiff[naoth::JointData::LKneePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::LKneePitch] = stiffness;
    }
    if ( pos[naoth::JointData::RKneePitch] > theMotorJointData.max[naoth::JointData::RKneePitch] )
    {
      stiff[naoth::JointData::RKneePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::RKneePitch] = stiffness;
    }

    // Ankle pitch
    if ( pos[naoth::JointData::LAnklePitch] < theMotorJointData.min[naoth::JointData::LAnklePitch] )
    {
      stiff[naoth::JointData::LAnklePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::LAnklePitch] = stiffness;
    }
    if ( pos[naoth::JointData::RAnklePitch] < theMotorJointData.min[naoth::JointData::RAnklePitch] )
    {
      stiff[naoth::JointData::RAnklePitch] = -1;
    }
    else
    {
      stiff[naoth::JointData::RAnklePitch] = stiffness;
    }
  }

private:
  double totalTime;
  double time;
  double height;
  bool standardStand;
  InverseKinematic::CoMFeetPose targetPose;
  InverseKinematic::CoMFeetPose startPose;
  double stiffness;
};

#endif  /* _StandMotion_H */

