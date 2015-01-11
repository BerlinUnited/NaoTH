/** 
 * @file   InitialMotionFactory.cpp
 * 
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */
 
#include "InitialMotionFactory.h"

// motions
#include "DeadMotion.h"
#include "InitialMotion.h"
#include "Sit.h"

InitialMotionFactory::InitialMotionFactory()
  :
  currentCreator(NULL)
{
}

InitialMotionFactory::~InitialMotionFactory()
{
  currentCreator = NULL;
}


Module* InitialMotionFactory::createMotion(const MotionRequest& motionRequest)
{
  if(currentCreator != NULL)
    currentCreator->setEnabled(false);
  currentCreator = NULL;

  static ModuleCreator<DeadMotion>* creatorDeadMotion = registerModule<DeadMotion>("DeadMotion");
  static ModuleCreator<InitialMotion>* creatorInitialMotion = registerModule<InitialMotion>("InitialMotion");
  static ModuleCreator<Sit>* creatorSit = registerModule<Sit>("Sit");
  
  switch(motionRequest.id)
  {
  case motion::dead: currentCreator = creatorDeadMotion; break;
  case motion::init: currentCreator = creatorInitialMotion; break;
  case motion::sit: currentCreator = creatorSit; break;
  default: currentCreator = NULL;
  }//end switch

  if(currentCreator != NULL)
  {
    currentCreator->setEnabled(true);
    return currentCreator->getModule();
  }

  return NULL;
}//end createMotion
