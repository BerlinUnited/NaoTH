#ifndef ROLEDECISIONDYNAMIC_H
#define ROLEDECISIONDYNAMIC_H

#include <functional>
#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/Roles.h"

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"


BEGIN_DECLARE_MODULE(RoleDecisionDynamic)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugDrawings)

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


class RoleDecisionDynamic : public RoleDecisionDynamicBase
{
public:
    RoleDecisionDynamic();

    virtual ~RoleDecisionDynamic();

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
            PARAMETER_REGISTER(striker_ball_difference_function) = 3;
            PARAMETER_REGISTER(striker_ball_difference_distance) = 500.0;
            PARAMETER_REGISTER(striker_ball_difference_distance_m) = 0.35;
            PARAMETER_REGISTER(striker_ball_difference_distance_n) = -40.0;
            PARAMETER_REGISTER(striker_goalie_ball_distance) = 1500.0;
            PARAMETER_REGISTER(striker_goalie_min_x_pos) = -2700.0;
        }

        double striker_ball_lost_time;
        double striker_ball_bonus_time;
        double striker_indicator_bonus;

        int striker_ball_difference_function;
        double striker_ball_difference_distance;
        double striker_ball_difference_distance_m;
        double striker_ball_difference_distance_n;

        double striker_goalie_ball_distance;
        double striker_goalie_min_x_pos;
    } params;

    struct Striker {
        unsigned int playerNumber;
        double indicator;
        Vector2d ball;
        double sameBallRadius;
    };

    void decideStriker(std::map<unsigned int, Roles::Dynamic>& roles);
    void decideGoalieSupporter(std::map<unsigned int, Roles::Dynamic>& roles);
    void decideSupporter(std::map<unsigned int, Roles::Dynamic>& roles);

    void checkStriker(const TeamMessageData& msg, const double& indicator, const Vector2d& ball, std::vector<Striker>& striker, bool force = false);
    void handleGoalie(const TeamMessageData *goalie, std::vector<Striker>& striker);

    std::function<double(double)> ballDifferenceRadius;
    inline double ballDifferenceRadiusConstant(double /*d*/) { return params.striker_ball_difference_distance; }
    inline double ballDifferenceRadiusLinear(double d) { return d * params.striker_ball_difference_distance_m + params.striker_ball_difference_distance_n; }
    inline double ballDifferenceRadiusConstantLinear(double d) { return std::max(ballDifferenceRadiusConstant(d), ballDifferenceRadiusLinear(d)); }
    void ballDifferenceRadiusChanger(int selection) {
        switch (selection) {
            case 1: ballDifferenceRadius = std::bind(&RoleDecisionDynamic::ballDifferenceRadiusConstant, this, std::placeholders::_1); break;
            case 2: ballDifferenceRadius = std::bind(&RoleDecisionDynamic::ballDifferenceRadiusLinear, this, std::placeholders::_1); break;
            case 3: ballDifferenceRadius = std::bind(&RoleDecisionDynamic::ballDifferenceRadiusConstantLinear, this, std::placeholders::_1); break;
            default: ballDifferenceRadius = std::bind(&RoleDecisionDynamic::ballDifferenceRadiusConstant, this, std::placeholders::_1);
        }
    }

    double strikerIndicatorDistance(const TeamMessageData& msg);
    double strikerIndicatorTimeToBall(const TeamMessageData& msg);
    double strikerIndicatorBnB(const TeamMessageData& msg);
};

#endif // ROLEDECISIONDYNAMIC_H
