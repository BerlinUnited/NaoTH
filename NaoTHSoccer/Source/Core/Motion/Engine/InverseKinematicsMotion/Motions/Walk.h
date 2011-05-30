/**
* @file Walk.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "IKMotion.h"

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
  
  InverseKinematic::ZMPFeetPose stopWalking();
  
  bool canStop() const;
  
  InverseKinematic::ZMPFeetPose startToWalk(const WalkRequest& req);
  
  bool chooseStartingFoot(const InverseKinematic::ZMPFeetPose& p, const WalkRequest& req) const;
  
private:
  const IKParameters::Walk& theWalkParameters;
  
  InverseKinematic::ZMPFeetPose theZMPFeetPose;
  InverseKinematic::CoMFeetPose theCoMFeetPose;
  
  int theWaitLandingCount;
  int theUnsupportedCount;
  
  bool isStopping;
  
  // parameters of walk
  double theBodyPitchOffset;
};