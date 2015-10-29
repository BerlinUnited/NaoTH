/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#include "Walk.h"
#include "Walk/FootTrajectoryGenerator.h"
#include "Walk/ZMPPlanner.h"

using namespace naoth;
using namespace InverseKinematic;

Walk::Walk() : IKMotion(getInverseKinematicsMotionEngineService(), motion::walk, getMotionLock())
{
}
  
void Walk::execute()
{
  // check the integrity 
  getMotionRequest().walkRequest.assertValid();

  // planing phase
  // add new steps or delete executed ones if necessary
  manageSteps(getMotionRequest());

  // NOTE: check the integrity of the step buffer
  ASSERT(!stepBuffer.empty());
  ASSERT(!stepBuffer.last().isPlanned());
  ASSERT(!stepBuffer.first().isExecuted());

  // running phase
  planZMP();
  executeStep();

  FIELD_DRAWING_CONTEXT;
  stepBuffer.draw(getDebugDrawings());


  // TODO: apply online stabilization
    
  // apply inverse kinematic
  bool solved = false;
  HipFeetPose c = getEngine().controlCenterOfMass(getMotorJointData(), theCoMFeetPose, solved, false);
  getEngine().solveHipFeetIK(c);
  getEngine().copyLegJoints(getMotorJointData().position);

	// set the stiffness for walking
  for( int i = JointData::RShoulderRoll; i < JointData::numOfJoint; i++) {
    getMotorJointData().stiffness[i] = parameters().general.stiffness;
  }

  // WIEDERLICHER HACK: force the hip joint
  if (getMotorJointData().position[JointData::LHipRoll] < 0) {
    getMotorJointData().position[JointData::LHipRoll] *= parameters().general.hipRollSingleSupFactorLeft;
  }
  if (getMotorJointData().position[JointData::RHipRoll] > 0) {
    getMotorJointData().position[JointData::RHipRoll] *= parameters().general.hipRollSingleSupFactorRight;
  }


  updateMotionStatus(getMotionStatus());

  if(getMotionRequest().id != getId() && getEngine().zmpControl.is_stationary()) {
    setCurrentState(motion::stopped);
    std::cout << "walk stopped" << std::endl;
  } else {
    setCurrentState(motion::running);
  }
}//end execute


void Walk::manageSteps(const MotionRequest& motionRequest)
{
  // add the initial step
  // NOTE: zmp for this step it already completely planed,
  //       so we add the actual first step right after
  if(stepBuffer.empty())
  {
    std::cout << "walk start" << std::endl;

    // use the current com pose as a basis for start
    CoMFeetPose currentCOMFeetPose = getEngine().getCurrentCoMFeetPose();
    currentCOMFeetPose.localInLeftFoot();
    currentCOMFeetPose.com.translation.z = parameters().hip.comHeight;
    
    // new step (don't move the feet)
    Step& initialStep = stepBuffer.add();
    initialStep.footStep = FootStep(currentCOMFeetPose.feet, FootStep::NONE);

    // initialize the zmp buffer with the current com pose
    initialStep.numberOfCycles = getEngine().zmpControl.init(currentCOMFeetPose.com.translation, currentCOMFeetPose.com.translation);
    initialStep.planningCycle = initialStep.numberOfCycles;
  }

  // current step has been executed, remove
  if ( stepBuffer.first().isExecuted() ) {
    stepBuffer.pop();
  }

  // add a new step
  if(stepBuffer.last().isPlanned()) {
    const Step& lastStep = stepBuffer.last();
    Step& step = stepBuffer.add();
    calculateNewStep(lastStep, step, motionRequest.walkRequest);
  }

}//end manageSteps


