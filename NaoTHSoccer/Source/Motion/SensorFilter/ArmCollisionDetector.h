/**
* @file ArmCollisionDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich, Mellmann</a>
* ArmCollisionDetector: detect collisions with arms based on joint position deviation.
*/

#ifndef ARM_COLLISION_DETECTOR_H
#define ARM_COLLISION_DETECTOR_H

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>

#include "Representations/Motion/MotionStatus.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include "Representations/Motion/CollisionPercept.h"

// tools
#include <Tools/DataStructures/RingBufferWithSum.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugParameterList.h"


BEGIN_DECLARE_MODULE(ArmCollisionDetector)
  REQUIRE(FrameInfo)
  REQUIRE(MotorJointData)
  REQUIRE(SensorJointData)
  REQUIRE(MotionStatus)
  REQUIRE(MotionRequest)

  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugParameterList)

  PROVIDE(CollisionPercept)
END_DECLARE_MODULE(ArmCollisionDetector)

class ArmCollisionDetector : private ArmCollisionDetectorBase
{
public:
  ArmCollisionDetector();
  ~ArmCollisionDetector();

  void execute();

public:
  class Parameter : public ParameterList
  {
  public:
    Parameter() : ParameterList("ArmCollisionDetector") 
    {
      PARAMETER_REGISTER(maxErrorStand) = 0.02;
      PARAMETER_REGISTER(maxErrorWalk) = 0.05;
      syncWithConfig();
    }

    double maxErrorStand;
    double maxErrorWalk;
  } params;


private:
  RingBufferWithSum<double, 100> collisionBufferLeft;
  RingBufferWithSum<double, 100> collisionBufferRight;

  RingBuffer<double, 4> jointDataBufferLeft;
  RingBuffer<double, 4> jointDataBufferRight;
  //naoth::FrameInfo frameWhenArmsBack;
  //ArmMotionRequest::ArmMotionID armStatus;
};

#endif // ARM_COLLISION_DETECTOR_H
