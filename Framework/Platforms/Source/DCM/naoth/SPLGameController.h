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

    bool get(naoth::GameData& gameData, unsigned int time);

    void setReturnData(const naoth::GameReturnData& data);

    void socketLoop();

private:
    bool update();

private:
    bool exiting;
    GSocket* socket;
    GSocketAddress* broadcastAddress;
    GThread* socketThread;
    RoboCupGameControlData dataIn;
    RoboCupGameControlReturnData dataOut;

    bool dataUpdated;
    naoth::GameData data;
    GMutex*  dataMutex;
    GMutex* returnDataMutex;

    GError* bindAndListen(unsigned int port = GAMECONTROLLER_PORT);

    void sendData(const RoboCupGameControlReturnData& data);
};

#endif // SPLGAMECONTROLLER_H
