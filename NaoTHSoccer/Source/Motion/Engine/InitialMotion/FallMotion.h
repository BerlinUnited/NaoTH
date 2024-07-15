/**
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
*/

#ifndef FALLMOTION_H // todo: is it ok to remove the underscore?
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

class FallMotion final: private FallMotionBase, public AbstractMotion
{
public:
  FallMotion();

  ~FallMotion() override = default;

  void execute() override;

private:
  FrameInfo startTime;

  struct keyFrame {
      std::vector<double> jointValues; // todo: use std::array<double, 22> here?
      std::vector<double> stiffnessValues;
      double triggerMs;
      double altTriggerGrad;
  };

  const std::vector<keyFrame> forwards = {
    {
      {-38,0,-10,10,90,90, 0,  0,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
      {100,100,  100,100,100,100,  100,100,100,100, 30,30,30,30,30,30, 30,30,30,30},
      100,
      30
    },{
        {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
        {100,100,  100,100,100,100,  100,100,100,100, 30,30,30,30,30,30, 30,30,30,30},
        250,
        55
    },{
        {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
        {15,15,  15,15,15,15,  15,15,15,15, 5,5,5,5,5,5, 5,5,5,5},
        600,
        70
    },{
        {-38,0,-10,10, 5, 5,60,-60,-100,100,0,0,-24,-24,0,0,105,105,-75,-75,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        1500,
        999
    }
  };

  const std::vector<keyFrame> backwards = {
    {
          {29,0,-12,12,123,123, 0,  0,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
          {100,100,  100,100,100,100,  100,100,100,100, 30,30,30,30,30,30, 30,30,30,30},
          150,
          28
      },{
          {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
          {100,100,  100,100,100,100,  100,100,100,100, 30,30,30,30,30,30, 30,30,30,30},
          300,
          55
      },{
          {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
          {15,15,  15,15,15,15,  15,15,15,15, 5,5,5,5,5,5, 5,5,5,5},
          600,
          70
      },{
          {29,0,-12,12,123,123,78,-78,-17,17,0,0,-90,-90,0,0,105,105,-45,-45,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          1500,
          999
      }
  };

  double stiffness_increase;
  double oldStiffness[JointData::numOfJoint]{};
//  double oldJoints[naoth::JointData::numOfJoint]{};
};

#endif  /* FALLMOTION_H */
