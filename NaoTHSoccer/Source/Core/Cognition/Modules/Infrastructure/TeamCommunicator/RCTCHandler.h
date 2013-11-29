#ifndef _RCTCHandler_H_
#define _RCTCHandler_H_

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/RobotInfo.h>

#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"

#include "Representations/Modeling/BodyState.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/PlayersModel.h"
#include "Representations/Modeling/SPLStandardMessage.h"

#include "Tools/Communication/RCTC/TeamMessage.h"


BEGIN_DECLARE_MODULE(RCTCHandler)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotInfo)
  REQUIRE(RobotPose)
  REQUIRE(BallModel)
  REQUIRE(BodyState)
  REQUIRE(MotionStatus)
  REQUIRE(SoccerStrategy)
  REQUIRE(PlayersModel)
  
  PROVIDE(ExtendedSPLStandardMessage)
  REQUIRE(RCTCTeamMessageDataIn)
  PROVIDE(RCTCTeamMessageDataOut)
END_DECLARE_MODULE(RCTCHandler)

class RCTCHandler: public RCTCHandlerBase
{
public:
  RCTCHandler();

  virtual void execute();

private:
  unsigned int lastSentTimestamp;
  unsigned int send_interval;

  void createMessage(rctc::Message& msg);
};

#endif // _RCTCHandler_H_
