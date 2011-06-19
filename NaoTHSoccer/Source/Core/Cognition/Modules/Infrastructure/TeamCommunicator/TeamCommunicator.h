#ifndef TEAMCOMMUNICATOR_H
#define TEAMCOMMUNICATOR_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/TeamMessage.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include <glib.h>
#include <gio/gio.h>

BEGIN_DECLARE_MODULE(TeamCommunicator)
  REQUIRE(FrameInfo)

  PROVIDE(TeamMessage)
END_DECLARE_MODULE(TeamCommunicator)

class TeamCommunicator : public TeamCommunicatorBase
{

public:
  TeamCommunicator();

  virtual void execute();

  virtual ~TeamCommunicator();
private:
  bool initialized;
  GSocket* socket;
  unsigned int lastSentTimestamp;

  GSocketAddress* lanBroadcastAddress;
  GSocketAddress* wlanBroadcastAddress;

  GError* bindAndListen(unsigned int port);
  void handleMessage(char* buffer, gsize size);
  void createMessage(naothmessages::TeamCommMessage& msg);
};

#endif // TEAMCOMMUNICATOR_H
