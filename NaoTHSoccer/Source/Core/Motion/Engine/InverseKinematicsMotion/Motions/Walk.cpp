/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*
*/

#include "Walk.h"
#include "Walk/ZMPPlanner.h"
#include "Walk/FootTrajectoryGenerator.h"
#include "Tools/Debug/DebugModify.h"

using namespace InverseKinematic;
using namespace naoth;

unsigned int Walk::theStepID = 0;

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
  //calculateError();

  bool protecting = FSRProtection();
  if ( protecting )
  {
    if ( !isStopped() )
    {
      cout<<"walk stopped because of fsr protecting"<<endl;
    }
    stepBuffer.clear();
    theEngine.controlZMPclear();
    currentState = motion::stopped;
  }
  else
  {
    if ( !waitLanding() )
    {
      plan(motionRequest);
      theCoMFeetPose = executeStep();
    }

    HipFeetPose c = theEngine.controlCenterOfMass(theCoMFeetPose);

    theEngine.rotationStabilize(c.hip);

    theEngine.solveHipFeetIK(c);
    theEngine.copyLegJoints(theMotorJointData.position);
    theEngine.autoArms(c, theMotorJointData.position);

    // force the hip joint
    if (theMotorJointData.position[JointData::LHipRoll] < 0)
      theMotorJointData.position[JointData::LHipRoll] *= theWalkParameters.leftHipRollSingleSupFactor;

    if (theMotorJointData.position[JointData::RHipRoll] > 0)
      theMotorJointData.position[JointData::RHipRoll] *= theWalkParameters.rightHipRollSingleSupFactor;
  }

  updateMotionStatus(motionStatus);
}

bool Walk::FSRProtection()
{
  if ( !theWalkParameters.enableFSRProtection )
    return false;

  // no foot on the ground,
  // both feet should on the ground, e.g canStop
  // TODO: check current of leg joints
  // ==> stop walking

  static unsigned int noTouchCount = 0;

  if ( theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE
      && noTouchCount < theWalkParameters.minFSRProtectionCount )
  {
    noTouchCount ++;
  }
  else
  {
    noTouchCount = 0;
  }

  if ( !isStopping && canStop() )
  {
    return noTouchCount >= theWalkParameters.minFSRProtectionCount;
  }
  else
  {
    return false;
  }
}

bool Walk::waitLanding()
{
  if ( currentState != motion::running
      || theBlackBoard.theSupportPolygon.mode == SupportPolygon::NONE )
  {
    theUnsupportedCount = 0;
    return false;
  }

  double leftH = theCoMFeetPose.feet.left.translation.z;
  double rightH = theCoMFeetPose.feet.right.translation.z;
  bool raiseLeftFoot = leftH > 0.1 && leftH > rightH;
  bool raiseRightFoot = rightH > 0.1 && rightH > leftH;

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
  MODIFY("walk.character", walkRequest.character);

  ASSERT(walkRequest.character<=1);
  ASSERT(walkRequest.character>=0);
  ASSERT(!Math::isNan(walkRequest.target.translation.x));
  ASSERT(!Math::isNan(walkRequest.target.translation.y));
  ASSERT(!Math::isNan(walkRequest.target.rotation));
  
  if ( motionRequest.id == getId() || !canStop() )
  {
    walk(walkRequest);
    isStopping = false;
    stoppingStepFinished = false;
  }
  else
  {
    if (motionRequest.standardStand) // should end with typical stand
    {
      stopWalking();
    }
    else
    {
      stopWalkingWithoutStand();
    }
  }
}

void Walk::addStep(const Step& step)
{
  stepBuffer.push_back(step);
  theStepID++;
}

