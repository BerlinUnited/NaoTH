#ifndef SPLGAMECONTROLLER_H
#define SPLGAMECONTROLLER_H

#include <gio/gio.h>
#include <MessagesSPL/RoboCupGameControlData.h>
#include "Representations/Infrastructure/GameData.h"

using namespace spl;

class SPLGameController
{
public:
  SPLGameController();

  ~SPLGameController();

  void get(naoth::GameData& gameData);
  void set(const naoth::GameReturnData& data);

  void socketLoop();

private:
  bool update();

private:
  bool exiting;
  int returnPort;
  GSocket* socket;
  GSocketAddress* gamecontrollerAddress;
  GThread* socketThread;

  RoboCupGameControlData dataIn;
  RoboCupGameControlReturnData dataOut;

  unsigned int lastGetTime;
  naoth::GameData data;
  GMutex*  dataMutex;
  GMutex* returnDataMutex;

  GError* bindAndListen(unsigned int port = GAMECONTROLLER_DATA_PORT);

  void sendData(const RoboCupGameControlReturnData& data);
};

#endif // SPLGAMECONTROLLER_H
