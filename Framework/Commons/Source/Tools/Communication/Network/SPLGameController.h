#ifndef SPLGAMECONTROLLER_H
#define SPLGAMECONTROLLER_H

#include <gio/gio.h>
#include "MessagesSPL/RoboCupGameControlData.h"
#include "Representations/Infrastructure/GameData.h"

#include <mutex>
#include <thread>

class SPLGameController
{
public:
  SPLGameController();
  ~SPLGameController();

  void get(naoth::GameData& gameData);
  void set(const naoth::GameReturnData& data);

private:
  void socketLoop();
  bool update();

private:
  bool exiting;
  std::thread socketThread;
  
  GSocket* socket;
  GCancellable* cancelable;

  int returnPort;
  GSocketAddress* gamecontrollerAddress;

  hsl::RoboCupGameControlData dataIn;
  hsl::RoboCupGameControlReturnData dataOut;

  naoth::GameData data;
  std::mutex  dataMutex;
  std::mutex returnDataMutex;

  GError* bindAndListen(unsigned int port = GAMECONTROLLER_DATA_PORT);

  void sendData(const hsl::RoboCupGameControlReturnData& data);
};

#endif // SPLGAMECONTROLLER_H
