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

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"

#include <Representations/Modeling/SPLStandardMessage.h>

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

  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  PROVIDE(TeamMessage)
END_DECLARE_MODULE(TeamCommReceiver)

class TeamCommReceiver: public TeamCommReceiverBase
{
public:

  TeamCommReceiver();

  virtual void execute();

  virtual ~TeamCommReceiver();

private:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("TeamCommReceiver")
    {
      PARAMETER_REGISTER(monotonicTimestampCheckResetTime) = 30000;
      PARAMETER_REGISTER(monotonicTimestampCheck) = true;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    int monotonicTimestampCheckResetTime;
    bool monotonicTimestampCheck;
    
    virtual ~Parameters() {}
  } parameters;

private:
  bool parseSPLStandardMessage(const std::string& data, SPLStandardMessage& msg) const;
  bool parseTeamMessage(const SPLStandardMessage& msg, TeamMessage::Data& teamMsg) const;
  void handleMessage(const std::string& data, bool allowOwn = false);
  
  bool monotonicTimeStamp(const TeamMessage::Data& data) const
  {
    return 
           // it's probably not our message (playing dropin => accept in any case)
           data.timestamp == 0 ||
           // the new message is monotonic => accept
           data.timestamp > getTeamMessage().data[data.playerNum].timestamp || 
           // the new message is much older than the current one => weird => reset
           data.timestamp + parameters.monotonicTimestampCheckResetTime < getTeamMessage().data[data.playerNum].timestamp;
  }

  RingBuffer<std::string, 100> delayBuffer;
  int droppedMessages;
};

#endif // TEAMCOMMRECEIVER_H
