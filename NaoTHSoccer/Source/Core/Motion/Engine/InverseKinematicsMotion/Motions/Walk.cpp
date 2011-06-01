/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "Walk.h"
#include "Walk/ZMPPlanner.h"
#include "Walk/FootTrajectoryGenerator.h"

using namespace InverseKinematic;

Walk::Walk()
:IKMotion(motion::WALK),
theWalkParameters(theParameters.walk),
theWaitLandingCount(0),
theUnsupportedCount(0),
isStopping(false),
currentCycle(0)
{
  updateParameters();
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
  bool raiseLeftFoot = theCoMFeetPose.feet.left.translation.z > 0;
  bool raiseRightFoot = theCoMFeetPose.feet.right.translation.z > 0;
  
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
  if ( currentState == stopped )
  {
    currentFootStep = firstStep(req);
    currentCycle = 0;
  }
  else
  {
    if ( currentCycle >= numberOfCyclePerFootStep )
    {
      // new foot step
      updateParameters();
      currentFootStep = theFootStepPlanner.nextStep(currentFootStep, req);
      currentCycle = 0;
    }
  }
  
  // generate ZMP and Feet trajectory
  ZMPFeetPose result;
  Pose3D liftFoot = FootTrajectorGenerator::genTrajectory(currentFootStep.footBegin(), currentFootStep.footEnd(),
                    currentCycle, samplesDoubleSupport, samplesSingleSupport,
                    stepHeight, footPitchOffset, footYawOffset, footRollOffset, curveFactor);
  switch(currentFootStep.liftingFoot())
  {
    case FootStep::LEFT:
    {
      result.feet.left = liftFoot;
      result.feet.right = currentFootStep.supFoot();
      break;
    }
    case FootStep::RIGHT:
    {
      result.feet.left = currentFootStep.supFoot();
      result.feet.right = liftFoot;
      break;
    }
  }
  
  result.zmp = ZMPPlanner::simplest(currentFootStep, theWalkParameters.comHeight);
  
  // body rotation
  double rAng = result.feet.left.rotation.getZAngle();
  double lAng = result.feet.right.rotation.getZAngle();
  double hipRotation = Math::calculateMeanAngle(rAng, lAng);
  if (abs(Math::normalizeAngle(hipRotation - lAng)) > Math::pi_2)
  {
    hipRotation = Math::normalizeAngle(hipRotation + Math::pi);
  }
  result.zmp.rotation = RotationMatrix::getRotationZ(hipRotation);
  result.zmp.rotation.rotateY(theBodyPitchOffset);
  
  currentState = running;
  isStopping = false;
  //stoppingStepFinished = false;
    //stoppingStepCount = 0;
  currentCycle++;
  return result;
}

ZMPFeetPose Walk::stopWalking()
{
  ZMPFeetPose result;
  return result;
}

FootStep Walk::firstStep(const WalkRequest& req) const
{
  ZMPFeetPose startingZMPFeetPose;
  startingZMPFeetPose = theEngine.getPlannedZMPFeetPose();
  
  //TODO: consider current ZMP
  FootStep step = theFootStepPlanner.firstStep(startingZMPFeetPose.feet, req, theFeetSepWidth);
  return step;
}

void Walk::updateParameters()
{
  theBodyPitchOffset = 0; //TODO
  theFeetSepWidth = NaoInfo::HipOffsetY; //TODO
  samplesDoubleSupport = 4;
  samplesSingleSupport = 20;
  numberOfCyclePerFootStep = samplesDoubleSupport + samplesSingleSupport;
  stepHeight = 20;
  footPitchOffset = 0;
  footYawOffset = 0;
  footRollOffset = 0;
  curveFactor = 10;
}
