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
  int t_since_fall_start;

  // example
  struct KeyFrame {
    // can je angles or stiffness
    std::vector<double> jointValues;

    // until which 
    double angleLimit;
  };

  std::vector<std::vector<double>> fall_front_map;
  std::vector<std::vector<double>> fall_stiffness_front_map;
  std::vector<double> fall_times_front;
  std::vector<double> fall_t_stiffness_front;

  std::vector<std::vector<double>> fall_back_map;
  std::vector<std::vector<double>> fall_stiffness_back_map;
  std::vector<double> fall_times_back;
  std::vector<double> fall_t_stiffness_back;

  double stiffness_increase;
  double oldStiffness[naoth::JointData::numOfJoint]{};
  double freeStiffness[naoth::JointData::numOfJoint]{};
};

#endif  /* _DEADMOTION_H */
