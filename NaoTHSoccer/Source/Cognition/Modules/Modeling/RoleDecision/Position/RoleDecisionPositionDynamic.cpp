#include "RoleDecisionPositionDynamic.h"
#include "Tools/Math/Common.h"

RoleDecisionPositionDynamic::RoleDecisionPositionDynamic()
{
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:goalie_defensive_ellipse", "draws the defensive line (ellipse), on which the goalie position itself, if the ball is in the own half", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:supporter_position", "draws the supporter position", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:striker_set_play_position", "draws the defending set play position of the striker", false);

    getDebugParameterList().add(&params);
}

RoleDecisionPositionDynamic::~RoleDecisionPositionDynamic()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionDynamic::execute()
{
    const auto& role = getRoleDecisionModel().getRole(getPlayerInfo().playerNumber);

    // handle the static role goalie separately
    if(role.role == Roles::goalie) {
        goalie();
        return;
    }

    // only update the position for myself
    switch (role.dynamic)
    {
        case Roles::striker:
            striker();
            break;
        case Roles::supporter:
            supporter();
            break;
        case Roles::goalie_supporter:
            // goalieSupporter();
            break;
        default:
            // use the static position as default
            getRoleDecisionModel().dynamic_position = getRoleDecisionModel().getStaticRolePosition(role.role).home;
            break;
    }
}

bool RoleDecisionPositionDynamic::isDefendingSetPlay()
{
    // it is our kickoff, we don't defend
    if (getPlayerInfo().kickoff) {
        return false;
    }

    switch (getPlayerInfo().robotSetPlay)
    {
        case PlayerInfo::set_none:
            return false;
        case PlayerInfo::goal_kick:
        {
            // if we don't know where the ball is, we defend -- since we don't know if it's ours
            if (!getTeamBallModel().valid)
            {
                return true;
            }

            // the ball is in the opponent half (opponent goal kick)
            if (getTeamBallModel().positionOnField.x > 0)
            {
                return true;
            }

            return false;
        }
        case PlayerInfo::direct_free_kick:
            // always defend, since we don't know if it's ours
            return true;
        case PlayerInfo::corner_kick:
        {
            // if we don't know where the ball is, we defend -- since we don't know if it's ours
            if (!getTeamBallModel().valid)
            {
                return true;
            }

            // the ball is in our half (opponent corner kick)
            if (getTeamBallModel().positionOnField.x < 0)
            {
                return true;
            }

            return false;
        }
        case PlayerInfo::indirect_free_kick:
            // always defend, since we don't know if it's ours
            return true;
        case PlayerInfo::penalty_kick:
            return true;
        default:
            return false;
    }
}

void RoleDecisionPositionDynamic::goalie()
{
    // prevent "oscillation" when the ball is near the defense line
    // go forward, when ball is behind min x and go back if the ball is in front of max x
    double defense_x = params.goalie_last_active ? params.goalie_defense_max_x : params.goalie_defense_min_x;
    // we're using the teamball, so it has to be valid
    if(getTeamBallModel().valid && getTeamBallModel().positionOnField.x <= defense_x) {
        // Calculates the defensive position of the goalie
        // The position is on an ellipse within the penalty area.
        // The position is calculated in such a way, that a direct shot to the middle of the goal is prevented
        auto p = calculateEllipsePoint({getTeamBallModel().positionOnField.x - getFieldInfo().xPosOwnGroundline, getTeamBallModel().positionOnField.y});
        getRoleDecisionModel().dynamic_position.x = p.x + getFieldInfo().xPosOwnGroundline;
        getRoleDecisionModel().dynamic_position.y = p.y;

        params.goalie_last_active = true;
    } else {
        params.goalie_last_active = false;
        // set position to default
        getRoleDecisionModel().dynamic_position = getRoleDecisionModel().getStaticRolePosition(Roles::goalie).home;
    }

    DEBUG_REQUEST("RoleDecision:Dynamic:goalie_defensive_ellipse",
        FIELD_DRAWING_CONTEXT;
        OVAL(getFieldInfo().xPosOwnGroundline, 0, params.goalie_max_come_out, getFieldInfo().goalWidth/2);
    );
}

Vector2d RoleDecisionPositionDynamic::calculateEllipsePoint(const Vector2d& ball)
{
    Vector2d point;
    // ball is NOT behind our ground line
    if(ball.x > 0) {
        // direct line from goal center to the ball: f(x) = mx
        double m = ball.y / ball.x;
        double a = params.goalie_max_come_out;
        double b = getFieldInfo().goalWidth / 2.0;
        // position on the ellipse: x = \frac{ab}{\sqrt{b^2 + m^2 a^2}}
        point.x = (a*b) / std::sqrt(b*b + m*m * a*a);
        // y = m*x
        point.y = m*point.x;
        // prevent walking against goal post
        point.x = Math::clamp(point.x, 100.0, getFieldInfo().xPosOwnPenaltyArea - getFieldInfo().xPosOwnGroundline);
        point.y = Math::clamp(point.y, getFieldInfo().yPosRightGoalpost + 100, getFieldInfo().yPosLeftGoalpost - 100);
    }
    return point;
}

