#include "RoleDecisionDynamic.h"
#include "Tools/Math/Line.h"

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
    std::map<unsigned int, Roles::Dynamic> new_roles;

    decideStriker(new_roles);
    decideGoalieSupporter(new_roles);
    decideSupporter(new_roles);

    // apply the new role decisions to the model
    for(auto& r : getRoleDecisionModel().roles) {
        const auto& it = new_roles.find(r.first);
        if(it != new_roles.cend()) {
            r.second.dynamic = it->second;
        } else {
            r.second.dynamic = Roles::none;
        }
    }
}

void RoleDecisionDynamic::decideStriker(std::map<unsigned int, Roles::Dynamic>& roles)
{
    /*
     * - only active
     * - only playerr who saw/see the ball
     * - the closest player
     * - give a bonus to the current striker
     * - if the goalie wants to be striker, he should become striekr!
     */

    std::vector<Striker> new_striker;
    const TeamMessageData* goalie = nullptr;

    // iterate over all robots(messages)
    for (const auto& i : getTeamMessage().data) {
        unsigned int playerNumber = i.first;
        const TeamMessageData& msg = i.second;

        // inactive/fallen/penalized robot
        if(!getTeamMessagePlayersState().isPlaying(playerNumber)) { continue; }

        // ball not seen
        if(msg.ballAge < 0) { continue; }

        double ballAge = msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime());
        double timeBonus = getRoleDecisionModel().isStriker(playerNumber) ? params.striker_ball_bonus_time : 0.0;

        // TODO: instead of using 'hard-coded' values, base this on the average receiving time!
        // last time ball seen is too big (striker gets an additional bonus)
        if(ballAge > (params.striker_ball_lost_time + timeBonus)) { continue; }

        if(getRoleDecisionModel().roles[playerNumber].role == Roles::goalie) {
            // goalie needs to be handled specially later
            goalie = &msg;
        } else {
            Vector2d globalBall = msg.pose * msg.ballPosition;
            double indicator = strikerIndicatorBnB(msg);//strikerIndicatorDistance(msg);

            checkStriker(msg, indicator, globalBall, new_striker);
        }
    }//end for

    if(goalie != nullptr) {
        handleGoalie(goalie, new_striker);
    }

    // set the striker decision to the model
    for(const auto& s : new_striker) {
        roles[s.playerNumber] = Roles::striker;
    }
}

void RoleDecisionDynamic::handleGoalie(const TeamMessageData* goalie, std::vector<Striker>& striker)
{
    double indicator = 0.0;
    Vector2d globalBall = goalie->pose * goalie->ballPosition;
    // ball is inside penalty area, goalie is definitely getting striker
    if(globalBall.x <= getFieldInfo().xPosOwnPenaltyArea && fabs(globalBall.y) <= getFieldInfo().yPosLeftPenaltyArea) {
        checkStriker(*goalie, indicator, globalBall, striker, true);
    } else {
        bool strikerAlreadyDefending = false;
        // check if a striker is already defending our goal (striker is between goal and ball)
        for(auto& s : striker) {
            if(getTeamMessage().data.at(s.playerNumber).pose.translation.x < globalBall.x) {
                Math::Line playerBallLine(globalBall, (getTeamMessage().data.at(s.playerNumber).pose.translation - globalBall));
                Math::LineSegment goalLine(getFieldInfo().ownGoalPostRight, getFieldInfo().ownGoalPostLeft);
                if(goalLine.intersect(playerBallLine)) {
                    strikerAlreadyDefending = true;
                    break;
                }
            }
        }
        // if goalie is alone on the field or the ball is near the goal and nobody already defendin => the goalie has to clear the ball
        if(getTeamMessagePlayersState().getActiveCount() <= 1
           || (!strikerAlreadyDefending && globalBall.x < params.striker_goalie_min_x_pos && goalie->ballPosition.abs() <= params.striker_goalie_ball_distance))
        {
            // set goalie as striker and make sure nobody is "better"
            checkStriker(*goalie, indicator, globalBall, striker, true);
        }
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
    double strikerBonus = getRoleDecisionModel().isStriker(msg.playerNumber) ? params.striker_indicator_bonus : 0.0;
    return msg.ballPosition.abs2() - strikerBonus;
}

double RoleDecisionDynamic::strikerIndicatorTimeToBall(const TeamMessageData& msg) {
    // TODO!
    return msg.custom.timeToBall;
}

double RoleDecisionDynamic::strikerIndicatorBnB(const TeamMessageData& msg)
{
    // TODO: copy from WalkParameters configuration
    // estimate time to ball for dortmund guys
    const double stepTime = params.bnb_step_time; //ms
    const double speed = params.bnb_step_distance / stepTime; // mm/ms
    const double turnSpeed = Math::fromDegrees(params.bnb_turn_speed) / stepTime;

    const Vector2d ball = msg.pose * msg.ballPosition;
    const Vector2d goal(getFieldInfo().xPosOpponentGroundline, 0);
    const Pose2D anchor(msg.pose.rotation + msg.ballPosition.angle(), ball.x, ball.y);

    Vector2d t = anchor / goal;

    //double strikerBonus = getRoleDecisionModel().isStriker(msg.playerNumber) ? params.striker_indicator_bonus : 0.0;

    return (msg.ballPosition.abs() / speed)
            + (fabs(msg.ballPosition.angle()) / turnSpeed)
            + (fabs(t.angle()) / turnSpeed);
}

void RoleDecisionDynamic::decideGoalieSupporter(std::map<unsigned int, Roles::Dynamic>& roles)
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
            roles[i.first] = Roles::none;
        }
    }
}

void RoleDecisionDynamic::decideSupporter(std::map<unsigned int, Roles::Dynamic>& roles)
{
    /*
     * - the supporter helps the striker
     * - eg. to receive a pass or as backup, if the goalie lost the ball or a duael
     * - should be someone, who also sees the ball and is in a good position to help ...
     */
    // TODO!
    std::vector<Striker> new_supporter;

    // iterate over all robots(messages)
    for (const auto& i : getTeamMessage().data) {
        if(roles.find(i.first) == roles.cend()) {
            unsigned int playerNumber = i.first;
            const TeamMessageData& msg = i.second;

            // inactive/fallen/penalized robot
            if(!getTeamMessagePlayersState().isPlaying(playerNumber)) { continue; }

            // ball not seen
            if(msg.ballAge < 0) { continue; }

            double ballAge = msg.ballAge + getFrameInfo().getTimeSince(msg.frameInfo.getTime());

            // TODO: instead of using 'hard-coded' values, base this on the average receiving time!
            // last time ball seen is too big (striker gets an additional bonus)
            if(ballAge > params.striker_ball_lost_time) { continue; }

            // goalie doesn't support, all others do!
            if(getRoleDecisionModel().roles[playerNumber].role != Roles::goalie) {
                //Vector2d globalBall = msg.pose * msg.ballPosition;
                //double indicator = msg.ballPosition.abs();

                //checkStriker(msg, indicator, globalBall, new_striker);
            }
        }
    }

    // set the supporter decision to the model
    for(const auto& s : new_supporter) {
        roles[s.playerNumber] = Roles::supporter;
    }
}
