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
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Modeling/IMUData.h>


BEGIN_DECLARE_MODULE(FallMotion)
  REQUIRE(FrameInfo)
  REQUIRE(RobotInfo)
  REQUIRE(SensorJointData)
  REQUIRE(MotionRequest)
  REQUIRE(IMUData)
  REQUIRE(InertialSensorData)

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
  FrameInfo startTime;

  struct keyFrame {
      std::array<double, 22> jointValues;
      std::array<double, 22> stiffnessValues;
      double triggerMs;
      double altTriggerGrad;
  };

  double stiffness_increase;
  double oldStiffness[naoth::JointData::numOfJoint]{};
//  double oldJoints[naoth::JointData::numOfJoint]{};
};

#endif  /* _FALLMOTION_H */
