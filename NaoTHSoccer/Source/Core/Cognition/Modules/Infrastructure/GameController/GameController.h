#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/LEDRequest.h>
#include "Representations/Infrastructure/ButtonState.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Modeling/PlayerInfo.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(GameController)
  REQUIRE(ButtonState)
  REQUIRE(GameData)
  REQUIRE(FrameInfo)

  PROVIDE(GameReturnData)
  PROVIDE(GameControllerLEDRequest)
  PROVIDE(PlayerInfo)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(GameController)

class GameController : public GameControllerBase
{
public:
  GameController();
  virtual void execute();
  virtual ~GameController();

private:
  void readButtons();
  void readHeadButtons();
  void updateLEDs();
};

#endif // GAMECONTROLLER_H
