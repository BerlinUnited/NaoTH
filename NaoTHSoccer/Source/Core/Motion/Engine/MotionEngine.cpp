

#include "MotionEngine.h"

// motion factories
#include "InitialMotion/InitialMotionFactory.h"
#include "InverseKinematicsMotion/InverseKinematicsMotionFactory.h"
#include "ParallelKinematicMotionEngine/ParallelKinematicMotionFactory.h"
#include "KeyFrameMotion/KeyFrameMotionEngine.h"

MotionEngine::MotionEngine()
{
  theEmptyMotion = registerModule<EmptyMotion>("EmptyMotion");

  theHeadMotionEngine = registerModule<HeadMotionEngine>("HeadMotionEngine", true);
  theIKArmGrasping = registerModule<IKArmGrasping>("IKArmGrasping", true);


  theMotionFactories.push_back(registerModule<InitialMotionFactory>("InitialMotionFactory", true)->getModuleT());
  theMotionFactories.push_back(registerModule<InverseKinematicsMotionFactory>("InverseKinematicsMotionFactory", true)->getModuleT());
  theMotionFactories.push_back(registerModule<KeyFrameMotionFactory>("KeyFrameMotionFactory", true)->getModuleT());
  theMotionFactories.push_back(registerModule<ParallelKinematicMotionFactory>("ParallelKinematicMotionFactory", true)->getModuleT());


  //
  currentlyExecutedMotion = createEmptyMotion();

  // init internal state
  //selectMotion();// create init motion
  state = initial;
}

MotionEngine::~MotionEngine()
{

}


void MotionEngine::execute()
{
  // ensure initialization
  switch (state)
  {
  case initial: // wait for the init motion to start
  {
    getHeadMotionRequest().id = HeadMotionRequest::numOfHeadMotion;
    getMotionRequest().time = getMotionStatus().time;
    getMotionRequest().id = motion::init;

    if ( getMotionStatus().currentMotion == motion::init
      && getMotionLock().id == motion::init
      && !getMotionLock().isStopped())
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
    getHeadMotionRequest().id = HeadMotionRequest::numOfHeadMotion;
    getMotionRequest().time = getMotionStatus().time;
    getMotionRequest().id = motion::init;
    break;
  }
  }//end switch

  // IMPORTANT: execute head motion firstly
  // stabilization of the walk depends on the head position
  // cf. InverseKinematicsMotionEngine::controlCenterOfMass(...)
  theHeadMotionEngine->execute();

  // select a new motion if necessary
  selectMotion();
  ASSERT(NULL!=currentlyExecutedMotion);
  
  //
  currentlyExecutedMotion->execute();

  // TODO: a good place to execute arms?
  theIKArmGrasping->execute();

  getMotionStatus().currentMotionState = getMotionLock().state;
}//end execute


void MotionEngine::selectMotion()
{
  ASSERT(currentlyExecutedMotion != NULL);

  // test if the current MotionStatus allready arrived in cognition
  if ( getMotionStatus().time != getMotionRequest().time )
    return;

  if (getMotionStatus().currentMotion == getMotionRequest().id
    && getMotionLock().isStopped())
  {
    changeMotion(createEmptyMotion());
  }

  if (getMotionStatus().currentMotion != getMotionRequest().id
    &&
    (getMotionLock().isStopped() || getMotionRequest().forced))
  {
    // unlock if forced
    if(getMotionRequest().forced) {
      getMotionLock().forceUnlock();
    }

    Module* newMotion = NULL;
    for ( MotionFactorieRegistry::iterator iter=theMotionFactories.begin();
          NULL==newMotion && iter!=theMotionFactories.end(); ++iter)
    {
      newMotion = (*iter)->createMotion(getMotionRequest());
    }

    if (NULL != newMotion)
    {
      // assure the right motion acquired the lock
      ASSERT(getMotionLock().id == getMotionRequest().id);
      changeMotion(newMotion);
    } else
    {
      changeMotion(createEmptyMotion());
      std::cerr << "Warning: Request " << motion::getName(getMotionRequest().id)
        << " cannot be executed!" << std::endl;
    }
  }
}//end selectMotion

void MotionEngine::changeMotion(Module* m)
{
  currentlyExecutedMotion = m;
  getMotionStatus().lastMotion = getMotionStatus().currentMotion;
  getMotionStatus().currentMotion = getMotionLock().id;
  getMotionStatus().time = getFrameInfo().getTime();
}//end changeMotion