void RoleDecisionPositionDynamic::striker()
{
    if (isDefendingSetPlay())
    {
        Vector2d globalBall = getRobotPose() * getBallModel().last_known_ball; // getTeamBallModel().positionOnField
        Vector2d ownGoal(getFieldInfo().xPosOwnGroundline, 0);
        Vector2d robotPos = getRobotPose().translation;

        // Check if the robot is already behind the ball relative to the ball-goal line
        Vector2d ballToGoal = ownGoal - globalBall;
        Vector2d ballToGoalDir = ballToGoal.normalize();
        Vector2d robotToBall = globalBall - robotPos;

        // Calculate the projection of robot-to-ball vector onto the ball-to-goal direction
        // If this projection is positive, the robot is behind the ball
        double behindTheBall = robotToBall * ballToGoalDir;

        Vector2d defensivePosition;
        if (behindTheBall > params.striker_avoidence_threshold)
        {
            // Robot is behind the ball, position to the side to avoid overrunning
            // Calculate the orthogonal direction (perpendicular to ball-goal line)
            Vector2d orthogonalDir(-ballToGoalDir.y, ballToGoalDir.x);

            // Choose side based on robot's current position relative to ball-goal line
            // Use the cross product to determine which side the robot is on
            double side = ballToGoalDir.x * robotToBall.y - ballToGoalDir.y * robotToBall.x;

            // Position the robot to the side of the ball, maintaining defensive distance
            defensivePosition = globalBall + orthogonalDir * (getFieldInfo().centerCircleRadius * 1.1 * ((side >= 0) ? -1.0 : 1.0));
        } else {
            // Robot is in front of the ball, can safely move between ball and goal
            defensivePosition = globalBall + ballToGoalDir * (getFieldInfo().centerCircleRadius * 1.1);
        }

        // Ensure the position is within the field
        defensivePosition.x = Math::clamp(defensivePosition.x, getFieldInfo().xPosOwnGroundline + 500, getFieldInfo().xPosOpponentGroundline - 500);
        defensivePosition.y = Math::clamp(defensivePosition.y, getFieldInfo().yPosRightSideline + 500, getFieldInfo().yPosLeftSideline - 500);

        // Set the dynamic position
        getRoleDecisionModel().dynamic_position = defensivePosition;

        // Debug drawing
        DEBUG_REQUEST("RoleDecision:Dynamic:striker_set_play_position",
            FIELD_DRAWING_CONTEXT;
            PEN("ff00ff", 20);
            CIRCLE(globalBall.x, globalBall.y, 100);
            PEN("00ff00", 20);
            CIRCLE(defensivePosition.x, defensivePosition.y, 100);
            PEN("ffff00", 10);
            LINE(globalBall.x, globalBall.y, ownGoal.x, ownGoal.y);
            PEN("00ffff", 10);
            LINE(defensivePosition.x, defensivePosition.y, globalBall.x, globalBall.y);
            PEN("0000ff", 10);
            CIRCLE(globalBall.x, globalBall.y, getFieldInfo().centerCircleRadius);
        );
    }
}

void RoleDecisionPositionDynamic::supporter()
{
    // Get the striker's player number and position
    PlayerNumber strikerNumber = getRoleDecisionModel().getPlayerNumber(Roles::striker);
    if(strikerNumber == 0) { return; }

    const auto& strikerPlayer = getTeamState().getPlayer(strikerNumber);
    Vector2d strikerPos = strikerPlayer.pose().translation;
    Vector2d ownGoal(getFieldInfo().xPosOwnGroundline, 0);
    Vector2d strikerToGoal = ownGoal - strikerPos;

    // Calculate the direction from ball to goal
    Vector2d strikerToGoalDir = strikerToGoal.normalize();

    // Calculate the orthogonal direction (perpendicular to ball-goal line)
    Vector2d orthogonalDir(-strikerToGoalDir.y, strikerToGoalDir.x); // 90 degree rotation

    // Position the robot orthogonal to the ball-goal line, at least center circle distance
    double scalingFactor = 1 + (params.supporter_offense_scaling - 1) * strikerPos.x * 2.0 / getFieldInfo().xFieldLength;
    Vector2d supporterBasePos = strikerPos + strikerToGoalDir * (params.supporter_offset * std::max(scalingFactor, 1.0));

    double fieldSideY = strikerPos.y >= 0 ? 1.0 : -1.0;
    Vector2d supporterPos = supporterBasePos + orthogonalDir * params.supporter_offset_side * fieldSideY;

    // Ensure the position is within the field
    supporterPos.x = Math::clamp(supporterPos.x, getFieldInfo().xPosOwnGroundline + 500, getFieldInfo().xPosOpponentGroundline - 500);
    supporterPos.y = Math::clamp(supporterPos.y, getFieldInfo().yPosRightSideline + 500, getFieldInfo().yPosLeftSideline - 500);

    // Set the dynamic position
    getRoleDecisionModel().dynamic_position = supporterPos;

    // Debug drawing
    DEBUG_REQUEST("RoleDecision:Dynamic:supporter_position",
        FIELD_DRAWING_CONTEXT;
        PEN("00ff00", 20);
        CIRCLE(supporterPos.x, supporterPos.y, 100);
        PEN("0000ff", 20);
        CIRCLE(strikerPos.x, strikerPos.y, 100);
        PEN("0000aa", 20);
        CIRCLE(strikerPos.x, strikerPos.y, params.supporter_offset);
        PEN("ff0000", 20);
        CIRCLE(ownGoal.x, ownGoal.y, 100);
        LINE(ownGoal.x, ownGoal.y, strikerPos.x, strikerPos.y);
        LINE(supporterPos.x, supporterPos.y, strikerPos.x, strikerPos.y);
    );
}
