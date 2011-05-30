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

InitialMotionFactory::InitialMotionFactory()
  :
  currentMotion(NULL)
{
}

InitialMotionFactory::~InitialMotionFactory()
{
  if (currentMotion != NULL)
  {
    delete currentMotion;
    currentMotion = NULL;
  }
}

AbstractMotion* InitialMotionFactory::createMotion(const MotionRequest& motionRequest)
{
  if(currentMotion != NULL)
  {
    delete currentMotion;
    currentMotion = NULL;
  }//end if

  switch(motionRequest.id)
  {
    case motion::DEAD: currentMotion = new DeadMotion(); break;
    case motion::INIT: currentMotion = new InitialMotion(); break;
    default: currentMotion = NULL;
  }//end switch

  return currentMotion;
}//end createMotion
