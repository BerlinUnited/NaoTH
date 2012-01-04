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
  currentMotion(NULL)
{
}

ParallelKinematicMotionFactory::~ParallelKinematicMotionFactory()
{
  if (currentMotion != NULL)
  {
    delete currentMotion;
    currentMotion = NULL;
  }
}

AbstractMotion* ParallelKinematicMotionFactory::createMotion(const MotionRequest& motionRequest)
{
  if(currentMotion != NULL)
  {
    delete currentMotion;
    currentMotion = NULL;
  }//end if

  switch(motionRequest.id)
  {
    case motion::parallel_dance: currentMotion = new ParallelDance(); break;
    case motion::parallel_stepper: currentMotion = new ParallelStepper(); break;
    default: currentMotion = NULL;
  }//end switch

  return currentMotion;
}//end createMotion
