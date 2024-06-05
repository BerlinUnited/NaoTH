/**
* @file DeadMotion.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
* fall motion disable all the motor joints, i.e set the hardness to 0, except the head joints
*/

#ifndef FALLMOTION_H
#define FALLMOTION_H

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
  virtual ~FallMotion() {}

  void execute();


private:
  struct keyFrame {
    std::array<double, 22> jointValues;
    std::array<double, 22> stiffnessValues;
    double triggerMs;
    double altTriggerGrad;
  };


  /*   
  HeadPitch,
  HeadYaw,

  RShoulderRoll,
  LShoulderRoll,
  RShoulderPitch,
  LShoulderPitch,

  RElbowRoll,
  LElbowRoll,
  RElbowYaw,
  LElbowYaw,

  RHipYawPitch, // doesn't exist on Nao
  LHipYawPitch,
  RHipPitch,
  LHipPitch,
  RHipRoll,
  LHipRoll,
  RKneePitch,
  LKneePitch,
  RAnklePitch,
  LAnklePitch,
  RAnkleRoll,
  LAnkleRoll,

  LWristYaw,
  RWristYaw,
  LHand,
  RHand
  */


//0.0;-38.0;5.0;10.0;100.0;-60.0;-90.0;0.0;5.0;-10.0;-100.0;60.0;90.0;0.0;0.0;0.0;-24.0;105.0;-75.0;0.0;0.0;0.0;-24.0;105.0;-75.0;0.0
//0.0;-38.0;5.0;10.0;100.0;-60.0;-90.0;0.0;5.0;-10.0;-100.0;60.0;90.0;0.0;0.0;0.0;-24.0;105.0;-75.0;0.0;0.0;0.0;-24.0;105.0;-75.0;0.0


private: // hardcoded key frames
  std::vector<keyFrame> forwards = {
    {
      { -38,   0,  -10,  10,  90,  90,   0,   0, -100, 100,  0,  0, -24, -24,  0,  0, 105, 105, -75, -75,  0,  0 }, // angles
      { 100, 100,  100, 100, 100, 100, 100, 100,  100, 100, 20, 20,  20,  20, 20, 20,  20,  20,  20,  20, 20, 20 }, // stiffness
      200, // 0 -> 1
      300
    },{
      { -38,   0,  -10,  10,   5,   5,  60, -60, -100, 100,  0,  0, -24, -24,  0,  0, 105, 105, -75, -75,  0,  0 }, // angles
      { 100, 100,  100, 100, 100, 100, 100, 100,  100, 100, 20, 20,  20,  20, 20, 20,  20,  20,  20,  20, 20, 20 }, // stiffness
      600, // 1 -> 2
      550
    },{
      { -38,   0,  -10,  10,   5,   5,  60, -60, -100, 100,  0,  0, -24, -24,  0,  0, 105, 105, -75, -75,  0,  0 }, // angles
      {  30,  30,   15,  15,  15,  15,  15,  15,   15,  15, 20, 20,  20,  20, 20, 20,  20,  20,  20,  20, 20, 20 }, // stiffness
      600, // 2 -> 3?
      700
    },{
      { -38,   0,  -10,  10,   5,   5,  60, -60, -100, 100,  0,  0, -24, -24,  0,  0, 105, 105, -75, -75,  0,  0 }, // angles
      {   0,   0,    0,   0,   0,   0,   0,   0,    0,   0,  0,  0,   0,   0,  0,  0,   0,   0,   0,   0,  0,  0 }, // stiffness
      1500,
      999
    }
  };

  std::vector<keyFrame> backwards = {
    {
      {  29,   0, -12,  12, 123, 123,   0,  0,  -17,  17,  0,  0, -90, -90,  0,  0, 105, 105, -45, -45, 0, 0 },
      { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 30, 30,  30,  30, 30, 30,  30,  30,  30,  30, 0, 0 },
      150,
      28
    },{
      {  29,   0, -12,  12, 123, 123,  78, -78, -17,  17,  0,  0, -90, -90,  0,  0, 105, 105, -45, -45, 0, 0 },
      { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 30, 30,  30,  30, 30, 30,  30,  30,  30,  30, 0, 0 },
      300,
      55
    },{
      { 29,  0, -12, 12, 123, 123, 78, -78, -17, 17, 0, 0, -90, -90, 0, 0, 105, 105,-45, -45, 0, 0 },
      { 15, 15,  15, 15,  15,  15, 15,  15,  15, 15, 5, 5,   5,   5, 5, 5,   5,   5,  5,   5, 0, 0 },
      600,
      70
    },{
      { 29, 0, -12, 12, 123, 123, 78, -78, -17, 17, 0, 0, -90, -90, 0, 0, 105, 105, -45, -45, 0, 0},
      {  0, 0,   0,  0,   0,   0,  0,   0,   0,  0, 0, 0,   0,   0, 0, 0,   0,   0,   0,   0, 0, 0},
      1500,
      999
    }
  };

private:
  bool fallingForward; // falling direction
  FrameInfo startTime;

  double stiffness_increase;
  double oldStiffness[naoth::JointData::numOfJoint]{};
//  double oldJoints[naoth::JointData::numOfJoint]{};
};

#endif  /* FALLMOTION_H */
