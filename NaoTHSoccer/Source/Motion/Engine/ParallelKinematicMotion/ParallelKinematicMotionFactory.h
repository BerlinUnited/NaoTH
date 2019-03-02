/**
* @file InverseKinematicsMotionEngine.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* 
*/

#ifndef _ParallelKinematicMotionFactory_H
#define	_ParallelKinematicMotionFactory_H

#include "Motion/Engine/MotionFactory.h"
#include <ModuleFramework/Module.h>

DECLARE_MODULE_WITHOUT_REPRESENTATIONS(ParallelKinematicMotionFactory)

class ParallelKinematicMotionFactory: public ParallelKinematicMotionFactoryBase, public MotionFactory
{
public:
  ParallelKinematicMotionFactory();
  virtual ~ParallelKinematicMotionFactory();

  virtual Module* createMotion(const MotionRequest& motionRequest);
  void execute(){} // dummy

private:
  AbstractModuleCreator* currentMotionCreator;
};

#endif	/* _ParallelKinematicMotionFactory_H */

