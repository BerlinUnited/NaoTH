/**
* @file ParallelStepper.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#ifndef _ParallelStepper_H
#define	_ParallelStepper_H

#include "Motion/AbstractMotion.h"
#include "Motion/MorphologyProcessor/ParallelKinematic.h"

#include <ModuleFramework/Module.h>

BEGIN_DECLARE_MODULE(ParallelStepper)
  REQUIRE(RobotInfo)
  //REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(ParallelStepper)

class ParallelStepper : private ParallelStepperBase, public AbstractMotion
{
public:
  ParallelStepper();
  
  virtual ~ParallelStepper(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/){};
  void execute();
  
private:
  double shift; // current shift in mm
  double speed; // current speed of the motion in degreees
  double t; // current time

  Kinematics::ParallelKinematic theParallelKinematic;
};

#endif	/* _ParallelStepper_H */

