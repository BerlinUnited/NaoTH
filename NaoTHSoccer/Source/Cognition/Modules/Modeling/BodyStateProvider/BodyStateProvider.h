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
//#include <Tools/DataStructures/ParameterList.h>
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
  virtual ~BodyStateProvider(){
    getDebugParameterList().remove(&theParams);
  }

  void execute();

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("BodyStateParameters")
    {
      PARAMETER_REGISTER(getup_threshold) = 1.2;

      PARAMETER_REGISTER(batteryChargingThreshold) = 0.8;
      
      PARAMETER_REGISTER(maxTimeForLiftUpStand) = 500;
      PARAMETER_REGISTER(maxTimeForLiftUpWalk) = 1000;
      syncWithConfig();
    }

    ~Parameters()
    {
    }

    double getup_threshold;
    double batteryChargingThreshold;
    
    double maxTimeForLiftUpStand;
    double maxTimeForLiftUpWalk;
  } theParams;


  void updateTheFallDownState();

  void updateTheFootState();

  void updateTheLegTemperature();

  void updateIsLiftedUp();

  // internal data
  RingBufferWithSum<Vector2<double>, 10> inertialBuffer;

  // filter the battery state
  RingBufferWithSum<double, 1000> batteryChargeBuffer;
};

#endif //__BodyStateProvider_h_
