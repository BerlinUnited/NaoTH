/**
* @file BodyStateProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class BodyStateProvider
*/

#ifndef __BodyStateProvider_h_
#define __BodyStateProvider_h_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/JointData.h"

#include "Representations/Modeling/BodyState.h"

// Tools
#include "Tools/DataStructures/RingBufferWithSum.h"

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
  ~BodyStateProvider(){}

  /** executes the module */
  void execute();

private:

  void updateTheFallDownState();

  void updateTheFootState();

  void updateTheLegTemperature();

  // internal data
  RingBufferWithSum<Vector2<double>, 10> inertialBuffer;

};

#endif //__BodyStateProvider_h_
