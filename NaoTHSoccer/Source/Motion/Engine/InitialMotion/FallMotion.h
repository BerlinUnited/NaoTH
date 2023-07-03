/**
* @file DeadMotion.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
* fall motion disable all the motor joints, i.e set the hardness to 0, except the head joints
*/

#ifndef _FALLMOTION_H
#define _FALLMOTION_H

#include "Motion/Engine/AbstractMotion.h"

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>


BEGIN_DECLARE_MODULE(FallMotion)
  REQUIRE(RobotInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  
  PROVIDE(MotionLock)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(FallMotion)

class FallMotion : private FallMotionBase, public AbstractMotion
{
public:
  FallMotion();

  virtual ~FallMotion(){}

  void execute();

private:

  double stiffness_increase;
  double oldStiffness[naoth::JointData::numOfJoint];
  double freeStiffness[naoth::JointData::numOfJoint];
};

#endif  /* _DEADMOTION_H */

