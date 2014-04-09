#ifndef TEAMCOMMRECEIVER_H
#define TEAMCOMMRECEIVER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/PlayersModel.h"

#include <Tools/DataStructures/RingBuffer.h>

BEGIN_DECLARE_MODULE(TeamCommReceiver)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotInfo)
  REQUIRE(TeamMessageDataIn)
  // additionally needed to add our own message
  REQUIRE(RobotPose)
  REQUIRE(BallModel)
  REQUIRE(BodyState)
  REQUIRE(MotionStatus)
  REQUIRE(SoccerStrategy)
  REQUIRE(PlayersModel)
  REQUIRE(BatteryData)

  PROVIDE(TeamMessage)
END_DECLARE_MODULE(TeamCommReceiver)

class TeamCommReceiver: public TeamCommReceiverBase
{
public:

  TeamCommReceiver();

  virtual void execute();

private:
  void handleMessage(const std::string& data, bool allowOwn = false);

  RingBuffer<std::string, 100> delayBuffer;
  int droppedMessages;
};

#endif // TEAMCOMMRECEIVER_H
