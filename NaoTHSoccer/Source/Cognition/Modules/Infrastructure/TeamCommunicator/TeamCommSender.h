#ifndef TEAMCOMMSENDER_H
#define TEAMCOMMSENDER_H

#include <MessagesSPL/SPLStandardMessage.h>
#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include "Representations/Infrastructure/CpuData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/KickActionModel.h"

BEGIN_DECLARE_MODULE(TeamCommSender)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotInfo)
  REQUIRE(RobotPose)
  REQUIRE(BallModel)
  REQUIRE(BodyState)
  REQUIRE(MotionStatus)
  REQUIRE(RoleDecisionModel)
  REQUIRE(SoccerStrategy)
  REQUIRE(PlayersModel)
  REQUIRE(TeamMessage)
  REQUIRE(BatteryData)
  REQUIRE(KickActionModel)
  REQUIRE(CpuData)

  PROVIDE(TeamMessageData)
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

  void fillMessageBeforeSending() const;
};

#endif // TEAMCOMMSENDER_H
