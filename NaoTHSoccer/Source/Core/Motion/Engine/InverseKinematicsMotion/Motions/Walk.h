/**
* @file Walk.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#ifndef _IK_MOTION_H_
#define _IK_MOTION_H_

#include "IKMotion.h"
#include "Walk/FootStep.h"
#include "Walk/FootStepPlanner.h"

class Walk: public IKMotion
{
public:
  Walk();
  
  virtual void execute(const MotionRequest& motionRequest, MotionStatus& motionStatus);
  
private:
  bool FSRProtection();
  
  bool waitLanding();
  
  InverseKinematic::CoMFeetPose genCoMFeetTrajectory(const MotionRequest& motionRequest);
  
  InverseKinematic::ZMPFeetPose walk(const WalkRequest& req);
  
  InverseKinematic::CoMFeetPose stopWalking();
  
  bool canStop() const;
  
  FootStep firstStep(const WalkRequest& req);
  
  void updateParameters();
  
private:
  const IKParameters::Walk& theWalkParameters;
  
  InverseKinematic::ZMPFeetPose theZMPFeetPose;
  InverseKinematic::CoMFeetPose theCoMFeetPose;
  
  int theWaitLandingCount;
  int theUnsupportedCount;
  
  bool isStopping;
  bool stoppingStepFinished;
  WalkRequest stoppingRequest;
  
  int currentCycle;
  FootStep currentFootStep;
  
  FootStepPlanner theFootStepPlanner;
  
  // calcualted parameters of walk
  double bodyPitchOffset;
  int samplesDoubleSupport;
  int samplesSingleSupport;
  int numberOfCyclePerFootStep;
};

#endif // _IK_MOTION_H_
