#ifndef TEAMCOMMSENDER_H
#define TEAMCOMMSENDER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/PlayerInfo.h"
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/TeamMessage.h"

BEGIN_DECLARE_MODULE(TeamCommSender)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotInfo)
  REQUIRE(RobotPose)
  REQUIRE(BallModel)

  PROVIDE(TeamMessageDataOut)
END_DECLARE_MODULE(TeamCommSender)

class TeamCommSender: public TeamCommSenderBase
{
public:
  TeamCommSender();

  virtual void execute();

private:
  unsigned int lastSentTimestamp;
  unsigned int send_interval;

  void createMessage(naothmessages::TeamCommMessage& msg);
};

#endif // TEAMCOMMSENDER_H
