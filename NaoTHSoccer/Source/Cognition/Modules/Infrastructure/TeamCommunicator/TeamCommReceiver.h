#ifndef TEAMCOMMRECEIVER_H
#define TEAMCOMMRECEIVER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Infrastructure/WifiMode.h"

#include <Tools/DataStructures/RingBuffer.h>

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"

#include <MessagesSPL/SPLStandardMessage.h>

BEGIN_DECLARE_MODULE(TeamCommReceiver)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)

  REQUIRE(TeamMessageDataIn)
  REQUIRE(WifiMode)

  PROVIDE(TeamState)
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
      PARAMETER_REGISTER(acceptMixedTeamMessages) = false;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    int monotonicTimestampCheckResetTime;
    bool monotonicTimestampCheck;
    bool acceptMixedTeamMessages;
    
    virtual ~Parameters() {}
  } parameters;

private:
  void handleMessage(const std::string& data);
  void updateTeamState(const TeamMessageData& msg);

  // TODO: move this into SPLStandardMessage.h or some other more suitable place
  /** Parses the informations of the string (spl message) and updates the corresponding fields of this object.
  *  A player/team number different than 0 defines which messages of a player/team are only parsed (restricted to this number).
  *  If the number is negative, the messages of the player/team are ignored instead of restricted!
  */
  static bool parseFromSplMessageString(const std::string &data, SPLStandardMessage& spl);

  bool monotonicTimeStamp(const TeamMessageData& data) const
  {
    return 
      // it's probably not our message (playing dropin => accept in any case)
      data.custom.timestamp == 0 ||
      // the new message is monotonic => accept
      data.custom.timestamp > getTeamMessage().data[data.playerNumber].custom.timestamp ||
      // the new message is much older than the current one => weird => reset
      data.custom.timestamp + parameters.monotonicTimestampCheckResetTime < getTeamMessage().data[data.playerNumber].custom.timestamp;
  }

  RingBuffer<std::string, 100> delayBuffer;
};

#endif // TEAMCOMMRECEIVER_H
