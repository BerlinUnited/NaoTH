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

    void socketLoop();

private:
    void update();

private:
    bool exiting;
    GSocket* socket;
    GSocketAddress* broadcastAddress;
    GThread* socketThread;
    GMutex*  dataMutex;
    struct Data {
      RoboCupGameControlData dataIn;
      RoboCupGameControlReturnData dataOut;
    } theData[2];

    int lastUpdatedIndex;
    int writingIndex;

    GError* bindAndListen(unsigned int port = GAMECONTROLLER_PORT);

    void sendData(const RoboCupGameControlReturnData& data);
};

#endif // SPLGAMECONTROLLER_H
