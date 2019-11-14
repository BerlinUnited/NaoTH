#include "RoleDecisionDynamic.h"
#include "Tools/Math/Line.h"

RoleDecisionDynamic::RoleDecisionDynamic()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:striker_ball_difference_radius",
                           "Draws the radius around a seen ball, which is used to determine if the seen ball is the same",
                           false);

    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:goalie_striker_defender_drawings",
                           "Draws two overlapping circles where the intersection represents a valid defending striker.",
                           false);

    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:striker",
                           "Sets the striker decision to true for this robot",
                           false);
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
            double indicator = (this->*params.strikerIndicatorFn)(msg);

            checkStriker(msg, indicator, globalBall, new_striker);
        }
    }//end for

    if(goalie != nullptr) {
        //goalieStrikerDecision(goalie, new_striker);
        Vector2d globalBall = goalie->pose * goalie->ballPosition;

        if((this->*params.strikerGoalieDecisionFn)(goalie, new_striker)) {
            checkStriker(*goalie, 0.0, globalBall, new_striker, true);
        }
    }

    // set the striker decision to the model
    for(const auto& s : new_striker) {
        roles[s.playerNumber] = Roles::striker;
    }

    DEBUG_REQUEST("RoleDecision:Dynamic:striker",
        roles[getPlayerInfo().playerNumber] = Roles::striker;
    );
}

bool RoleDecisionDynamic::goalieStrikerDecisionWants(const TeamMessageData* goalie, std::vector<Striker>& /*striker*/)
{
    // NOTE: this only works if the goalie actually sets the flag!
    return goalie->custom.wantsToBeStriker;
}

bool RoleDecisionDynamic::goalieStrikerDecisionCondition(const TeamMessageData* goalie, std::vector<Striker>& striker)
{
    Vector2d globalBall = goalie->pose * goalie->ballPosition;

    if(getTeamMessagePlayersState().getActiveCount() <= 1) {
        // goalie is alone on the field
        return true;
    }
    else if(globalBall.x <= getFieldInfo().xPosOwnPenaltyArea && fabs(globalBall.y) <= getFieldInfo().yPosLeftPenaltyArea) {
        // ball is inside penalty area, goalie is definitely getting striker
        return true;
    } else if(globalBall.x < params.goalie_striker_min_x_pos && goalie->ballPosition.abs2() <= (params.goalie_striker_ball_distance*params.goalie_striker_ball_distance)) {
        // ball is in the goalie's area
        bool strikerAlreadyDefending = false;
        double r = (this->*params.strikerBallDifferenceFn)(goalie->ballPosition.abs());
        // check if a striker is already defending our goal
        for(auto& s : striker) {
            // do we defend the same ball and is striker between behind the ball?
            if(checkSameBall(s, globalBall, r)  // same ball?
                    && getTeamMessage().data.at(s.playerNumber).pose.translation.x < globalBall.x // behind the ball
                    && (defendingGoalDirectLine(globalBall, getTeamMessage().data.at(s.playerNumber).pose.translation) // between ball and goal
                        || defendingGoalCircle(globalBall, getTeamMessage().data.at(s.playerNumber).pose.translation)) // near the ball
                    )
            {
                strikerAlreadyDefending = true;
                break;
            }
        }
        // nobody already defendin => the goalie has to clear the ball
        if(!strikerAlreadyDefending)
        {
            // set goalie as striker and make sure nobody is "better"
            return true;
        }
    }


    DEBUG_REQUEST("RoleDecision:Dynamic:goalie_striker_defender_drawings",
      FIELD_DRAWING_CONTEXT;

      Vector2d anchor(getFieldInfo().xPosOwnGroundline, 0);

      PEN("000000", 10);
      CIRCLE(getFieldInfo().xPosOwnGroundline, 0, (globalBall - anchor).abs());

      PEN("ff9d00", 10);
      CIRCLE(globalBall.x, globalBall.y, params.goalie_striker_decision_distance);
    );

    return false;
}

bool RoleDecisionDynamic::checkSameBall(const Striker& s,const Vector2d& ball, double radius)
{
    // use the larger radius for 'same ball' comparison
    double sameBallRadius = radius > s.sameBallRadius ? radius : s.sameBallRadius;
    return (s.ball - ball).abs2() < sameBallRadius*sameBallRadius;
}

void RoleDecisionDynamic::checkStriker(const TeamMessageData& msg,
                                       const double& indicator,
                                       const Vector2d& ball,
                                       std::vector<Striker>& striker,
                                       bool force)
{
    bool done = false;
    double r = (this->*params.strikerBallDifferenceFn)(msg.ballPosition.abs());

    DEBUG_REQUEST("RoleDecision:Dynamic:striker_ball_difference_radius",
      FIELD_DRAWING_CONTEXT;
      PEN("000000", 30);
      CIRCLE(ball.x, ball.y, r);
      //TEXT_DRAWING(firstBall.x, firstBall.y+60, secondNumber);
      //TEXT_DRAWING(firstBall.x, firstBall.y-60, r);
    );

    for(auto& s : striker) {
        // same ball as the current striker
        if(checkSameBall(s, ball, r)) {
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

double RoleDecisionDynamic::strikerIndicatorTimeToBall(const TeamMessageData& msg)
{
    const Vector2d ball = msg.pose * msg.ballPosition;
    const Vector2d goal(getFieldInfo().xPosOpponentGroundline, 0);
    const Pose2D anchor(msg.pose.rotation + msg.ballPosition.angle(), ball.x, ball.y);

    Vector2d target = anchor / goal;

    double strikerBonus = getRoleDecisionModel().isStriker(msg.playerNumber) ? params.striker_indicator_bonus : 0.0;

    // NOTE: its an estimation! with sidekicks the turning times can be ignored?!
    return (msg.ballPosition.abs() / params.step_speed)             // time to ball
            + (fabs(msg.ballPosition.angle()) / params.turn_speed)  // time to turn to ball
            + (fabs(target.angle()) / params.turn_speed)            // time to turn to goal (positioning for a kick)
            - strikerBonus;                                         // current striker gets a bonus!
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


bool RoleDecisionDynamic::defendingGoalDirectLine(const Vector2d& ball, const Vector2d& player_pos) const
{
    Math::Line playerBallLine(ball, (player_pos - ball));
    Math::LineSegment goalLine(getFieldInfo().ownGoalPostRight, getFieldInfo().ownGoalPostLeft);
    return goalLine.intersect(playerBallLine);
}

bool RoleDecisionDynamic::defendingGoalCircle(const Vector2d& ball, const Vector2d& player_pos) const
{
    Vector2d anchor(getFieldInfo().xPosOwnGroundline, 0);
    return (player_pos - anchor).abs2() < (ball - anchor).abs2() // is player in the defender circle
            && (player_pos - ball).abs2() < (params.goalie_striker_decision_distance*params.goalie_striker_decision_distance); // is the player near the ball
}
