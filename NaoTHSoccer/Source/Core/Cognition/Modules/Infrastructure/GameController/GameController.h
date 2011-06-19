#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/ButtonData.h>
#include "Representations/Modeling/PlayerInfo.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(GameController)
REQUIRE(ButtonData)
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

    void loadPlayerInfoFromFile();
    void readButtons();
    void readWLAN();
};

#endif // GAMECONTROLLER_H
