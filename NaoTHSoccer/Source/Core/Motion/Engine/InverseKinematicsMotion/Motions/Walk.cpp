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
stoppingStepFinished(false)
{
}
  
void Walk::execute(const MotionRequest& motionRequest, MotionStatus& motionStatus)
{
  if ( FSRProtection() ) return;
  
  if ( waitLanding() ) return;

  calculateError();
  
  plan(motionRequest);

  theCoMFeetPose = executeStep();
  
  HipFeetPose c = theEngine.controlCenterOfMass(theCoMFeetPose);
  
  theEngine.rotationStabilize(c.hip);

  theEngine.solveHipFeetIK(c);
  theEngine.copyLegJoints(theMotorJointData.position);

  // force the hip joint
  if (theMotorJointData.position[JointData::LHipRoll] < 0)
    theMotorJointData.position[JointData::LHipRoll] *= theWalkParameters.leftHipRollSingleSupFactor;

  if (theMotorJointData.position[JointData::RHipRoll] > 0)
    theMotorJointData.position[JointData::RHipRoll] *= theWalkParameters.rightHipRollSingleSupFactor;
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
  bool raiseLeftFoot = theCoMFeetPose.feet.left.translation.z > 0.1;
  bool raiseRightFoot = theCoMFeetPose.feet.right.translation.z > 0.1;

  // don't raise two feet
  ASSERT( !(raiseLeftFoot && raiseRightFoot) );
  
  bool leftFootSupportable = theBlackBoard.theSupportPolygon.isLeftFootSupportable();
  bool rightFootSupportable = theBlackBoard.theSupportPolygon.isRightFootSupportable();

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
  if (isStopping || stepBuffer.empty() )
    return true;
    
  // wait until full step finished
  const Step& planningStep = stepBuffer.back();
  return planningStep.planningCycle >= planningStep.numberOfCyclePerFootStep;
}

void Walk::plan(const MotionRequest& motionRequest)
{
  WalkRequest walkRequest = motionRequest.walkRequest;
  ASSERT(!Math::isNan(walkRequest.translation.x));
  ASSERT(!Math::isNan(walkRequest.translation.y));
  ASSERT(!Math::isNan(walkRequest.rotation));
  
  if (motionRequest.id == getId() || !canStop() )
  {
    walk(walkRequest);
    isStopping = false;
    stoppingStepFinished = false;
  }
  else
  {
    if (walkRequest.stopWithStand) // should end with typical stand
    {
      stopWalking();
    }
    else
    {
      currentState = motion::stopped;
    }
  }
}

void Walk::manageSteps(const WalkRequest& req)
{
  if ( (currentState != motion::running || stepBuffer.empty()) && !isStopping )
  {
    ZMPFeetPose currentZMP = theEngine.getPlannedZMPFeetPose();
    currentZMP.localInLeftFoot();
    Step zeroStep;
    updateParameters(zeroStep);
    zeroStep.footStep = FootStep(currentZMP.feet, FootStep::NONE);
    int prepareStep = theEngine.controlZMPstart(currentZMP);
    if ( prepareStep > 0 )
    {
      zeroStep.numberOfCyclePerFootStep = prepareStep;
      zeroStep.planningCycle = prepareStep;
      stepBuffer.push_back(zeroStep);
    }

    theFootStepPlanner.updateParameters(theParameters);
    Step step;
    step.footStep = firstStep(req);
    updateParameters(step);
    stepBuffer.push_back(step);
    // set the stiffness for walking
    for( int i=JointData::RShoulderRoll; i<JointData::numOfJoint; i++)
    {
      theMotorJointData.stiffness[i] = theWalkParameters.stiffness;
    }
  }
  else if ( !stepBuffer.empty() )
  {
    Step& planningStep = stepBuffer.back();
    if ( planningStep.planningCycle >= planningStep.numberOfCyclePerFootStep )
    {
      // this step is planned completely
      // new foot step
      Step step;
      step.footStep = theFootStepPlanner.nextStep(planningStep.footStep, req);
      theFootStepPlanner.updateParameters(theParameters);
      updateParameters(step);
      stepBuffer.push_back(step);
    }
  }
}

void Walk::planStep()
{
  Step& planningStep = stepBuffer.back();
  Vector2d zmp = ZMPPlanner::simplest(planningStep.footStep, theParameters.hipOffsetX);
  // TODO: change the height?
  theEngine.controlZMPpush(Vector3d(zmp.x, zmp.y, theWalkParameters.comHeight));
  planningStep.planningCycle++;
}

