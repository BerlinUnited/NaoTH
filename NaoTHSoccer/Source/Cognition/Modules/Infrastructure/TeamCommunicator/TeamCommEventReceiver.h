#ifndef TEAMCOMMEVENTRECEIVER_H
#define TEAMCOMMEVENTRECEIVER_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Infrastructure/WifiMode.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"

BEGIN_DECLARE_MODULE(TeamCommEventReceiver)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)

  REQUIRE(TeamMessageDataIn)
  REQUIRE(WifiMode)

  PROVIDE(TeamMessage)
  PROVIDE(TeamMessageData)
END_DECLARE_MODULE(TeamCommEventReceiver)

class TeamCommEventReceiver: public TeamCommEventReceiverBase
{
public:

  TeamCommEventReceiver();

  virtual void execute();

  virtual ~TeamCommEventReceiver();

private:
  class Parameters: public ParameterList
  {
  public: 
    Parameters(): ParameterList("TeamCommEventReceiver")
    {      
      // load from the file after registering all parameters
      syncWithConfig();
    }

    virtual ~Parameters() {}
  } parameters;

private:
  void handleMessage(const std::string& data);
};

#endif // TEAMCOMMEVENTRECEIVER_H
