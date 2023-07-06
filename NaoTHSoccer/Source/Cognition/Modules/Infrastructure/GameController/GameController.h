#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <ModuleFramework/Module.h>
#include <Tools/Debug/DebugRequest.h>

#include <PlatformInterface/PlatformInterface.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/LEDRequest.h>
#include "Representations/Infrastructure/ButtonState.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Perception/WhistlePercept.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/WifiMode.h"

#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(GameController)
  PROVIDE(DebugRequest)
  
  REQUIRE(ButtonState)
  REQUIRE(FrameInfo)
  REQUIRE(WhistlePercept)
  REQUIRE(WifiMode)

  REQUIRE(RobotPose)
  REQUIRE(BallModel)

  PROVIDE(GameData)
  PROVIDE(PlayerInfo)
  PROVIDE(GameReturnData)
  PROVIDE(GameControllerLEDRequest)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(GameController)

class GameController : public GameControllerBase
{
public:
  GameController();
  virtual void execute();
  virtual ~GameController();

private:
  void handleDebugRequest();
  void handleButtons();
  void handleHeadButtons();
  void updateLEDs();

private:
  bool debug_whistle_heard;
  bool play_by_whistle;
};

#endif // GAMECONTROLLER_H