void Walk::calculateNewStep(const Step& lastStep, Step& newStep, const WalkRequest& walkRequest) //const
{
  // update the parameters in case they have changed
  theFootStepPlanner.updateParameters(getEngine().getParameters());
  
  newStep.walkRequest = walkRequest;

  if ( getMotionRequest().id != getId() )
  {
    // try to make a last step to align the feet if it is required
    if ( getMotionRequest().standardStand ) {
      newStep.footStep = theFootStepPlanner.finalStep(lastStep.footStep, walkRequest);
    } else {
      newStep.footStep = theFootStepPlanner.zeroStep(lastStep.footStep);
    }

    newStep.numberOfCycles = (newStep.footStep.liftingFoot() == FootStep::NONE)?1:parameters().step.duration/getRobotInfo().basicTimeStep;
    newStep.type = STEP_WALK;

    std::cout << "walk stopping ..." << std::endl;
    return;
  }

  // indicates whether the requested foot is movable in this step
  // i.e., it was NOT moved in the last step
  bool stepControlPossible = 
        lastStep.footStep.liftingFoot() == FootStep::NONE
    || (lastStep.footStep.liftingFoot() == FootStep::RIGHT && walkRequest.stepControl.moveLeftFoot)
    || (lastStep.footStep.liftingFoot() == FootStep::LEFT && !walkRequest.stepControl.moveLeftFoot);


  if ( stepControlPossible && walkRequest.stepControl.stepID == stepBuffer.stepId() )
  {
    // step control
    newStep.footStep = theFootStepPlanner.controlStep(lastStep.footStep, walkRequest);
    newStep.numberOfCycles = walkRequest.stepControl.time / getRobotInfo().basicTimeStep;
    newStep.type = STEP_CONTROL;
  }
  else
  {
    newStep.footStep = theFootStepPlanner.nextStep(lastStep.footStep, walkRequest);
    newStep.numberOfCycles = parameters().step.duration / getRobotInfo().basicTimeStep;
    newStep.type = STEP_WALK;
  }
}


void Walk::planZMP()
{
  ASSERT(!stepBuffer.empty());

  Step& planningStep = stepBuffer.last();
  ASSERT(!planningStep.isPlanned());

  Vector3d zmp;
  if(planningStep.footStep.liftingFoot() == FootStep::NONE)
  {
    Pose3D finalBody = calculateStableCoMByFeet(planningStep.footStep.end(), getEngine().getParameters().bodyPitchOffset);
    zmp = finalBody.translation;
  } else {
    // TODO: should it be a part of the Step?
    double zmpOffsetY = parameters().hip.ZMPOffsetY + parameters().hip.ZMPOffsetYByCharacter * (1-planningStep.walkRequest.character);
    double zmpOffsetX = getEngine().getParameters().hipOffsetX;

    Vector2d zmp_simple = ZMPPlanner::simplest(planningStep.footStep, zmpOffsetX, zmpOffsetY);
    zmp = Vector3d(zmp_simple.x, zmp_simple.y, parameters().hip.comHeight);
  }

  //zmp.z = parameters().hip.comHeight;
  getEngine().zmpControl.push(zmp);

  FIELD_DRAWING_CONTEXT;
  getDebugDrawings().pen(Color::BLUE, 5.0);
  getDebugDrawings().drawCircle(zmp.x, zmp.y, 10);

  planningStep.planningCycle++;
}


void Walk::executeStep()
{
  ASSERT(!stepBuffer.empty());

  Step& executingStep = stepBuffer.first();
  ASSERT(!executingStep.isExecuted());

  Vector3d com;
  if( !getEngine().zmpControl.pop(com) ) {
    return;
  }

  // debug
  FIELD_DRAWING_CONTEXT;
  getDebugDrawings().pen(Color::BLUE, 1.0);
  getDebugDrawings().fillOval(com.x, com.y, 10, 10);

  switch(executingStep.footStep.liftingFoot()) 
  {
    case FootStep::LEFT:
    {
      theCoMFeetPose.feet.left = calculateLiftingFootPos(executingStep);
      theCoMFeetPose.feet.right = executingStep.footStep.supFoot();
      break;
    }
    case FootStep::RIGHT:
    {
      theCoMFeetPose.feet.left = executingStep.footStep.supFoot();
      theCoMFeetPose.feet.right = calculateLiftingFootPos(executingStep);
      break;
    }
    case FootStep::NONE:
    {
      theCoMFeetPose.feet = executingStep.footStep.begin();
      break;
    }
    default: ASSERT(false);
  }

  theCoMFeetPose.com.translation = com;
  // TODO: check this
  theCoMFeetPose.com.rotation = calculateBodyRotation(theCoMFeetPose.feet, getEngine().getParameters().bodyPitchOffset);
  executingStep.executingCycle++;
}

