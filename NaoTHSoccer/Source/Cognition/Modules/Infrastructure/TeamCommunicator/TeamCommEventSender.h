#ifndef TEAMCOMMEVENTSENDER_H
#define TEAMCOMMEVENTSENDER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/GameData.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Infrastructure/WifiMode.h"
#include "Representations/Modeling/TeamMessageNTP.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/TeamMessageDecision.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"


typedef naothmessages::TeamState::Player Message;


BEGIN_DECLARE_MODULE(TeamCommEventSender)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(WifiMode)
  REQUIRE(GameData)
  REQUIRE(TeamMessageNTP)
  REQUIRE(MotionStatus)
  REQUIRE(RoleDecisionModel)
  REQUIRE(RobotPose)
  REQUIRE(BallModel)
  REQUIRE(BodyState)
  REQUIRE(SoccerStrategy)
  REQUIRE(TeamMessageDecision)

  PROVIDE(TeamState)
  PROVIDE(TeamMessageData)
  PROVIDE(TeamMessageDataOut)
END_DECLARE_MODULE(TeamCommEventSender)

class TeamCommEventSender: public TeamCommEventSenderBase
{
public:
  TeamCommEventSender();
  ~TeamCommEventSender();

  virtual void execute();

private:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("TeamCommEventSender")
    {
      PARAMETER_REGISTER(strictMessageBudget) = true;
      PARAMETER_REGISTER(thresholdMessageBudget) = 10;

      // load from the file after registering all parameters
      syncWithConfig();
    }

    bool strictMessageBudget;
    unsigned int thresholdMessageBudget; // a threshold, when teamcomm should be "disabled"
    
    virtual ~Parameters() {}
  } parameters;

private:
  unsigned int lastSentTimestamp;

  void updateMyself() const;
  inline bool shouldSendMessage() const;
  bool createMessage() const;
};

#endif // TEAMCOMMEVENTSENDER_H
