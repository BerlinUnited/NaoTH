/** 
 * @file   InitialMotionFactory.h
 * 
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _INITIALMOTIONFACTORY_H
#define _INITIALMOTIONFACTORY_H

#include "Motion/Engine/MotionFactory.h"

class InitialMotionFactory: public MotionFactory
{
public:
  InitialMotionFactory();
  virtual ~InitialMotionFactory();

  virtual Module* createMotion(const MotionRequest& motionRequest);
  void execute(){} // dummy

private:
  AbstractModuleCreator* currentCreator;
};

#endif  /* _INITIALMOTIONFACTORY_H */

