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
#define	_StandMotion_H

#include "IKMotion.h"

class StandMotion : public IKMotion
{
public:

  StandMotion()
  : IKMotion(motion::stand),
    totalTime(0),
    time(0),
    height(-1)
  {
  }

  void calculateTrajectory(const MotionRequest& motionRequest)
  {
    // standing
    if (currentState != running || abs(height - motionRequest.standHeight) > 1 ) {
      // init pose
      height = motionRequest.standHeight;
      double comHeight = (motionRequest.standHeight < 0.0) ? theParameters.walk.comHeight : motionRequest.standHeight;
      targetPose = getStandPose(comHeight);
      startPose = theEngine.getCurrentCoMFeetPose();
      targetPose.localInCoM();
      startPose.localInCoM();

      double speed = theParameters.stand.speed;
      double distLeft = (targetPose.feet.left.translation - startPose.feet.left.translation).abs();
      double distRight = (targetPose.feet.right.translation - startPose.feet.right.translation).abs();
      double distMax = (distLeft > distRight) ? distLeft : distRight;
      totalTime = distMax / speed;
      time = 0;
    }
  }//end calculateTrajectory

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/)
  {
    if (time >= totalTime && motionRequest.id != getId())
    {
      currentState = stopped;
      return;
    }

    calculateTrajectory(motionRequest);

    time += theBlackBoard.theFrameInfo.basicTimeStep;
    double k = min(time / totalTime, 1.0);
    InverseKinematic::CoMFeetPose p = theEngine.interpolate(startPose, targetPose, k);
    InverseKinematic::HipFeetPose c = theEngine.controlCenterOfMass(p);

    // use stablization when at least one foot is on the ground
    if (theBlackBoard.theSupportPolygon.mode != SupportPolygon::NONE) {
      theEngine.rotationStabilize(c.hip);
    }

    theEngine.solveHipFeetIK(c);
    theEngine.copyLegJoints(theMotorJointData.position);

    currentState = running;
  }//end execute

private:
  double totalTime;
  double time;
  double height;
  InverseKinematic::CoMFeetPose targetPose;
  InverseKinematic::CoMFeetPose startPose;
};

#endif	/* _StandMotion_H */

