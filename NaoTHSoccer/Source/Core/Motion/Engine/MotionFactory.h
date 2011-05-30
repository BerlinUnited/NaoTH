/**
* @file MotionEngine.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class MotionFactory
*/

#ifndef __MotionFactory_h_
#define __MotionFactory_h_

#include "Motion/AbstractMotion.h"

class MotionFactory
{
public:
  virtual ~MotionFactory(){}

  virtual AbstractMotion* createMotion(const MotionRequest& motionRequest) = 0;
};

#endif //__MotionFactory_h_
