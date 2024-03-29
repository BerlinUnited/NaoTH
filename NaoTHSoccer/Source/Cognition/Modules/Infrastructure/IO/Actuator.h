/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _ACTUATOR_H
#define _ACTUATOR_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>

#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/TeamMessageDebug.h>
#include <Representations/Infrastructure/DebugMessage.h>
#include <Representations/Infrastructure/GameData.h>
#include "Representations/Infrastructure/AudioControl.h"

#include "Representations/Motion/Request/HeadMotionRequest.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Motion/Request/MotionRequest.h"

#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/CameraMatrixOffset.h"

#include "Representations/Modeling/BodyState.h"


using namespace naoth;

BEGIN_DECLARE_MODULE(Actuator)
  
  REQUIRE(CameraSettingsRequest)
  REQUIRE(CameraSettingsRequestTop)
  REQUIRE(LEDData)
  REQUIRE(UltraSoundSendData)
  REQUIRE(MotionStatus)
  REQUIRE(TeamMessageDataOut)
  REQUIRE(TeamMessageDebug)
  REQUIRE(DebugMessageOut)
  REQUIRE(FrameInfo)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrixOffset)
  REQUIRE(GameReturnData)
  REQUIRE(BodyState)
  REQUIRE(AudioControl)

  // HACK: needed to transform the HeadMotionRequest
  REQUIRE(KinematicChain)

  PROVIDE(SoundPlayData)

  // HACK: check execute()
  PROVIDE(MotionRequest)
  PROVIDE(HeadMotionRequest)
END_DECLARE_MODULE(Actuator)

class Actuator : public ActuatorBase
{
public:
  Actuator();
  virtual ~Actuator();

  virtual void execute();

  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);
  
};

#endif  /* _ACTUACTOR_H */

