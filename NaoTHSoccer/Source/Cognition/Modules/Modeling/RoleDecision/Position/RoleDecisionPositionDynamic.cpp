#include "RoleDecisionPositionDynamic.h"
#include "Tools/Math/Common.h"

RoleDecisionPositionDynamic::RoleDecisionPositionDynamic()
{
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:goalie_defensive_ellipse", "draws the defensive line (ellipse), on which the goalie position itself, if the ball is in the own half", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:supporter_position", "draws the supporter position", false);

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

void RoleDecisionPositionDynamic::supporter()
{
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