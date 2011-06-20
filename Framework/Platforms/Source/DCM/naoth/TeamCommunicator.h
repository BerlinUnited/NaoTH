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
private:
  bool initialized;
  GSocket* socket;
  char* buffer;
  unsigned int lastSentTimestamp;

  GSocketAddress* lanBroadcastAddress;
  GSocketAddress* wlanBroadcastAddress;

  GError* bindAndListen(unsigned int port);
};
} // namespace naoth

#endif // NAOTH_TEAMCOMMUNICATOR_H
