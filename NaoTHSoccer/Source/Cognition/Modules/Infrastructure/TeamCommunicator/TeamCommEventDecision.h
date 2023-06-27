#ifndef TEAMCOMMEVENTDECISION_H
#define TEAMCOMMEVENTDECISION_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/GameData.h>
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"

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
  REQUIRE(GameData)
  REQUIRE(RoleDecisionModel)
  REQUIRE(RobotPose)
  REQUIRE(BallModel)

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
            // interval, distance
            PARAMETER_REGISTER(decision_method, &Parameters::setDecisionMethod) = "interval";

            // params for the interval decision method
            PARAMETER_REGISTER(byInterval_interval) = 400;
            
            // params for the distance decision method
            PARAMETER_REGISTER(byDistance_minInterval) = 0;
            PARAMETER_REGISTER(byDistance_defender) = 0.5;
            PARAMETER_REGISTER(byDistance_midfielder) = 0.75;
            PARAMETER_REGISTER(byDistance_forward) = 1.0;

            // load from the file after registering all parameters
            syncWithConfig();
        }
        virtual ~Parameters() {}

        unsigned int byInterval_lastSentTimestamp = 0; // not configurable
        unsigned int byInterval_interval = 0;

        Pose2D byDistance_lastPose; // not configurable
        unsigned int byDistance_lastSentTimestamp = 0; // not configurable
        unsigned int byDistance_minInterval = 0; // safety condition, min interval we're allowed to send a new message
        double byDistance_defender          = 0.5;
        double byDistance_midfielder        = 0.75;
        double byDistance_forward           = 1.0;

        std::string decision_method;
        void (TeamCommEventDecision::*decisionMethod)();
        void setDecisionMethod(std::string method)
        {
            if(method == "distance")          { decisionMethod = &TeamCommEventDecision::byDistance; }
            //else if (method == "distance")    { decisionMethod = &TeamCommEventDecision::byDistance; }
            else                            { decisionMethod = &TeamCommEventDecision::byInterval; }
        }
    } params;

    void byInterval();
    void byDistance();
};

#endif  /* TEAMCOMMEVENTDECISION_H */