CoMFeetPose Walk::executeStep()
{
  Vector3d com;
  if ( !theEngine.controlZMPpop(com) || stepBuffer.empty() )
  {
    return theEngine.getCurrentCoMFeetPose();
  }

  Step& executingStep = stepBuffer.front();
  FootStep& exeFootStep = executingStep.footStep;
  Pose3D* liftFoot = NULL;

  CoMFeetPose result;
  switch(exeFootStep.liftingFoot())
  {
  case FootStep::LEFT:
  {
    liftFoot = &result.feet.left;
    result.feet.right = exeFootStep.supFoot();
    break;
  }
  case FootStep::RIGHT:
  {
    liftFoot = &result.feet.right;
    result.feet.left = exeFootStep.supFoot();

    break;
  }
  case FootStep::NONE:
  {
    result.feet = exeFootStep.begin();
    break;
  }
  default: ASSERT(false);
  }

  if ( liftFoot != NULL )
  {
    *liftFoot = FootTrajectorGenerator::genTrajectory(exeFootStep.footBegin(),
                                                      exeFootStep.footEnd(),
                                                      executingStep.executingCycle,
                                                      executingStep.samplesDoubleSupport,
                                                      executingStep.samplesSingleSupport,
                                                      theWalkParameters.stepHeight, 0, 0, 0,
                                                      theWalkParameters.curveFactor);
  }

  // body rotation
  double rAng = result.feet.left.rotation.getZAngle();
  double lAng = result.feet.right.rotation.getZAngle();
  double hipRotation = Math::calculateMeanAngle(rAng, lAng);
  if (abs(Math::normalizeAngle(hipRotation - lAng)) > Math::pi_2)
  {
    hipRotation = Math::normalizeAngle(hipRotation + Math::pi);
  }
  result.com.translation = com;
  result.com.rotation = RotationMatrix::getRotationZ(hipRotation);
  result.com.rotation.rotateY(executingStep.bodyPitchOffset);

  executingStep.executingCycle++;
  if ( executingStep.executingCycle >= executingStep.numberOfCyclePerFootStep )
  {
    // this step is executed
    stepBuffer.pop_front();
    //theCoMErr /= numberOfCyclePerFootStep;
    theCoMErr = Vector3d();
  }

  return result;
}

void Walk::walk(const WalkRequest& req)
{
  manageSteps(req);
  planStep();
  currentState = motion::running;
}

void Walk::stopWalking()
{
  ////////////////////////////////////////////////////////
  // add one step to get stand pose
  ///////////////////////////////////////////////////////


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

  if ( !stoppingStepFinished )
  {
    // make stopping step
    manageSteps(stoppingRequest);

    const Step& planningStep = stepBuffer.back();
    if ( planningStep.planningCycle == 0 )
    {
      Pose3D diff = planningStep.footStep.footBegin().invert() * planningStep.footStep.footEnd();
      if ( diff.translation.abs2() < 1 && diff.rotation.getZAngle() < Math::fromDegrees(1) )
      {
        // don't need to move the foot
        FootStep zeroStep(planningStep.footStep.end(), FootStep::NONE);
        stepBuffer.back().footStep = zeroStep;
        stoppingStepFinished = true;
      }
    }
  }

  if ( !stoppingStepFinished )
  {
    planStep();
  }
  else
  {
    Vector3d finalZmp = theEngine.controlZMPback();
    FeetPose feet = stepBuffer.back().footStep.end();
    finalZmp.x = (feet.left.translation.x + feet.right.translation.x)*0.5 + theParameters.hipOffsetX;
    finalZmp.y = (feet.left.translation.y + feet.right.translation.y)*0.5;
    finalZmp.z = theWalkParameters.comHeight;
    // wait for the com stops
    if ( theEngine.controlZMPstop(finalZmp) )
    {
      currentState = motion::stopped;
    }
    else
    {
      stepBuffer.back().numberOfCyclePerFootStep++;
    }
  }

  isStopping = true;
}

FootStep Walk::firstStep(const WalkRequest& req)
{
  ZMPFeetPose startingZMPFeetPose;
  startingZMPFeetPose = theEngine.getPlannedZMPFeetPose();
  
  //TODO: consider current ZMP
  FootStep step = theFootStepPlanner.firstStep(startingZMPFeetPose.feet, req);
  return step;
}

void Walk::updateParameters(Step& step) const
{
  const unsigned int basicTimeStep = theBlackBoard.theFrameInfo.basicTimeStep;
  
  step.bodyPitchOffset = Math::fromDegrees(theParameters.bodyPitchOffset);
  step.samplesDoubleSupport = max(0, (int) (theWalkParameters.doubleSupportTime / basicTimeStep));
  step.samplesSingleSupport = max(1, (int) (theWalkParameters.singleSupportTime / basicTimeStep));
  step.numberOfCyclePerFootStep = step.samplesDoubleSupport + step.samplesSingleSupport;
}

void Walk::calculateError()
{
  if ( currentState != motion::running )
    return;

  bool leftFootSupport = theCoMFeetPose.feet.left.translation.z < 0.1;
  bool rightFootSupport = theCoMFeetPose.feet.right.translation.z < 0.1;

  // at least one support foot
  ASSERT( leftFootSupport || rightFootSupport );

  // calculate error of com
  KinematicChain::LinkID supFoot = leftFootSupport ? KinematicChain::LFoot : KinematicChain::RFoot;
  Pose3D footObs = theBlackBoard.theKinematicChain.theLinks[supFoot].M;
  footObs.translate(0, 0, -NaoInfo::FootHeight);
  Vector3d comObs = footObs.rotation * ( footObs.invert() * theBlackBoard.theKinematicChain.CoM );

  const Pose3D& footRef = leftFootSupport ? theCoMFeetPose.feet.left : theCoMFeetPose.feet.right;
  Vector3d comRef = footRef.invert() * theCoMFeetPose.com.translation;
  Vector3d comErr = comObs - comRef;

  theCoMErr += comErr;
}
