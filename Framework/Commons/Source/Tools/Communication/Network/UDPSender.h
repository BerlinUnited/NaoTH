#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <gio/gio.h>
#include <MessagesSPL/RoboCupGameControlData.h>
#include "Representations/Infrastructure/GameData.h"

#include <mutex>
#include <thread>

using namespace spl;

class UDPSender
{
public:
  UDPSender(std::string ip, unsigned int port) : UDPSender(ip, port, "UDPSender") {};
  UDPSender(std::string ip, unsigned int port, std::string name);
  ~UDPSender();

  void send(const std::string& data);

private:
  void socketLoop();

  std::string ip;
  unsigned int port;
  std::string name;

  std::string dataOut = "Hello World";
  std::mutex  dataMutex;
  std::mutex returnDataMutex;

  bool exiting = false;
  GCancellable* cancelable;
  std::thread socketThread;

  GSocketAddress* address = NULL;
  GSocket* socket = NULL;

  GError* bindAddress();
  void socketSend(const std::string& data);
};

#endif // UDPSENDER_H
