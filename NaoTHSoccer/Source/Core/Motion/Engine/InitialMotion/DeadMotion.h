/**
* @file DeadMotion.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* dead motion disable all the motor joints, i.e set the hardness to 0
*/

#ifndef _DEADMOTION_H
#define _DEADMOTION_H

#include "Motion/AbstractMotion.h"

#include <ModuleFramework/Module.h>

BEGIN_DECLARE_MODULE(DeadMotion)
  REQUIRE(RobotInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(DeadMotion)

class DeadMotion : private DeadMotionBase, public AbstractMotion
{
public:
  DeadMotion();

  virtual ~DeadMotion(){}

  virtual void execute(const MotionRequest& motionRequest, MotionStatus& /*motionStatus*/){};
  void execute();

private:

  double stiffness_increase;
  double oldStiffness[naoth::JointData::numOfJoint];
  double freeStiffness[naoth::JointData::numOfJoint];
};

#endif  /* _DEADMOTION_H */

