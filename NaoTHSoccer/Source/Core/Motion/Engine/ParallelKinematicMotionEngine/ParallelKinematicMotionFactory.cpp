/**
* @file InverseKinematicsMotionEngine.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* 
*/

#include "ParallelKinematicMotionFactory.h"

// motions
#include "ParallelDance.h"
#include "ParallelStepper.h"

ParallelKinematicMotionFactory::ParallelKinematicMotionFactory()
  :
  currentMotionCreator(NULL)
{
}

ParallelKinematicMotionFactory::~ParallelKinematicMotionFactory()
{
}

Module* ParallelKinematicMotionFactory::createMotion(const MotionRequest& motionRequest)
{
  if(currentMotionCreator != NULL)
    currentMotionCreator->setEnabled(false);
  currentMotionCreator = NULL;

  static ModuleCreator<ParallelDance>* parallelDanceCreator = registerModule<ParallelDance>("ParallelDance");
  static ModuleCreator<ParallelStepper>* parallelStepperCreator = registerModule<ParallelStepper>("ParallelStepper");

  switch(motionRequest.id)
  {
    case motion::parallel_dance: currentMotionCreator = parallelDanceCreator; break;
    case motion::parallel_stepper: currentMotionCreator = parallelStepperCreator; break;
    default: currentMotionCreator = NULL;
  }//end switch

  if(currentMotionCreator != NULL)
  {
    currentMotionCreator->setEnabled(true);
    return currentMotionCreator->getModule();
  }

  return NULL;
}//end createMotion
