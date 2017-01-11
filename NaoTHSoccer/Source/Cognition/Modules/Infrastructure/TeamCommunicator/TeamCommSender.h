#ifndef TEAMCOMMSENDER_H
#define TEAMCOMMSENDER_H

#include <MessagesSPL/SPLStandardMessage.h>
#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include "Representations/Modeling/PlayerInfo.h"
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/TeamMessage.h"
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

  PROVIDE(TeamMessageDataOut)
END_DECLARE_MODULE(TeamCommSender)

class TeamCommSender: public TeamCommSenderBase
{
public:
  TeamCommSender();

  virtual void execute();

  static void fillMessage(const PlayerInfo &playerInfo,
                            const RobotInfo &robotInfo,
                            const FrameInfo &frameInfo,
                            const BallModel &ballModel,
                            const RobotPose &robotPose,
                            const BodyState &bodyState,
                            const RoleDecisionModel &roleDecisionModel,
                            const SoccerStrategy &soccerStrategy,
                            const PlayersModel &playersModel,
                            const BatteryData &batteryData,
                            const KickActionModel &kickActionModel,
                            TeamMessage::Data &out);

 static void convertToSPLMessage(const TeamMessage::Data& teamMsg, SPLStandardMessage& splMsg);

private:
  unsigned int lastSentTimestamp;
  unsigned int send_interval;

  void createMessage(SPLStandardMessage &msg);

  static void addSendOppModel(unsigned int oppNum, const PlayersModel &playersModel,
                              TeamMessage::Opponent &out);
};

#endif // TEAMCOMMSENDER_H
