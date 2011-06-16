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
:IKMotion(motion::walk),
theWalkParameters(theParameters.walk),
theWaitLandingCount(0),
theUnsupportedCount(0),
isStopping(false),
stoppingStepFinished(false),
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
  return currentCycle >= numberOfCyclePerFootStep;
}

CoMFeetPose Walk::genCoMFeetTrajectory(const MotionRequest& motionRequest)
{
  WalkRequest walkRequest = motionRequest.walkRequest;
  ASSERT(!Math::isNan(walkRequest.translation.x));
  ASSERT(!Math::isNan(walkRequest.translation.y));
  ASSERT(!Math::isNan(walkRequest.rotation));
  
  CoMFeetPose result;
  if (motionRequest.id == getId() || !canStop() )
  {
    theZMPFeetPose = walk(walkRequest);
    result = theEngine.controlZMP(theZMPFeetPose);
    isStopping = false;
    stoppingStepFinished = false;
  }
  else
  {
    if (walkRequest.stopWithStand) // should end with typical stand
    {
      result = stopWalking();
    }
    else
    {
      currentState = motion::stopped;
    }
  }
  
  return result;
}

ZMPFeetPose Walk::walk(const WalkRequest& req)
{
  if ( currentState == motion::stopped )
  {
    updateParameters();
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
  Pose3D liftFoot = FootTrajectorGenerator::genTrajectory(currentFootStep.footBegin(),
                    currentFootStep.footEnd(),
                    currentCycle, samplesDoubleSupport, samplesSingleSupport,
                    theWalkParameters.stepHeight, 0, 0, 0,
                    theWalkParameters.curveFactor);
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
  result.zmp.rotation.rotateY(bodyPitchOffset);
  
  currentState = motion::running;
  currentCycle++;
  return result;
}

CoMFeetPose Walk::stopWalking()
{
  ////////////////////////////////////////////////////////
  // add one step to get stand pose
  ///////////////////////////////////////////////////////

  ZMPFeetPose stoppingMovment;
  if ( !stoppingStepFinished )
  {
    // make stopping step
    if ( !isStopping ) // remember the stopping foot
    {
      /*
      switch (currentFootStep.liftingFoot()) {
      case FootStep::LEFT:
        stoppingRequest.coordinate = WalkRequest::LFoot;
        break;
      case FootStep::RIGHT:
        stoppingRequest.coordinate = WalkRequest::RFoot;
        break;
      }*/

      stoppingRequest.coordinate = WalkRequest::Hip;
      stoppingRequest.translation.x = 0;
      stoppingRequest.translation.y = 0;
      stoppingRequest.rotation = 0;
    }

    stoppingMovment = walk(stoppingRequest);

    if ( currentCycle == 1 )
    {
      Pose3D diff = currentFootStep.footBegin().invert() * currentFootStep.footEnd();
      if ( diff.translation.abs2() < 1 && diff.rotation.getZAngle() < Math::fromDegrees(1) )
      {
        stoppingStepFinished = true;
      }
    }
  }

  isStopping = true;
  if ( stoppingStepFinished )
  {
    // wait for the preview buffer
    theZMPFeetPose.zmp.translation.y = (theZMPFeetPose.feet.left.translation.y + theZMPFeetPose.feet.right.translation.y)*0.5;
    CoMFeetPose result = theCoMFeetPose;
    if ( theEngine.stopControlZMP(theZMPFeetPose, result) )
    {
      currentState = motion::stopped;
    }
    return result;
  }
  else
  {
    theZMPFeetPose = stoppingMovment;
    return theEngine.controlZMP(theZMPFeetPose);
  }
}

FootStep Walk::firstStep(const WalkRequest& req)
{
  ZMPFeetPose startingZMPFeetPose;
  startingZMPFeetPose = theEngine.getPlannedZMPFeetPose();
  
  //TODO: consider current ZMP
  FootStep step = theFootStepPlanner.firstStep(startingZMPFeetPose.feet, req);
  return step;
}

void Walk::updateParameters()
{
  const unsigned int basicTimeStep = theBlackBoard.theFrameInfo.basicTimeStep;
  
  bodyPitchOffset = Math::fromDegrees(theWalkParameters.bodyPitchOffset);
  samplesDoubleSupport = max(0, (int) (theWalkParameters.doubleSupportTime / basicTimeStep));
  samplesSingleSupport = max(1, (int) (theWalkParameters.singleSupportTime / basicTimeStep));
  numberOfCyclePerFootStep = samplesDoubleSupport + samplesSingleSupport;
  
  theFootStepPlanner.updateParameters(theParameters);
}
