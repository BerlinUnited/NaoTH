/**
* @file InverseKinematicsMotionFactory.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Declaration of class Inverse Kinematics Motion Factory
*/

#ifndef _InverseKinematicsMotionFactory_h_
#define _InverseKinematicsMotionFactory_h_


#include "Motion/Engine/MotionFactory.h"
#include "InverseKinematicsMotionEngine.h"

#include <ModuleFramework/Module.h>

BEGIN_DECLARE_MODULE(InverseKinematicsMotionFactory)
  PROVIDE(InverseKinematicsMotionEngineService)
END_DECLARE_MODULE(InverseKinematicsMotionFactory)

class InverseKinematicsMotionFactory : private InverseKinematicsMotionFactoryBase, public MotionFactory
{

public:
  InverseKinematicsMotionFactory();
  virtual ~InverseKinematicsMotionFactory();

  Module* createMotion(const MotionRequest& motionRequest);
  void execute(){} // dummy

private:
  AbstractModuleCreator* currentMotionCreator;

};

#endif // _InverseKinematicsMotionFactory_h_
