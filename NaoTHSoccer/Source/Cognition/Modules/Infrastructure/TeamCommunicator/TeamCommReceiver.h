#ifndef TEAMCOMMRECEIVER_H
#define TEAMCOMMRECEIVER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/PlayerInfo.h"

#include <Tools/DataStructures/RingBuffer.h>

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"

#include <MessagesSPL/SPLStandardMessage.h>

BEGIN_DECLARE_MODULE(TeamCommReceiver)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamMessageDataIn)

  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  PROVIDE(TeamMessage)
  PROVIDE(TeamMessageData)
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
  void handleMessage(const std::string& data, bool allowOwn = false);
  
  bool monotonicTimeStamp(const TeamMessageData& data) const
  {
    return 
           // it's probably not our message (playing dropin => accept in any case)
           data.timestamp == 0 ||
           // the new message is monotonic => accept
           data.timestamp > getTeamMessage().data[data.playerNumber].timestamp ||
           // the new message is much older than the current one => weird => reset
           data.timestamp + parameters.monotonicTimestampCheckResetTime < getTeamMessage().data[data.playerNumber].timestamp;
  }

  RingBuffer<std::string, 100> delayBuffer;
  int droppedMessages;
};

#endif // TEAMCOMMRECEIVER_H