Pose3D Walk::calculateLiftingFootPos(const Step& step) const
{
  int samplesDoubleSupport = std::max(1, (int) (parameters().step.doubleSupportTime / getRobotInfo().basicTimeStep));
  int samplesSingleSupport = step.numberOfCycles - samplesDoubleSupport;
  ASSERT(samplesSingleSupport >= 0 && samplesDoubleSupport >= 0);

  if ( step.type == STEP_CONTROL )
  {
    return FootTrajectorGenerator::stepControl(  
      step.footStep.footBegin(),
      step.footStep.footEnd(),
      step.executingCycle,
      samplesDoubleSupport,
      samplesSingleSupport,
      parameters().step.stepHeight,
      0, //footPitchOffset
      0, //footRollOffset
      step.walkRequest.stepControl.speedDirection,
      step.walkRequest.stepControl.scale);
    }
    else
    {
      return FootTrajectorGenerator::genTrajectory(
        step.footStep.footBegin(),
        step.footStep.footEnd(),
        step.executingCycle,
        samplesDoubleSupport,
        samplesSingleSupport,
        parameters().step.stepHeight,
        0, // footPitchOffset
        0  // footRollOffset
      );
    }
}

RotationMatrix Walk::calculateBodyRotation(const FeetPose& feet, double pitch) const
{
  double bodyAngleZ = Math::meanAngle(feet.left.rotation.getZAngle(), feet.right.rotation.getZAngle());
  return RotationMatrix::getRotationZ(bodyAngleZ) * RotationMatrix::getRotationY(pitch);
}

Pose3D Walk::calculateStableCoMByFeet(FeetPose feet, double pitch) const
{
  feet.left.translate(getEngine().getParameters().hipOffsetX, 0, 0);
  feet.right.translate(getEngine().getParameters().hipOffsetX, 0, 0);
  
  Pose3D com;
  com.rotation = calculateBodyRotation(feet, pitch);
  com.translation = (feet.left.translation + feet.right.translation) * 0.5;
  com.translation.z = parameters().hip.comHeight;

  return com;
}


void Walk::updateMotionStatus(MotionStatus& motionStatus) const
{
  if ( stepBuffer.empty() )
  {
    motionStatus.plannedMotion.lFoot = Pose2D();
    motionStatus.plannedMotion.rFoot = Pose2D();
    motionStatus.plannedMotion.hip = Pose2D();
  }
  else
  {
    FeetPose lastFeet = stepBuffer.last().footStep.end();
    Pose3D lastCom = calculateStableCoMByFeet(lastFeet, getEngine().getParameters().bodyPitchOffset);

    Pose3D plannedHip = theCoMFeetPose.com.invert() * lastCom;
    Pose3D plannedlFoot = theCoMFeetPose.feet.left.invert() * lastFeet.left;
    Pose3D plannedrFoot = theCoMFeetPose.feet.right.invert() * lastFeet.right;

    motionStatus.plannedMotion.hip = reduceDimen(plannedHip);
    motionStatus.plannedMotion.lFoot =  reduceDimen(plannedlFoot);
    motionStatus.plannedMotion.rFoot = reduceDimen(plannedrFoot);
  }

  // step control
  if ( stepBuffer.empty() )
  {
    motionStatus.stepControl.stepID = 0;
    motionStatus.stepControl.moveableFoot = MotionStatus::StepControlStatus::BOTH;
  }
  else
  {
    motionStatus.stepControl.stepID = stepBuffer.last().id();
    FootStep::Foot lastMovingFoot = stepBuffer.last().footStep.liftingFoot();
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
}//end updateMotionStatus