void Walk::manageSteps(const WalkRequest& req)
{
  if ( stepBuffer.empty() )
  {
    cout<<"walk start"<<endl;
    theCoMFeetPose = theEngine.getCurrentCoMFeetPose();
    ZMPFeetPose currentZMP = theEngine.getPlannedZMPFeetPose();
    currentZMP.localInLeftFoot();
    currentZMP.zmp.translation.z = theWalkParameters.comHeight;
    Step zeroStep;
    updateParameters(zeroStep, req.character);
    zeroStep.footStep = FootStep(currentZMP.feet, FootStep::NONE);
    int prepareStep = theEngine.controlZMPstart(currentZMP);
    zeroStep.numberOfCyclePerFootStep = prepareStep;
    zeroStep.planningCycle = prepareStep;

    addStep(zeroStep);
    theFootStepPlanner.updateParameters(theParameters);

    // set the stiffness for walking
    for( int i=JointData::RShoulderRoll; i<JointData::numOfJoint; i++)
    {
      theMotorJointData.stiffness[i] = theWalkParameters.stiffness;
    }
  }

  Step& planningStep = stepBuffer.back();
  if ( planningStep.planningCycle >= planningStep.numberOfCyclePerFootStep )
  {
    // this step is planned completely
    // new foot step
    Step step;
    bool stepCtr = false;
    switch (stepBuffer.back().footStep.liftingFoot())
    {
    case FootStep::NONE:
      stepCtr = true;
      break;
    case FootStep::LEFT:
      stepCtr = !req.stepControl.moveLeftFoot;
      break;
    case FootStep::RIGHT:
      stepCtr = req.stepControl.moveLeftFoot;
      break;
    default: ASSERT(false);
      break;
    }

    theFootStepPlanner.updateParameters(theParameters);

    if ( stepCtr && req.stepControl.stepID == theStepID )
    {
      // step control
      step.footStep = theFootStepPlanner.controlStep(planningStep.footStep, req);
      updateParameters(step, req.character);
      step.samplesSingleSupport = max(1, (int) (req.stepControl.time / theBlackBoard.theRobotInfo.basicTimeStep));
      step.numberOfCyclePerFootStep = step.samplesDoubleSupport + step.samplesSingleSupport;
      step.stepControlling = true;
      step.speedDirection = req.stepControl.speedDirection;
    }
    else
    {
      step.footStep = theFootStepPlanner.nextStep(planningStep.footStep, req);
      if ( !isStopping)
        adaptStepSize(step.footStep);
      updateParameters(step, req.character);
    }

    addStep(step);
  }
}

