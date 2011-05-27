/**
* @file InverseKinematicsMotionFactory.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Declaration of class Inverse Kinematics Motion Factory
*/

#ifndef _InverseKinematicsMotionFactory_h_
#define _InverseKinematicsMotionFactory_h_


#include "Core/Motion/MotionEngine/MotionFactory.h"
#include "Core/Representations/Motion/Request/MotionRequest.h"

class InverseKinematicsMotionFactory : public MotionFactory
{

public:
  InverseKinematicsMotionFactory();
  virtual ~InverseKinematicsMotionFactory();

  AbstractMotion* createMotion(const MotionRequest& motionRequest);

private:

  //InverseKinematicsMotionEngine engine;

  AbstractMotion* currentMotion;
};

#endif // _InverseKinematicsMotionFactory_h_
