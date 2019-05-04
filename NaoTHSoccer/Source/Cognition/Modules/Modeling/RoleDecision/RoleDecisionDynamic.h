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
            PARAMETER_REGISTER(striker_ball_difference_function, &Parameters::ballDifferenceRadiusChanger) = "constlinear"; // "const", "linear"
            PARAMETER_REGISTER(striker_ball_difference_distance) = 500.0;
            PARAMETER_REGISTER(striker_ball_difference_distance_m) = 0.35;
            PARAMETER_REGISTER(striker_ball_difference_distance_n) = -40.0;
            PARAMETER_REGISTER(goalie_striker_ball_distance) = 1500.0;
            PARAMETER_REGISTER(goalie_striker_min_x_pos) = -2700.0;
            PARAMETER_REGISTER(goalie_striker_decision_distance) = 500.0;
        }

        double striker_ball_lost_time;
        double striker_ball_bonus_time;
        double striker_indicator_bonus;

        std::string striker_ball_difference_function;
        double (RoleDecisionDynamic::*ballDiffFunc)(double);

        double striker_ball_difference_distance;
        double striker_ball_difference_distance_m;
        double striker_ball_difference_distance_n;

        double goalie_striker_ball_distance;
        double goalie_striker_min_x_pos;
        double goalie_striker_decision_distance;


    private:
        void ballDifferenceRadiusChanger(std::string method) {
            if(method.compare("const") == 0) { // const
                ballDiffFunc = &RoleDecisionDynamic::ballDifferenceRadiusConstant;
            } else if(method.compare("linear") == 0) { // linear
                ballDiffFunc = &RoleDecisionDynamic::ballDifferenceRadiusLinear;
            } else if(method.compare("constlinear") == 0) { // constlinear
                ballDiffFunc = &RoleDecisionDynamic::ballDifferenceRadiusConstantLinear;
            } else { // const
                ballDiffFunc = &RoleDecisionDynamic::ballDifferenceRadiusConstant;
            }
        }
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
    inline bool checkSameBall(const Striker &s, const Vector2d& ball, double r);

    /* Different radius methods for the same ball check. */
    inline double ballDifferenceRadiusConstant(double /*d*/) { return params.striker_ball_difference_distance; }
    inline double ballDifferenceRadiusLinear(double d) { return d * params.striker_ball_difference_distance_m + params.striker_ball_difference_distance_n; }
    inline double ballDifferenceRadiusConstantLinear(double d) { return std::max(ballDifferenceRadiusConstant(d), ballDifferenceRadiusLinear(d)); }

    /* Various evaluation functions who is faster to the ball for the striker decision. */
    double strikerIndicatorDistance(const TeamMessageData& msg);
    double strikerIndicatorTimeToBall(const TeamMessageData& msg);

    /* Various evaluation functions, if another striker is already defending the goal. */
    bool defendingGoalDirectLine(const Vector2d& ball, const Vector2d& player_pos) const;
    bool defendingGoalCircle(const Vector2d& ball, const Vector2d& player_pos) const;
};

#endif // ROLEDECISIONDYNAMIC_H
