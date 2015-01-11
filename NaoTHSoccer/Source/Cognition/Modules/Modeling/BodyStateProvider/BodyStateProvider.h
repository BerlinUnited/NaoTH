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

#include "Representations/Modeling/BodyState.h"

// Tools
#include "Tools/DataStructures/RingBufferWithSum.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(BodyStateProvider)
  REQUIRE(InertialSensorData)
  REQUIRE(FSRData)
  REQUIRE(FrameInfo)
  REQUIRE(SensorJointData)

  PROVIDE(BodyState)
END_DECLARE_MODULE(BodyStateProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class BodyStateProvider : public BodyStateProviderBase
{
public:

  BodyStateProvider();
  virtual ~BodyStateProvider(){}

  void execute();

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("BodyStateParameters")
    {
      PARAMETER_REGISTER(getup_threshold) = 1.2;
      PARAMETER_REGISTER(foot_threshold) = 1;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    double foot_threshold;
    double getup_threshold;
  } theParams;


  void updateTheFallDownState();

  void updateTheFootState();

  void updateTheLegTemperature();

  // internal data
  RingBufferWithSum<Vector2<double>, 10> inertialBuffer;

};

#endif //__BodyStateProvider_h_
