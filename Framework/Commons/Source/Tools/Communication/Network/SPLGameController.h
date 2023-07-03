#ifndef SPLGAMECONTROLLER_H
#define SPLGAMECONTROLLER_H

#include <gio/gio.h>
#include <MessagesSPL/RoboCupGameControlData.h>
#include "Representations/Infrastructure/GameData.h"

#include <mutex>
#include <thread>

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
  GCancellable* cancelable;

  GSocketAddress* gamecontrollerAddress;
  std::thread socketThread;

  RoboCupGameControlData dataIn;
  RoboCupGameControlReturnData dataOut;

  naoth::GameData data;
  std::mutex  dataMutex;
  std::mutex returnDataMutex;

  GError* bindAndListen(unsigned int port = GAMECONTROLLER_DATA_PORT);

  void sendData(const RoboCupGameControlReturnData& data);
};

#endif // SPLGAMECONTROLLER_H
