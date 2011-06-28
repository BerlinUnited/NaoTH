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

    bool update(naoth::GameData& gameData, unsigned int time);

private:
    GSocket* socket;
    char* buffer;
    GSocketAddress* wlanBroadcastAddress;

    GError* bindAndListen(unsigned int port = GAMECONTROLLER_PORT);

    void returnData(const naoth::GameData& gameData);
};

#endif // SPLGAMECONTROLLER_H
