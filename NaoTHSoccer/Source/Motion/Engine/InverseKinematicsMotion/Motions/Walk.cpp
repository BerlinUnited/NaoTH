/**
* @file Walk.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen, Kaden</a>
*
*/

#include "Walk.h"

using namespace naoth;
using namespace InverseKinematic;

Walk::Walk() : IKMotion(getInverseKinematicsMotionEngineService(), motion::walk, getMotionLock())
{
}
  
void Walk::execute()
{
  // check the integrity 
  getMotionRequest().walkRequest.assertValid();
  
  // update the parameters in case they have changed
  if ( stepBuffer.empty() || stepBuffer.last().isPlanned()) {
    theFootStepPlanner.updateParameters(getEngine().getParameters());
  }

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

  if(getMotionRequest().id != getId()) {
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

    // TODO: for now it returns getCurrentCoMFeetPose()
    ZMPFeetPose currentZMPFeetPose = getEngine().getPlannedZMPFeetPose();
    currentZMPFeetPose.localInLeftFoot();
    //currentZMPFeetPose.zmp.translation.z = parameters.hip.comHeight;
    
    // new step (don't move the feet)
    Step& initialStep = stepBuffer.add();
    initialStep.footStep = FootStep(currentZMPFeetPose.feet, FootStep::NONE);

    // plan the whole initial step
    initialStep.numberOfCycles = getEngine().controlZMPstart(currentZMPFeetPose);
    initialStep.planningCycle = initialStep.numberOfCycles;
  }

  // current step is executed, remove
  if ( stepBuffer.first().isExecuted() ) {
    stepBuffer.pop();
  }

  // add a new step
  if(stepBuffer.last().isPlanned())
  {
    // stop walking
    if ( motionRequest.id != getId())
    {
      // try to make a last step to align the feet if it is required
      if ( motionRequest.standardStand ) {
        newLastStep();
      } else {
        newZeroStep();
      }

      std::cout << "walk stopping ..." << std::endl;
      return;
    }
    else {
      // walk, add new step
      newStep(motionRequest.walkRequest);
    }
  }

}//end manageSteps

void Walk::newZeroStep()
{
  const Step& lastStep = stepBuffer.last();
  
  // create a new step
  Step& step = stepBuffer.add();
  step.footStep = FootStep(lastStep.footStep.end(), FootStep::NONE);
  step.numberOfCycles = 1;
}

void Walk::newLastStep()
{
  // TODO: check if an actual step is necessary based on the last step
  //       => calculate an actual step only if necessary

  const Step& lastStep = stepBuffer.last();

  // try to plan a real last step with an empty walk request
  FootStep footStep = theFootStepPlanner.nextStep(lastStep.footStep, WalkRequest());
  // how much did the foot move in this step
  Pose3D diff = footStep.footBegin().invert() * footStep.footEnd();

  // planed step almost didn't move the foot, i.e., is was almost a zero step
  if(diff.translation.abs2() < 1 && diff.rotation.getZAngle() < Math::fromDegrees(1)) {
    newZeroStep();
  } else {
    Step& step = stepBuffer.add();
    step.footStep = footStep;
    step.numberOfCycles = 250/getRobotInfo().basicTimeStep;
  }
}

void Walk::newStep(const WalkRequest& walkRequest)
{
  const Step& lastStep = stepBuffer.last();

  // create a new step
  Step& step = stepBuffer.add();

  // indicates whether the requested foot is movable in this step
  // i.e., it was NOT moved in the last step
  bool stepControlPossible = 
        lastStep.footStep.liftingFoot() == FootStep::NONE
    || (lastStep.footStep.liftingFoot() == FootStep::RIGHT && walkRequest.stepControl.moveLeftFoot)
    || (lastStep.footStep.liftingFoot() == FootStep::LEFT && !walkRequest.stepControl.moveLeftFoot);


  if ( stepControlPossible && walkRequest.stepControl.stepID == stepBuffer.stepId() )
  {
    // step control
    step.footStep = theFootStepPlanner.controlStep(lastStep.footStep, walkRequest);
  }
  else
  {
    step.footStep = theFootStepPlanner.nextStep(lastStep.footStep, walkRequest);
  }

  step.numberOfCycles = 250/getRobotInfo().basicTimeStep;
}


void Walk::planZMP()
{
  Step& planningStep = stepBuffer.last();
  ASSERT(!planningStep.isPlanned());

  //TODO: plan ZMP

  planningStep.planningCycle++;
}


void Walk::executeStep()
{
  Step& executingStep = stepBuffer.first();
  ASSERT(!executingStep.isExecuted());

  //TODO: plan step trajectory

  executingStep.executingCycle++;
}