void Walk::planStep()
{
  Step& planningStep = stepBuffer.back();
  ASSERT(planningStep.planningCycle < planningStep.numberOfCyclePerFootStep);
  double zmpOffset = theWalkParameters.ZMPOffsetY + theWalkParameters.ZMPOffsetYByCharacter * (1-planningStep.character);
  Vector2d zmp = ZMPPlanner::simplest(planningStep.footStep, theParameters.hipOffsetX, zmpOffset);
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
  ASSERT(executingStep.executingCycle < executingStep.planningCycle);

  FootStep& exeFootStep = executingStep.footStep;
  Pose3D* liftFoot = NULL;

  CoMFeetPose result;
  bool footSupporting;
  switch(exeFootStep.liftingFoot())
  {
  case FootStep::LEFT:
  {
    liftFoot = &result.feet.left;
    result.feet.right = exeFootStep.supFoot();
    footSupporting = theBlackBoard.theSupportPolygon.isRightFootSupportable();
    break;
  }
  case FootStep::RIGHT:
  {
    liftFoot = &result.feet.right;
    result.feet.left = exeFootStep.supFoot();
    footSupporting = theBlackBoard.theSupportPolygon.isLeftFootSupportable();
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

    // checking on when the foot is lifting
    if ( !executingStep.lifted ) // the foot is on the ground now
    {
      double doubleSupportEnd = executingStep.samplesDoubleSupport / 2 + executingStep.extendDoubleSupport;
      if( executingStep.executingCycle > doubleSupportEnd ) // want to lift the foot
      {
        int maxExtendSamples = static_cast<int>( theWalkParameters.maxExtendDoubleSupportTime / theBlackBoard.theRobotInfo.basicTimeStep );
        if( !footSupporting // but another foot can not support
            && executingStep.extendDoubleSupport < maxExtendSamples ) // allow to extend double support
        {
          executingStep.extendDoubleSupport++;
        }
        else
        {
          executingStep.lifted = true;
          //cout<<"extend "<<executingStep.extendDoubleSupport<<"/"<<maxExtendSamples<<endl;
        }
      }
    }

    if ( executingStep.stepControlling )
    {
      *liftFoot = FootTrajectorGenerator::stepControl(exeFootStep.footBegin(),
                                                        exeFootStep.footEnd(),
                                                        executingStep.executingCycle,
                                                        executingStep.samplesDoubleSupport,
                                                        executingStep.samplesSingleSupport,
                                                        executingStep.extendDoubleSupport,
                                                        theWalkParameters.stepHeight, 0, 0, 0,
                                                        theWalkParameters.curveFactor,
                                                        executingStep.speedDirection);
    }
    else
    {
      *liftFoot = FootTrajectorGenerator::genTrajectory(exeFootStep.footBegin(),
                                                        exeFootStep.footEnd(),
                                                        executingStep.executingCycle,
                                                        executingStep.samplesDoubleSupport,
                                                        executingStep.samplesSingleSupport,
                                                        executingStep.extendDoubleSupport,
                                                        theWalkParameters.stepHeight, 0, 0, 0,
                                                        theWalkParameters.curveFactor);
    }
  }

  result.com.translation = com;
  result.com.rotation = calculateBodyRotation(result.feet ,executingStep.bodyPitchOffset);

  executingStep.executingCycle++;
  if ( executingStep.executingCycle >= executingStep.numberOfCyclePerFootStep )
  {
    ASSERT( stepBuffer.size() > 1);
    // this step is executed
    stepBuffer.pop_front();
    //theCoMErr /= numberOfCyclePerFootStep;
    theCoMErr = Vector3d();
  }

  return result;
}

RotationMatrix Walk::calculateBodyRotation(const FeetPose& feet, double pitch) const
{
  double rAng = feet.left.rotation.getZAngle();
  double lAng = feet.right.rotation.getZAngle();
  double bodyRotation = Math::calculateMeanAngle(rAng, lAng);
  if (abs(Math::normalizeAngle(bodyRotation - lAng)) > Math::pi_2)
  {
    bodyRotation = Math::normalizeAngle(bodyRotation + Math::pi);
  }
  RotationMatrix rot = RotationMatrix::getRotationZ(bodyRotation);
  rot.rotateY(pitch);
  return rot;
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

  if ( currentState == motion::stopped )
    return;

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
    stoppingRequest.target = Pose2D();
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
        Step& lastStep = stepBuffer.back();
        lastStep.footStep = zeroStep;
        lastStep.numberOfCyclePerFootStep = 0;
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
    Pose3D finalLeftFoot = stepBuffer.back().footStep.end().left;
    CoMFeetPose standPose = getStandPose(theWalkParameters.comHeight);
    standPose.localInLeftFoot();
    Pose3D finalBody = finalLeftFoot * standPose.com;
    // wait for the com stops
    if ( theEngine.controlZMPstop(finalBody.translation) )
    {
      currentState = motion::stopped;
      stepBuffer.clear();
      cout<<"walk stopped (standard)"<<endl;
    }
    else
    {
      Step& lastStep = stepBuffer.back();
      lastStep.planningCycle++;
      lastStep.numberOfCyclePerFootStep = lastStep.planningCycle;
    }
  }

  isStopping = true;
}

void Walk::stopWalkingWithoutStand()
{
  ////////////////////////////////////////////////////////
  // add one step to get stand pose
  ///////////////////////////////////////////////////////

  if ( currentState == motion::stopped )
    return;

  if ( !isStopping ) // remember the stopping foot
  {
    stoppingRequest.coordinate = WalkRequest::Hip;
    stoppingRequest.target = Pose2D();
  }

  if ( !stoppingStepFinished )
  {
    // make stopping step
    manageSteps(stoppingRequest);

    const Step& planningStep = stepBuffer.back();
    if ( planningStep.planningCycle == 0 )
    {
      // just modify last planning step
      FootStep zeroStep(planningStep.footStep.begin(), FootStep::NONE);
      Step& lastStep = stepBuffer.back();
      lastStep.footStep = zeroStep;
      lastStep.numberOfCyclePerFootStep = 0;
      stoppingStepFinished = true;
    }
  }

  if ( !stoppingStepFinished )
  {
    planStep();
  }
  else
  {
    const Step& finalStep = stepBuffer.back();
    const FeetPose& finalFeet = finalStep.footStep.end();
    Pose3D finalBody = calculateStableCoMByFeet(finalFeet, finalStep.bodyPitchOffset);

    // wait for the com stops
    if ( theEngine.controlZMPstop(finalBody.translation) )
    {
      currentState = motion::stopped;
      stepBuffer.clear();
      cout<<"walk stopped"<<endl;
    }
    else
    {
      Step& lastStep = stepBuffer.back();
      lastStep.planningCycle++;
      lastStep.numberOfCyclePerFootStep = lastStep.planningCycle;
    }
  }

  isStopping = true;
}

