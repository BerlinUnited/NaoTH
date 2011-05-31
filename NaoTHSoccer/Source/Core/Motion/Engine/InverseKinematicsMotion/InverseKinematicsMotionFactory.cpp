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

/*
#include "Motions/IKKickMotion.h"
#include "Motions/IKDynamicKickMotion.h"
#include "Motions/IKDynamicKickMotion_new.h"

#include "Motions/GotoPoseDirectlyMotion.h"
#include "Motions/ExploreReachabilitySpace.h"
#include "Motions/Check_Reactivity.h"

#include "Motions/IKGrasping.h"
#include "Motions/IKDynamicKickMotion_new.h"
*/

InverseKinematicsMotionFactory::InverseKinematicsMotionFactory()
  :currentMotion(NULL)
{
}

InverseKinematicsMotionFactory::~InverseKinematicsMotionFactory()
{
  if ( currentMotion != NULL )
    delete currentMotion;
}

AbstractMotion* InverseKinematicsMotionFactory::createMotion(const MotionRequest& motionRequest)
{
  // FIXME: the old motion shouldn't be deleted!!! 
  if(currentMotion != NULL)
  {
    delete currentMotion;
    currentMotion = NULL;
  }//end if

  switch(motionRequest.id)
  {
    case motion::STAND: currentMotion = new StandMotion(); break;
    case motion::DANCE: currentMotion = new DanceMotion(); break;
    case motion::WALK: currentMotion = new Walk(); break;
    /*
    case MotionRequestID::kick: currentMotion = new IKDynamicKickMotion_new(engine); break;
    case MotionRequestID::grasp: currentMotion = new IKGrasping(engine); break;
        case MotionRequestID::check_reactivity: currentMotion = new CheckMotion(engine); break;*/
    default: currentMotion = NULL;
  }//end switch

  return currentMotion;
}//end createMotion
