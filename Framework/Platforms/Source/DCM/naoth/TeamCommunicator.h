#ifndef NAOTH_TEAMCOMMUNICATOR_H
#define NAOTH_TEAMCOMMUNICATOR_H

#include <string>
#include <vector>
#include <glib.h>
#include <gio/gio.h>

namespace naoth
{
class TeamCommunicator
{

public:
  TeamCommunicator();

  void send(const std::string& data);

  void receive(std::vector<std::string>& data);

  virtual ~TeamCommunicator();

  void sendLoop();

  void receiveLoop();

private:
  void send();

private:
  bool exiting;
  GSocket* socket;
  char* buffer;
  GThread* sendThread;
  GThread* receiveThread;
  std::vector<std::string> messageIn;
  GMutex*  messageInMutex;
  std::string messageOut;
  GMutex*  messageOutMutex;
  GCond* messageOutCond;

  GSocketAddress* lanBroadcastAddress;
  GSocketAddress* wlanBroadcastAddress;

  GError* bindAndListen(unsigned int port);
};
} // namespace naoth

#endif // NAOTH_TEAMCOMMUNICATOR_H
