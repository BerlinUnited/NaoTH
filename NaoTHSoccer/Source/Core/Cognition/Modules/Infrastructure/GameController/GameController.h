#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/LEDRequest.h>
#include <Representations/Infrastructure/ButtonData.h>
#include "Representations/Modeling/PlayerInfo.h"

#include <Tools/Communication/RoboCupGameControlData.h>

#include <gio/gio.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(GameController)
  REQUIRE(ButtonData)

  PROVIDE(GameControllerLEDRequest)
  PROVIDE(PlayerInfo)
END_DECLARE_MODULE(GameController)

class GameController : public GameControllerBase
{
public:
    GameController();
    virtual void execute();
    virtual ~GameController();

private:
    int lastChestButtonEventCounter;
    GSocket* socket;

    void loadPlayerInfoFromFile();
    void readButtons();
    void readWLAN();
    void updateLEDs();
    GError* bindAndListen(unsigned int port = GAMECONTROLLER_PORT);
};

#endif // GAMECONTROLLER_H
