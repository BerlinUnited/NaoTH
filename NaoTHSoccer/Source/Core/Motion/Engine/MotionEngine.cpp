

#include "MotionEngine.h"

// motion factories
#include "InitialMotion/InitialMotionFactory.h"
#include "InverseKinematicsMotion/InverseKinematicsMotionFactory.h"
#include "ParallelKinematicMotionEngine/ParallelKinematicMotionFactory.h"
#include "KeyFrameMotion/KeyFrameMotionEngine.h"

MotionEngine::MotionEngine()
  :
  theBlackBoard(MotionBlackBoard::getInstance())
{
  theMotionFactories.push_back(new InitialMotionFactory());
  theMotionFactories.push_back(new InverseKinematicsMotionFactory());
  theMotionFactories.push_back(new KeyFrameMotionEngine());
  theMotionFactories.push_back(new ParallelKinematicMotionFactory());

  // init internal state
  selectMotion();// create init motion
  state = initial;

  //
  currentlyExecutedMotion = &theEmptyMotion;
}

MotionEngine::~MotionEngine()
{
  for (std::list<MotionFactory*>::iterator iter = theMotionFactories.begin();
    iter != theMotionFactories.end(); ++iter)
  {
    delete *iter;
  }
}


void MotionEngine::execute()
{
  // ensure initialization
  switch (state)
  {
  case initial:
  {
    theBlackBoard.theHeadMotionRequest.id = HeadMotionRequest::numOfHeadMotion;
    theBlackBoard.theMotionRequest.time = theBlackBoard.theMotionStatus.time;
    theBlackBoard.theMotionRequest.id = motion::init;

    if ( theBlackBoard.theMotionStatus.currentMotion == motion::init
        && !currentlyExecutedMotion->isStopped()
        &&  currentlyExecutedMotion->isFinish() )
    {
      state = running;
    }

    break;
  }
  case running:
  {
    break;
  }
  case exiting:
  {
    theBlackBoard.theHeadMotionRequest.id = HeadMotionRequest::numOfHeadMotion;
    theBlackBoard.theMotionRequest.time = theBlackBoard.theMotionStatus.time;
    theBlackBoard.theMotionRequest.id = motion::init;
    break;
  }
  }//end switch

  // IMPORTANT: execute head motion firstly
  // stabilization of the walk depends on the head position
  // cf. InverseKinematicsMotionEngine::controlCenterOfMass(...)
  theHeadMotionEngine.execute();

  // motion engine execute
  selectMotion();
  ASSERT(NULL!=currentlyExecutedMotion);
  currentlyExecutedMotion->execute(theBlackBoard.theMotionRequest, theBlackBoard.theMotionStatus);
  theBlackBoard.theMotionStatus.currentMotionState = currentlyExecutedMotion->state();
}//end execute


void MotionEngine::selectMotion()
{
  ASSERT(currentlyExecutedMotion != NULL);

  // test if the current MotionStatus allready arrived in cognition
  if ( theBlackBoard.theMotionStatus.time != theBlackBoard.theMotionRequest.time )
    return;

  if (theBlackBoard.theMotionStatus.currentMotion == theBlackBoard.theMotionRequest.id
    && currentlyExecutedMotion->isStopped())
  {
    changeMotion(&theEmptyMotion);
  }

  if (theBlackBoard.theMotionStatus.currentMotion != theBlackBoard.theMotionRequest.id
    &&
    (currentlyExecutedMotion->isStopped() || theBlackBoard.theMotionRequest.forced))
  {
    AbstractMotion* newMotion = NULL;
    for ( std::list<MotionFactory*>::iterator iter=theMotionFactories.begin();
          NULL==newMotion && iter!=theMotionFactories.end(); ++iter)
    {
      newMotion = (*iter)->createMotion(theBlackBoard.theMotionRequest);
    }

    if (NULL != newMotion)
    {
      ASSERT(newMotion->getId() == theBlackBoard.theMotionRequest.id);
      changeMotion(newMotion);
    } else
    {
      changeMotion(&theEmptyMotion);
      cerr << "Warning: Request " << motion::getName(theBlackBoard.theMotionRequest.id)
        << " cannot be executed!" << endl;
    }
  }
}//end selectMotion

void MotionEngine::changeMotion(AbstractMotion* m)
{
  currentlyExecutedMotion = m;
  theBlackBoard.theMotionStatus.lastMotion = theBlackBoard.theMotionStatus.currentMotion;
  theBlackBoard.theMotionStatus.currentMotion = currentlyExecutedMotion->getId();
  theBlackBoard.theMotionStatus.time = theBlackBoard.theFrameInfo.getTime();
}//end changeMotion