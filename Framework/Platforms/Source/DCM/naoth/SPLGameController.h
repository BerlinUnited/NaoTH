#ifndef SPLGAMECONTROLLER_H
#define SPLGAMECONTROLLER_H

#include <gio/gio.h>
#include "RoboCupGameControlData.h"
#include "Representations/Infrastructure/GameData.h"

class SPLGameController
{
public:
    SPLGameController();

    ~SPLGameController();

private:
    GSocket* socket;
    char* buffer;

    void update(naoth::GameData& gameData);

    GError* bindAndListen(unsigned int port = GAMECONTROLLER_PORT);
};

#endif // SPLGAMECONTROLLER_H
