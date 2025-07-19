#include "RoleDecisionPositionDynamic.h"
#include "Tools/Math/Common.h"

RoleDecisionPositionDynamic::RoleDecisionPositionDynamic()
{
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:goalie_defensive_ellipse", "draws the defensive line (ellipse), on which the goalie position itself, if the ball is in the own half", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:supporter_position", "draws the supporter position", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:defending_set_play_position", "draws the defending set play position (between ball and own goal)", false);

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
        case PlayerInfo::pushing_free_kick:
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
        case PlayerInfo::kick_in:
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
    if (isDefendingSetPlay()) {
        positionBetweenBallAndGoal();
    }
}

void RoleDecisionPositionDynamic::supporter()
{
    if (isDefendingSetPlay()) {
        positionOrthogonalToBall();
        return;
    }

    // Get the striker's player number and position
    PlayerNumber strikerNumber = getRoleDecisionModel().getPlayerNumber(Roles::striker);
    if(strikerNumber == 0) { return; }

    const auto& strikerPlayer = getTeamState().getPlayer(strikerNumber);
    Vector2d strikerPos = strikerPlayer.pose().translation;

    // Get the supporter's position
    const auto& supporterPlayer = getTeamState().getPlayer(getPlayerInfo().playerNumber);
    Vector2d supporterPosCurrent = supporterPlayer.pose().translation;

    // Direction from own goal to striker
    Vector2d ownGoal(getFieldInfo().xPosOwnGroundline, 0);
    Vector2d strikerToGoal = (strikerPos - ownGoal).normalize();

    // Vector from striker to supporter
    Vector2d strikerToSupporter = supporterPosCurrent - strikerPos;
    double distanceToStriker = strikerToSupporter.abs();

    // Target position: offset behind striker
    double offset = std::max(params.supporter_offset, distanceToStriker);
    Vector2d supporterPos = strikerPos - strikerToGoal * offset;

    // Compute the normal (perpendicular) vector to the defending line
    Vector2d normal(-strikerToGoal.y, strikerToGoal.x); // 90 degree rotation

    // Determine which side the supporter is currently on
    double side = (strikerToGoal.x * strikerToSupporter.y - strikerToGoal.y * strikerToSupporter.x) >= 0 ? 1.0 : -1.0;

    // Offset along the normal (left/right of the line)
    supporterPos += normal * side * params.supporter_side_offset;

    // Set the computed position
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

void RoleDecisionPositionDynamic::positionBetweenBallAndGoal()
{
    Vector2d globalBall = getRobotPose() * getBallModel().position; // getTeamBallModel().positionOnField
    Vector2d ownGoal(getFieldInfo().xPosOwnGroundline, 0);
    Vector2d ballToGoal = ownGoal - globalBall;

    // Position the robot at a certain distance from the ball towards the goal
    // This creates a defensive line between the ball and the goal
    Vector2d defensivePosition = globalBall + ballToGoal.normalize() * (getFieldInfo().centerCircleRadius * 1.1);

    // Ensure the position is within the field
    defensivePosition.x = Math::clamp(defensivePosition.x, getFieldInfo().xPosOwnGroundline + 500, getFieldInfo().xPosOpponentGroundline - 500);
    defensivePosition.y = Math::clamp(defensivePosition.y, getFieldInfo().yPosRightSideline + 500, getFieldInfo().yPosLeftSideline - 500);

    // Set the dynamic position
    getRoleDecisionModel().dynamic_position = defensivePosition;

    // Debug drawing
    DEBUG_REQUEST("RoleDecision:Dynamic:defending_set_play_position",
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

void RoleDecisionPositionDynamic::positionOrthogonalToBall()
{
    Vector2d globalBall = getTeamBallModel().positionOnField;
    Vector2d ownGoal(getFieldInfo().xPosOwnGroundline, 0);
    Vector2d ballToGoal = ownGoal - globalBall;

    // Calculate the direction from ball to goal
    Vector2d ballToGoalDir = ballToGoal.normalize();

    // Calculate the orthogonal direction (perpendicular to ball-goal line)
    Vector2d orthogonalDir(-ballToGoalDir.y, ballToGoalDir.x); // 90 degree rotation

    // Position the robot orthogonal to the ball-goal line, at least center circle distance
    double scalingFactor = 1 + (params.setplay_second_defender_scaling - 1) * globalBall.x * 2.0 / getFieldInfo().xFieldLength;
    Vector2d ballGoalLine =       globalBall + ballToGoalDir * (getFieldInfo().centerCircleRadius * std::max(scalingFactor, 2.0));
    Vector2d orthogonalPosition = ballGoalLine + orthogonalDir * params.setplay_second_defender_offset;

    // Ensure the position is within the field
    orthogonalPosition.x = Math::clamp(orthogonalPosition.x, getFieldInfo().xPosOwnGroundline + 500, getFieldInfo().xPosOpponentGroundline - 500);
    orthogonalPosition.y = Math::clamp(orthogonalPosition.y, getFieldInfo().yPosRightSideline + 500, getFieldInfo().yPosLeftSideline - 500);

    // Set the dynamic position
    getRoleDecisionModel().dynamic_position = orthogonalPosition;

    // Debug drawing
    DEBUG_REQUEST("RoleDecision:Dynamic:defending_set_play_position",
        FIELD_DRAWING_CONTEXT;
        PEN("ff00ff", 20);
        CIRCLE(globalBall.x, globalBall.y, 100);
        PEN("00ff00", 20);
        CIRCLE(orthogonalPosition.x, orthogonalPosition.y, 100);
        PEN("ffff00", 10);
        LINE(globalBall.x, globalBall.y, ownGoal.x, ownGoal.y);
        PEN("00ffff", 10);
        LINE(orthogonalPosition.x, orthogonalPosition.y, globalBall.x, globalBall.y);
        PEN("0000ff", 10);
        CIRCLE(globalBall.x, globalBall.y, getFieldInfo().centerCircleRadius);
        PEN("0f0f0f", 10);
        LINE(ballGoalLine.x, ballGoalLine.y, globalBall.x, globalBall.y);
        PEN("f0f0f0", 10);
        LINE(orthogonalPosition.x, orthogonalPosition.y, ballGoalLine.x, ballGoalLine.y);
    );
}