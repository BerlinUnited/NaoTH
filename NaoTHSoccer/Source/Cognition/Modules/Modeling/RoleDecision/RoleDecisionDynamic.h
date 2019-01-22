#ifndef ROLEDECISIONDYNAMIC_H
#define ROLEDECISIONDYNAMIC_H

#include <ModuleFramework/Module.h>

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"


BEGIN_DECLARE_MODULE(RoleDecisionDynamic)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(GameData)
  REQUIRE(PlayerInfo)
//  REQUIRE(SoccerStrategy)
  REQUIRE(TeamMessage)
  REQUIRE(TeamMessagePlayersState)

  REQUIRE(TeamMessageData)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionDynamic);


typedef RoleDecisionModel RM;


class RoleDecisionDynamic : public RoleDecisionDynamicBase
{
public:
    RoleDecisionDynamic()
    {
        getDebugParameterList().add(&params);
    }

    virtual ~RoleDecisionDynamic()
    {
        getDebugParameterList().remove(&params);
    }

    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("RoleDecisionDynamic")
        {
            PARAMETER_REGISTER(striker_ball_lost_time) = 1000.0;
            PARAMETER_REGISTER(striker_ball_bonus_time) = 4000.0;
            PARAMETER_REGISTER(striker_indicator_bonus) = 300.0;
            PARAMETER_REGISTER(striker_ball_difference_distance) = 500.0;
            PARAMETER_REGISTER(striker_goalie_ball_distance) = 1500.0;
            PARAMETER_REGISTER(striker_goalie_min_x_pos) = -2700.0;
        }

        double striker_ball_lost_time;
        double striker_ball_bonus_time;
        double striker_indicator_bonus;

        double striker_ball_difference_distance;

        double striker_goalie_ball_distance;
        double striker_goalie_min_x_pos;
    } params;

    struct Striker {
        unsigned int playerNumber;
        double indicator;
        Vector2d ball;
    };

    void decideStriker(std::map<unsigned int, RM::DynamicRole>& roles);
    void decideGoalieSupporter(std::map<unsigned int, RM::DynamicRole>& roles);
    void decideSupporter(std::map<unsigned int, RM::DynamicRole>& roles);

    void checkStriker(const unsigned int& playerNumber, const double& indicator, const Vector2d& ball, std::vector<Striker>& striker, bool force = false);

    double strikerIndicatorDistance(const TeamMessageData& msg);
    double strikerIndicatorTimeToBall(const TeamMessageData& msg);
};

#endif // ROLEDECISIONDYNAMIC_H