void Walk::updateParameters(Step& step, double character) const
{
  ASSERT(character<=1);
  ASSERT(character>=0);
  step.character = character;

  const unsigned int basicTimeStep = theBlackBoard.theRobotInfo.basicTimeStep;
  
  step.bodyPitchOffset = Math::fromDegrees(theParameters.bodyPitchOffset);
  step.samplesDoubleSupport = max(0, (int) (theWalkParameters.doubleSupportTime / basicTimeStep));
  step.samplesSingleSupport = max(1, (int) (theWalkParameters.singleSupportTime / basicTimeStep));
  int extendDoubleSupportByCharacter = max(0, (int)((theWalkParameters.extendDoubleSupportTimeByCharacter / basicTimeStep)
                                                    *(1-character)));

  ASSERT(extendDoubleSupportByCharacter < step.samplesSingleSupport);
  step.samplesDoubleSupport += extendDoubleSupportByCharacter;
  step.samplesSingleSupport -= extendDoubleSupportByCharacter;
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

void Walk::updateMotionStatus(MotionStatus& motionStatus)
{
  if ( stepBuffer.empty() )
  {
    motionStatus.plannedMotion.lFoot = Pose2D();
    motionStatus.plannedMotion.rFoot = Pose2D();
    motionStatus.plannedMotion.hip = Pose2D();
  }
  else
  {
    FeetPose lastFeet = stepBuffer.back().footStep.end();
    Pose3D lastCom = calculateStableCoMByFeet(lastFeet, Math::fromDegrees(theParameters.bodyPitchOffset));
    Pose3D plannedHip = theCoMFeetPose.com.invert() * lastCom;
    Pose3D plannedlFoot = theCoMFeetPose.feet.left.invert() * lastFeet.left;
    Pose3D plannedrFoot = theCoMFeetPose.feet.right.invert() * lastFeet.right;

    motionStatus.plannedMotion.hip = reduceDimen(plannedHip);
    motionStatus.plannedMotion.lFoot =  reduceDimen(plannedlFoot);
    motionStatus.plannedMotion.rFoot = reduceDimen(plannedrFoot);
  }

  // step control
  motionStatus.stepControl.stepID = theStepID;
  if ( stepBuffer.empty() )
  {
    motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
  }
  else
  {
    FootStep::Foot lastMovingFoot = stepBuffer.back().footStep.liftingFoot();
    switch(lastMovingFoot)
    {
    case FootStep::NONE:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
      break;
    case FootStep::LEFT:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::RIGHT;
      break;
    case FootStep::RIGHT:
      motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::LEFT;
      break;
    default: ASSERT(false);
      break;
    }
  }
}

Pose3D Walk::calculateStableCoMByFeet(FeetPose feet, double pitch) const
{
  feet.left.translate(theParameters.hipOffsetX, 0 ,0);
  feet.right.translate(theParameters.hipOffsetX, 0 ,0);
  Pose3D com;
  com.rotation = calculateBodyRotation(feet, pitch);
  com.translation = (feet.left.translation + feet.right.translation) * 0.5;
  com.translation.z = theWalkParameters.comHeight;
  return com;
}

void Walk::adaptStepSize(FootStep& step) const
{
  Vector3<double> comRef, comObs;

  if ( theCoMFeetPose.feet.left.translation.z > theCoMFeetPose.feet.right.translation.z )
  {
    comRef = theCoMFeetPose.feet.right.invert() * theCoMFeetPose.com.translation;
    const Pose3D& foot = theBlackBoard.theKinematicChain.theLinks[KinematicChain::RFoot].M;
    comObs = foot.invert() * theBlackBoard.theKinematicChain.CoM;
  }
  else
  {
    comRef = theCoMFeetPose.feet.left.invert() * theCoMFeetPose.com.translation;
    const Pose3D& foot = theBlackBoard.theKinematicChain.theLinks[KinematicChain::LFoot].M;
    comObs = foot.invert() * theBlackBoard.theKinematicChain.CoM;
  }

  Vector3<double> comErr = comRef - comObs;

//  PLOT("comErr.x",comErr.x);
//  PLOT("comErr.y",comErr.y);

  double k = -1;
  MODIFY("adaptStepSizeK", k);

  Vector3d comErrG = step.supFoot().rotation * comErr;

  step.footEnd().translation.x += (comErrG.x * k);
  step.footEnd().translation.y += (comErrG.y * k);
}
