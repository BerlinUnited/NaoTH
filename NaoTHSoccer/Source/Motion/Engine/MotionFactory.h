/**
* @file MotionEngine.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class MotionFactory
*/

#ifndef _MotionFactory_h_
#define _MotionFactory_h_

#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/Module.h>

#include "Representations/Motion/Request/MotionRequest.h"

class MotionFactory: protected ModuleManager
{
public:
  virtual ~MotionFactory(){}
  virtual Module* createMotion(const MotionRequest& motionRequest) = 0;
};

#endif //_MotionFactory_h_
