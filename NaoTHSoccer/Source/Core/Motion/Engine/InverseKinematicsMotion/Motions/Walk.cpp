/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "Walk.h"

using namespace InverseKinematic;

Walk::Walk()
:IKMotion(motion::WALK),
theWalkParameters(theParameters.walk),
theWaitLandingCount(0),
theUnsupportedCount(0),
isStopping(false)
{
  
}
  
void Walk::execute(const MotionRequest& motionRequest, MotionStatus& motionStatus)
{
  if ( FSRProtection() ) return;
  
  if ( waitLanding() ) return;
  
  theCoMFeetPose = genCoMFeetTrajectory(motionRequest);
  
  HipFeetPose c = theEngine.controlCenterOfMass(theCoMFeetPose);
  
  theEngine.solveHipFeetIK(c);
  theEngine.copyLegJoints(theMotorJointData.position);
}

bool Walk::FSRProtection()
{
  // no foot on the ground, stop walking
  if ( theWalkParameters.enableFSRProtection &&
    theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE ) 
  {
    //TODO: clear walk?
    return true;
  }
  else
  {
    return false;
  }
}

bool Walk::waitLanding()
{
  bool raiseLeftFoot = theCoMFeetPose.lFoot.translation.z > 0;
  bool raiseRightFoot = theCoMFeetPose.rFoot.translation.z > 0;
  
  // don't raise two feet
  ASSERT( !(raiseLeftFoot && raiseRightFoot) );
  
  bool rightFootSupportable = theBlackBoard.theSupportPolygon.isLeftFootSupportable();
  bool leftFootSupportable = theBlackBoard.theSupportPolygon.isRightFootSupportable();

  bool unSupporting = (raiseLeftFoot && !rightFootSupportable)
                      || (raiseRightFoot && !leftFootSupportable);
                      
  if(unSupporting)
  {
    theUnsupportedCount++;
  }
  else
  {
    theUnsupportedCount = 0;
  }

  if ( theUnsupportedCount > theWalkParameters.maxUnsupportedCount
    && ( theWalkParameters.maxWaitLandingCount < 0 || theWaitLandingCount < theWalkParameters.maxWaitLandingCount) )
  {
    theWaitLandingCount++;
    return true;
  } else
  {
    theWaitLandingCount = 0;
    return false;
  }
}

bool Walk::canStop() const
{
  if (isStopping)
    return true;
    
  // wait until full step finished
  //cycle > numberOfSamples
  return false;
}

CoMFeetPose Walk::genCoMFeetTrajectory(const MotionRequest& motionRequest)
{
  WalkRequest walkRequest = motionRequest.walkRequest;
  ASSERT(!Math::isNan(walkRequest.translation.x));
  ASSERT(!Math::isNan(walkRequest.translation.y));
  ASSERT(!Math::isNan(walkRequest.rotation));
  
  if (motionRequest.id == getId() || !canStop() )
  {
    theZMPFeetPose = walk(walkRequest);
  }
  else
  {
    if (walkRequest.stopWithStand) // should end with typical stand
    {
      theZMPFeetPose = stopWalking();
    }
    else
    {
      currentState = stopped;
    }
  }
  
  CoMFeetPose result = theEngine.controlZMP(theZMPFeetPose);
  return result;
}

ZMPFeetPose Walk::walk(const WalkRequest& req)
{
  currentState = running;
  isStopping = false;
    //stoppingStepFinished = false;
    //stoppingStepCount = 0;
  
  ZMPFeetPose result;
  return result;
}

ZMPFeetPose Walk::stopWalking()
{
  ZMPFeetPose result;
  return result;
}