/**
* @file BodyStateProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class BodyStateProvider
*/

#ifndef _BodyStateProvider_h_
#define _BodyStateProvider_h_

#include <ModuleFramework/Module.h>


// Representations
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/BatteryData.h"
#include "Representations/Modeling/GroundContactModel.h"
#include "Representations/Motion/MotionStatus.h"

#include "Representations/Modeling/BodyState.h"

// Tools
#include "Tools/DataStructures/RingBufferWithSum.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugParameterList.h>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(BodyStateProvider)
  // debug
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
	
  REQUIRE(InertialSensorData)
  REQUIRE(FSRData)
  REQUIRE(FrameInfo)
  REQUIRE(SensorJointData)
  REQUIRE(BatteryData)
  REQUIRE(GroundContactModel)
  REQUIRE(MotionStatus)

  PROVIDE(BodyState)
END_DECLARE_MODULE(BodyStateProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class BodyStateProvider : public BodyStateProviderBase
{
public:

  BodyStateProvider();
  virtual ~BodyStateProvider();

  void execute();

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("BodyStateParameters")
    {
      PARAMETER_ANGLE_REGISTER(getup_threshold) = 70; // in deg
      PARAMETER_ANGLE_REGISTER(upright_threshold) = 30; // in deg

      PARAMETER_REGISTER(lifted_up_time) = 500; // in ms
      PARAMETER_REGISTER(lifted_up_time_walk) = 1000; // in ms

      PARAMETER_REGISTER(batteryChargingThreshold) = 0.8; // in A
      syncWithConfig();
    }

    double upright_threshold;
    double getup_threshold;

    double lifted_up_time;
    double lifted_up_time_walk;
    double batteryChargingThreshold;
  } params;


  void updateTheFallDownState();

  void updateTheFootState();

  void updateTheLegTemperature();

  void updateIsLiftedUp();

  /**
   * @brief Updates the 'ready to walk' body state.
   *        The robot is 'ready to walk', if he's not fallen, not lifted up and is able to walk
   *        (see determineReadyToWalkState()).
   */
  void updateIsReadyToWalk();

  /**
   * @brief Determines, whether the robot is ready to walk.
   *        The robot is ready to walk, if he's already walking or if the stand motion is completed.
   * @return true, if ready to walk, false otherwise
   */
  inline bool determineReadyToWalkState();

  // internal data
  RingBufferWithSum<Vector2d, 10> inertialBuffer;

  // filter the battery state
  RingBufferWithSum<double, 1000> batteryChargeBuffer;
};

#endif //__BodyStateProvider_h_
