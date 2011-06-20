#ifndef TEAMCOMMUNICATOR_H
#define TEAMCOMMUNICATOR_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamMessage.h"

#include <glib.h>
#include <gio/gio.h>

BEGIN_DECLARE_MODULE(TeamCommunicator)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamMessageData)

  PROVIDE(TeamMessage)
  PROVIDE(RobotMessageData)
END_DECLARE_MODULE(TeamCommunicator)

class TeamCommunicator : public TeamCommunicatorBase
{

public:
  TeamCommunicator();

  virtual void execute();

  virtual ~TeamCommunicator();
private:
  unsigned int lastSentTimestamp;

  void handleMessage(const std::string& data);
  void createMessage(naothmessages::TeamCommMessage& msg);
};

#endif // TEAMCOMMUNICATOR_H
