#include "RoleDecisionDynamic.h"

RoleDecisionDynamic::RoleDecisionDynamic()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:striker_ball_difference_radius",
                           "Draws the radius around a seen ball, which is used to determine if the seen ball is the same)",
                           false);

    ballDifferenceRadiusChanger(params.striker_ball_difference_function);
}

RoleDecisionDynamic::~RoleDecisionDynamic()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionDynamic::execute()
{
    std::map<unsigned int, RM::DynamicRole> new_roles;

    decideStriker(new_roles);
    decideGoalieSupporter(new_roles);
    decideSupporter(new_roles);

    // apply the new role decisions to the model
    for(auto& r : getRoleDecisionModel().roles) {
        const auto& it = new_roles.find(r.first);
        if(it != new_roles.cend()) {
            r.second.dynamic = it->second;
        } else {
            r.second.dynamic = RM::none;
        }
    }
}

void RoleDecisionDynamic::decideStriker(std::map<unsigned int, RM::DynamicRole>& roles)
{
    /*
     * - only active
     * - only playerr who saw/see the ball
     * - the closest player
     * - give a bonus to the current striker
     * - if the goalie wants to be striker, he should become striekr!
     */

    std::vector<Striker> new_striker;

    // iterate over all robots(messages)
    for (const auto& i : getTeamMessage().data) {
        unsigned int playerNumber = i.first;
        const TeamMessageData& msg = i.second;

        // inactive/fallen/penalized robot
        if(!getTeamMessagePlayersState().isPlaying(playerNumber)) { continue; }

        // ball not seen
        if(msg.ballAge < 0) { continue; }

        double ballAge = msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime());
        double timeBonus = getRoleDecisionModel().roles[playerNumber].dynamic == RM::striker ? params.striker_ball_bonus_time : 0.0;

        // TODO: instead of using 'hard-coded' values, base this on the average receiving time!
        // last time ball seen is too big (striker gets an additional bonus)
        if(ballAge > (params.striker_ball_lost_time + timeBonus)) { continue; }

        Vector2d globalBall = msg.pose * msg.ballPosition;
        double indicator = strikerIndicatorDistance(msg);

        // goalie needs to be handled specially
        if(getRoleDecisionModel().roles[playerNumber].role == RM::goalie) {
            // if the ball is near the goal, the goalie has to clear the ball
            if(globalBall.x < params.striker_goalie_min_x_pos && msg.ballPosition.abs() <= params.striker_goalie_ball_distance) {
                // set goalie as striker
                checkStriker(msg, indicator, globalBall, new_striker, true);
            }
        } else {
            checkStriker(msg, indicator, globalBall, new_striker);
        }
    }//end for

    // set the striker decision to the model
    for(const auto& s : new_striker) {
        roles[s.playerNumber] = RM::striker;
    }
}

void RoleDecisionDynamic::checkStriker(const TeamMessageData& msg,
                                       const double& indicator,
                                       const Vector2d& ball,
                                       std::vector<Striker>& striker,
                                       bool force)
{
    bool done = false;
    double r = ballDifferenceRadius(msg.ballPosition.abs());

    DEBUG_REQUEST("RoleDecision:Dynamic:striker_ball_difference_radius",
      FIELD_DRAWING_CONTEXT;
      PEN("000000", 30);
      CIRCLE(ball.x, ball.y, r);
      //TEXT_DRAWING(firstBall.x, firstBall.y+60, secondNumber);
      //TEXT_DRAWING(firstBall.x, firstBall.y-60, r);
    );

    for(auto& s : striker) {
        // use the larger radius for 'same ball' comparison
        double sameBallRadius = r > s.sameBallRadius ? r : s.sameBallRadius;
        // same ball as the current striker
        if((s.ball - ball).abs2() < sameBallRadius*sameBallRadius) {
            // this player is 'faster' to the ball than the current striker or 'forced' to be striker
            if(force || s.indicator > indicator) {
                // set this player for the ball as striker
                s.playerNumber = msg.playerNumber;
                s.indicator = indicator;
                s.ball = ball;
            }
            // don't look at the other stirker - they have different balls
            done = true;
            break;
        }
    }
    // add this player as striker for the ball
    if(!done) {
        striker.push_back({msg.playerNumber, indicator, ball, r});
    }
}


double RoleDecisionDynamic::strikerIndicatorDistance(const TeamMessageData& msg)
{
    double strikerBonus = getRoleDecisionModel().roles[msg.playerNumber].dynamic == RM::striker ?
                params.striker_indicator_bonus : 0.0;
    return msg.ballPosition.abs() - strikerBonus;
}

double RoleDecisionDynamic::strikerIndicatorTimeToBall(const TeamMessageData& msg) {
    // TODO!
    return msg.custom.timeToBall;
}

void RoleDecisionDynamic::decideGoalieSupporter(std::map<unsigned int, RM::DynamicRole>& roles)
{
    /*
     * - if goalie is inactive, someone should become "goalie"
     * - someone close to the own goal should be goalie
     * - until, the goalie comes back and is near the home position
     */
    // TODO!

    // iterate over all robots(messages)
    for (const auto& i : getTeamMessage().data) {
        if(roles.find(i.first) == roles.cend()) {
            roles[i.first] = RM::none;
        }
    }
}

void RoleDecisionDynamic::decideSupporter(std::map<unsigned int, RM::DynamicRole>& roles)
{
    /*
     * - the supporter helps the striker
     * - eg. to receive a pass or as backup, if the goalie lost the ball or a duael
     * - should be someone, who also sees the ball and is in a good position to help ...
     */
    // TODO!

    // iterate over all robots(messages)
    for (const auto& i : getTeamMessage().data) {
        if(roles.find(i.first) == roles.cend()) {
            roles[i.first] = RM::none;
        }
    }
}
