#ifndef TEAMCOMMDEBUGGER_H
#define TEAMCOMMDEBUGGER_H

#include <arpa/inet.h>

#include <ModuleFramework/Module.h>
#include <MessagesSPL/SPLStandardMessage.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageDebug.h>
#include <Representations/Infrastructure/BatteryData.h>
#include "Representations/Infrastructure/CpuData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Infrastructure/WifiMode.h"
#include "Representations/Perception/WhistlePercept.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(TeamCommDebugger)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(WifiMode)
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(RobotInfo)
  REQUIRE(BatteryData)
  REQUIRE(BodyState)
  REQUIRE(CpuData)
  REQUIRE(WhistlePercept)
  REQUIRE(BallModel)
  REQUIRE(TeamBallModel)
  REQUIRE(TeamState)

  PROVIDE(TeamMessageDebug)
END_DECLARE_MODULE(TeamCommDebugger)

class TeamCommDebugger: public TeamCommDebuggerBase
{
public:
  TeamCommDebugger() { getDebugParameterList().add(&parameters); }
  ~TeamCommDebugger() { getDebugParameterList().remove(&parameters); }

  virtual void execute();

private:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("TeamCommDebugger")
    {
      PARAMETER_REGISTER(host, &Parameters::setHost) = "127.0.0.1";
      PARAMETER_REGISTER(port) = 10704;
      PARAMETER_REGISTER(send_interval) = 2000;
      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    std::string host;
    unsigned int port;
    unsigned int send_interval;
    
    // the host is set, if the host is a correct ip address
    bool setHost(std::string h) {
      struct sockaddr_in sa;
      return inet_pton(AF_INET, h.c_str(), &(sa.sin_addr)) == 1;
    }

    virtual ~Parameters() {}
  } parameters;

private:
  unsigned int lastSentTimestamp = 0;
};

#endif // TEAMCOMMDEBUGGER_H
