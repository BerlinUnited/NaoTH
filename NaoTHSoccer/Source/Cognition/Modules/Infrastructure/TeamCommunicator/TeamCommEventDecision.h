#ifndef TEAMCOMMEVENTDECISION_H
#define TEAMCOMMEVENTDECISION_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Modeling/PlayerInfo.h"

#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/RingBufferWithStddev.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

#include "Representations/Modeling/TeamMessageDecision.h"
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessageStatistics.h"


BEGIN_DECLARE_MODULE(TeamCommEventDecision)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamState)

  PROVIDE(TeamMessageDecision)
END_DECLARE_MODULE(TeamCommEventDecision)


class TeamCommEventDecision: public TeamCommEventDecisionBase
{
public:
    TeamCommEventDecision();
    virtual ~TeamCommEventDecision();

    virtual void execute();


private:
    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("TeamCommEventDecision") {
            PARAMETER_REGISTER(byInterval_interval) = 400;
            // load from the file after registering all parameters
            syncWithConfig();
        }
        virtual ~Parameters() {}

        unsigned int byInterval_lastSentTimestamp = 0; // not configurable
        unsigned int byInterval_interval = 0;
    } params;

    void byInterval();
};

#endif  /* TEAMCOMMEVENTDECISION_H */
