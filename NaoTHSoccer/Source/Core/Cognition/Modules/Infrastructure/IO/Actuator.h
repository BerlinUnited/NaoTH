/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _ACTUATOR_H
#define  _ACTUATOR_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>

#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/DebugMessage.h>

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Motion/Request/MotionRequest.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(Actuator)
  REQUIRE(HeadMotionRequest)
  REQUIRE(CameraSettingsRequest)
  REQUIRE(LEDData)
  REQUIRE(IRSendData)
  REQUIRE(SoundPlayData)
  REQUIRE(MotionStatus)
  REQUIRE(TeamMessageDataOut)
  REQUIRE(DebugMessageOut)

  // HACK: check execute()
  PROVIDE(MotionRequest)
END_DECLARE_MODULE(Actuator)

class Actuator : public ActuatorBase
{
public:
  Actuator();
  ~Actuator();

  virtual void execute();

  void init(naoth::PlatformInterfaceBase& platformInterface);
  
};

#endif  /* _ACTUACTOR_H */

