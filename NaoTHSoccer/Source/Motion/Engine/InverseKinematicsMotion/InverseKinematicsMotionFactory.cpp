/**
* @file InverseKinematicsMotionFactory.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Implementation of class Inverse Kinematics Motion Factory
*/

#include "InverseKinematicsMotionFactory.h"

#include "Motions/StandMotion.h"
#include "Motions/Dance.h"
#include "Motions/Walk.h"
#include "Motions/Walk2018/Walk2018.h"
//#include "Motions/ProtectFalling.h"
//#include "Motions/IKDynamicKickMotion.h"

InverseKinematicsMotionFactory::InverseKinematicsMotionFactory()
  : currentMotionCreator(NULL)
{
  getInverseKinematicsMotionEngineService().setEngine(registerModule<InverseKinematicsMotionEngine>("InverseKinematicsMotionEngine", true)->getModuleT());
}

InverseKinematicsMotionFactory::~InverseKinematicsMotionFactory()
{
}

#define REGISTER_MOTION(typeID, name) \
  static ModuleCreator<name>* name##Creator = registerModule<name>(#name); \
  if(motionRequest.id == typeID) \
  { \
    assert(currentMotionCreator==NULL); currentMotionCreator = name##Creator; \
  }


Module* InverseKinematicsMotionFactory::createMotion(const MotionRequest& motionRequest)
{
  if(currentMotionCreator != NULL)
    currentMotionCreator->setEnabled(false);
  currentMotionCreator = NULL;

  REGISTER_MOTION(motion::stand, StandMotion);
  if(motionRequest.id == motion::unrelaxed_stand)
  {
    assert(currentMotionCreator==NULL); currentMotionCreator = StandMotionCreator;
  }
  REGISTER_MOTION(motion::dance, DanceMotion);
  if(getInverseKinematicsMotionEngineService().getEngine().getParameters().useWalk2018){
      REGISTER_MOTION(motion::walk, Walk2018);
  } else {
      REGISTER_MOTION(motion::walk, Walk);
  }
  //REGISTER_MOTION(motion::walk2018, Walk2018);
  //REGISTER_MOTION(motion::protect_falling, ProtectFalling);
  //REGISTER_MOTION(motion::kick, IKDynamicKickMotion);

  if(currentMotionCreator != NULL)
  {
    currentMotionCreator->setEnabled(true);
    return currentMotionCreator->getModule();
  }

  return NULL;
}//end createMotion
