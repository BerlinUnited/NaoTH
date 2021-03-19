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
    struct Striker {
        unsigned int playerNumber;
        double indicator;
        Vector2d ball;
        double sameBallRadius;
    };

    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("RoleDecisionDynamic")
        {
            PARAMETER_REGISTER(strikerIndicator, &Parameters::setStrikerIndicatorFunction) = "time"; // "distance"

            PARAMETER_REGISTER(striker_ball_lost_time) = 1000.0;
            PARAMETER_REGISTER(striker_ball_bonus_time) = 4000.0;
            PARAMETER_REGISTER(striker_indicator_bonus) = 300.0;

            PARAMETER_REGISTER(striker_ball_difference, &Parameters::setStrikerBallDifferenceFunction) = "constlinear"; // "const", "linear", "constlinear"
            PARAMETER_REGISTER(striker_ball_difference_distance) = 500.0;
            PARAMETER_REGISTER(striker_ball_difference_distance_m) = 0.35;
            PARAMETER_REGISTER(striker_ball_difference_distance_n) = -40.0;

            PARAMETER_REGISTER(goalie_striker_decision, &Parameters::setStrikerGoalieDecisionFunction) = "condition"; // "wants"
            PARAMETER_REGISTER(goalie_striker_ball_distance) = 1500.0;
            PARAMETER_REGISTER(goalie_striker_min_x_pos) = -2700.0;
            PARAMETER_REGISTER(goalie_striker_decision_distance) = 500.0;

            PARAMETER_REGISTER(step_time, &Parameters::setStepTime) = 250;
            PARAMETER_REGISTER(step_distance, &Parameters::setStepDistance) = 80;
            PARAMETER_REGISTER(turn_angle, &Parameters::setTurnAngle) = 30;

            // load from the file after registering all parameters
            syncWithConfig();
        }

        // striker function, returns an indicator who's fastest
        std::string strikerIndicator;
        double (RoleDecisionDynamic::*strikerIndicatorFn)(const TeamMessageData&);

        double striker_ball_lost_time;
        double striker_ball_bonus_time;
        double striker_indicator_bonus;

        // ball difference function returns, how similiar two balls should be
        std::string striker_ball_difference;
        double (RoleDecisionDynamic::*strikerBallDifferenceFn)(double);

        double striker_ball_difference_distance;
        double striker_ball_difference_distance_m;
        double striker_ball_difference_distance_n;

        // decides, whether the goalie gets striker or not
        std::string goalie_striker_decision;
        bool (RoleDecisionDynamic::*strikerGoalieDecisionFn)(const TeamMessageData*, std::vector<RoleDecisionDynamic::Striker>&);

        double goalie_striker_ball_distance;
        double goalie_striker_min_x_pos;
        double goalie_striker_decision_distance;

        // params for the time-to-ball striker function
        double step_time;
        double step_distance;
        double turn_angle;
        // the calculated speeds
        double step_speed;
        double turn_speed;

    private:
        void setStrikerBallDifferenceFunction(std::string variant) {
            if(variant.compare("const") == 0) { // constant
                strikerBallDifferenceFn = &RoleDecisionDynamic::ballDifferenceRadiusConstant;
            } else if(variant.compare("linear") == 0) { // linear
                strikerBallDifferenceFn = &RoleDecisionDynamic::ballDifferenceRadiusLinear;
            } else if(variant.compare("constlinear") == 0) { // constlinear
                strikerBallDifferenceFn = &RoleDecisionDynamic::ballDifferenceRadiusConstantLinear;
            } else {
                // backup, in case something went wrong
                strikerBallDifferenceFn = &RoleDecisionDynamic::ballDifferenceRadiusConstant;
            }
        }
        void setStrikerIndicatorFunction(std::string variant) {
            if(variant.compare("distance") == 0) { // distance
                strikerIndicatorFn = &RoleDecisionDynamic::strikerIndicatorDistance;
            } else if(variant.compare("time") == 0) { // time
                strikerIndicatorFn = &RoleDecisionDynamic::strikerIndicatorTimeToBall;
            } else {
                // backup, in case something went wrong
                strikerIndicatorFn = &RoleDecisionDynamic::strikerIndicatorDistance;
            }
        }
        void setStrikerGoalieDecisionFunction(std::string variant) {
            if(variant.compare("wants") == 0) { // wants
                strikerGoalieDecisionFn = &RoleDecisionDynamic::goalieStrikerDecisionWants;
            } else if(variant.compare("condition") == 0) { // condition
                strikerGoalieDecisionFn = &RoleDecisionDynamic::goalieStrikerDecisionCondition;
            } else {
                // backup, in case something went wrong
                strikerGoalieDecisionFn = &RoleDecisionDynamic::goalieStrikerDecisionCondition;
            }
        }

        void setStepTime(double t) { setSpeed(t, step_distance, turn_angle); }
        void setStepDistance(double d) { setSpeed(step_time, d, turn_angle); }
        void setTurnAngle(double a) { setSpeed(step_time, step_distance, a); }
        void setSpeed(double t, double d, double a) {
            step_speed = d / t;
            turn_speed = Math::fromDegrees(a) / step_time;
        }
    } params;

    void decideStriker(std::map<unsigned int, Roles::Dynamic>& roles);
    void decideGoalieSupporter(std::map<unsigned int, Roles::Dynamic>& roles);
    void decideSupporter(std::map<unsigned int, Roles::Dynamic>& roles);

    void checkStriker(const TeamMessageData& msg, const double& indicator, const Vector2d& ball, std::vector<Striker>& striker, bool force = false);
    bool checkSameBall(const Striker &s, const Vector2d& ball, double r);

    bool goalieStrikerDecisionWants(const TeamMessageData *goalie, std::vector<Striker>& striker);
    bool goalieStrikerDecisionCondition(const TeamMessageData *goalie, std::vector<Striker>& striker);

    /* Different radius methods for the same ball check. */
    double ballDifferenceRadiusConstant(double /*d*/) { return params.striker_ball_difference_distance; }
    double ballDifferenceRadiusLinear(double d) { return d * params.striker_ball_difference_distance_m + params.striker_ball_difference_distance_n; }
    double ballDifferenceRadiusConstantLinear(double d) { return std::max(ballDifferenceRadiusConstant(d), ballDifferenceRadiusLinear(d)); }

    /* Various evaluation functions who is faster to the ball for the striker decision. */
    double strikerIndicatorDistance(const TeamMessageData& msg);
    double strikerIndicatorTimeToBall(const TeamMessageData& msg);

    /* Various evaluation functions, if another striker is already defending the goal. */
    bool defendingGoalDirectLine(const Vector2d& ball, const Vector2d& player_pos) const;
    bool defendingGoalCircle(const Vector2d& ball, const Vector2d& player_pos) const;
};

#endif // ROLEDECISIONDYNAMIC_H
