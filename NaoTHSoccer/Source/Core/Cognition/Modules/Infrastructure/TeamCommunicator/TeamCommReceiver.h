#ifndef TEAMCOMMRECEIVER_H
#define TEAMCOMMRECEIVER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"

BEGIN_DECLARE_MODULE(TeamCommReceiver)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotInfo)
  REQUIRE(TeamMessageDataIn)

  PROVIDE(TeamMessage)
END_DECLARE_MODULE(TeamCommReceiver)

class TeamCommReceiver: public TeamCommReceiverBase
{
public:
  virtual void execute();

private:
  void handleMessage(const std::string& data);
};

#endif // TEAMCOMMRECEIVER_H
