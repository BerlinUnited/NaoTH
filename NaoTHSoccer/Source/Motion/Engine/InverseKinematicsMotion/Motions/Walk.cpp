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
  // check if the request is valid (assert if not)
  getMotionRequest().walkRequest.assertValid();
  
  manageSteps(getMotionRequest());

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
    std::cout << "neowalk start" << std::endl;
    // TODO: is this initialization necessary here?
    //theCoMFeetPose = getEngine().getCurrentCoMFeetPose();
    
    // TODO: for now it returns getCurrentCoMFeetPose()
    ZMPFeetPose currentZMPFeetPose = getEngine().getPlannedZMPFeetPose();
    currentZMPFeetPose.localInLeftFoot();
    //currentZMPFeetPose.zmp.translation.z = parameters.hip.comHeight;
    
    // new step (don't move the feet)
    Step& initialStep = stepBuffer.add();
    //setParameters(initialStep);
    initialStep.footStep = FootStep(currentZMPFeetPose.feet, FootStep::NONE);
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

      std::cout << "neowalk stopping ..." << std::endl;
      return;
    }
    else {
      // walk, add new step
      newStep(motionRequest.walkRequest);
    }
  }

}//end manageSteps

void Walk::newLastStep()
{
  // TODO: check if an actual step is necessary based on the last step
  //       => calculate an actual step only if necessary

  const Step& lastStep = stepBuffer.last();

  // try to plan a real last step with an empty walk request
  FootStep footStep = theFootStepPlanner.nextStep(lastStep.footStep, WalkRequest());
  
  // last step almost didn't move the foot, i.e., is was almost a zero step
  Pose3D diff = footStep.footBegin().invert() * footStep.footEnd();
  bool lastStepWasAlmostZero = diff.translation.abs2() < 1 && diff.rotation.getZAngle() < Math::fromDegrees(1);

  if(lastStepWasAlmostZero) {
    newZeroStep();
  } else {
    Step& step = stepBuffer.add();
    step.footStep = footStep;
    //setParameters(step);
  }
}

void Walk::newZeroStep()
{
  const Step& lastStep = stepBuffer.last();
  
  // create a new step
  Step& step = stepBuffer.add();
  step.footStep = FootStep(lastStep.footStep.end(), FootStep::NONE);
  step.numberOfCycles = 1;
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
    
    /*
    setParameters(step);

    // HACK: override some parameters for the step control
    step.samplesSingleSupport = std::max(1, (int) (walkRequest.stepControl.time / getRobotInfo().basicTimeStep));
    step.numberOfCyclePerFootStep = step.samplesDoubleSupport + step.samplesSingleSupport;
    
    step.stepControlling = true;
    step.speedDirection = walkRequest.stepControl.speedDirection;
    step.scale = walkRequest.stepControl.scale;
    */
  }
  else
  {
    step.footStep = theFootStepPlanner.nextStep(lastStep.footStep, walkRequest);
    //setParameters(step);
  }
}
