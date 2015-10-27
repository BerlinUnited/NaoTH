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

  if ( getMotionRequest().id != getId() )
  {
    // try to make a last step to align the feet if it is required
    if ( getMotionRequest().standardStand ) {
      newStep.footStep = theFootStepPlanner.finalStep(lastStep.footStep, walkRequest);
    } else {
      newStep.footStep = theFootStepPlanner.zeroStep(lastStep.footStep);
    }

    newStep.numberOfCycles = (newStep.footStep.liftingFoot() == FootStep::NONE)?1:250/getRobotInfo().basicTimeStep;
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
  }
  else
  {
    newStep.footStep = theFootStepPlanner.nextStep(lastStep.footStep, walkRequest);
  }

  newStep.numberOfCycles = 250/getRobotInfo().basicTimeStep;
}


void Walk::planZMP()
{
  ASSERT(!stepBuffer.empty());

  Step& planningStep = stepBuffer.last();
  ASSERT(!planningStep.isPlanned());

  Vector3d zmp;
  if(planningStep.footStep.liftingFoot() == FootStep::NONE)
  {
    FeetPose feet(planningStep.footStep.end());
    zmp = (feet.left.translation + feet.right.translation)*0.5;
  } else {
    zmp = planningStep.footStep.supFoot().translation;
  }

  zmp.z = parameters().hip.comHeight;
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

  FIELD_DRAWING_CONTEXT;
  getDebugDrawings().pen(Color::BLUE, 1.0);
  getDebugDrawings().fillOval(com.x, com.y, 10, 10);

  //TODO: plan step trajectory

  theCoMFeetPose.com.translation = com;
  executingStep.executingCycle++;
}
