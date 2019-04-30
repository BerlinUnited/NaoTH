#include "RoleDecisionPositionDynamicGoalie.h"
#include "Tools/Math/Common.h"

RoleDecisionPositionDynamicGoalie::RoleDecisionPositionDynamicGoalie()
{
    DEBUG_REQUEST_REGISTER("RoleDecision:Dynamic:goalie_defensive_ellipse", "draws the defensive line (ellipse), on which the goalie position itself, if the ball is in the own half", false);

    getDebugParameterList().add(&params);
}

RoleDecisionPositionDynamicGoalie::~RoleDecisionPositionDynamicGoalie()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionDynamicGoalie::execute() {
    // prevent "oscillation" when the ball is near the defense line
    // go forward, when ball is behind min x and go back if the ball is in front of max x
    double defense_x = lastActive ? params.goalie_defense_max_x : params.goalie_defense_min_x;
    // we're using the teamball, so it has to be valid
    if(getTeamBallModel().valid && getTeamBallModel().positionOnField.x <= defense_x) {
        // Calculates the defensive position of the goalie
        // The position is on an ellipse within the penalty area.
        // The position is calculated in such a way, that a direct shot to the middle of the goal is prevented
        auto p = calculateEllipsePoint({getTeamBallModel().positionOnField.x - getFieldInfo().xPosOwnGroundline, getTeamBallModel().positionOnField.y});
        getRoleDecisionModel().roles_position[Roles::goalie].home.x = p.x + getFieldInfo().xPosOwnGroundline;
        getRoleDecisionModel().roles_position[Roles::goalie].home.y = p.y;

        lastActive = true;
    } else {
        lastActive = false;
        // set position to default
        getRoleDecisionModel().roles_position[Roles::goalie].home = getRoles().defaults.at(Roles::goalie).home;
    }

    // keep the default kickoff positions
    getRoleDecisionModel().roles_position[Roles::goalie].own = getRoles().defaults.at(Roles::goalie).own;
    getRoleDecisionModel().roles_position[Roles::goalie].opp = getRoles().defaults.at(Roles::goalie).opp;

    DEBUG_REQUEST("RoleDecision:Dynamic:goalie_defensive_ellipse",
        FIELD_DRAWING_CONTEXT;
        OVAL(getFieldInfo().xPosOwnGroundline, 0, params.goalie_max_come_out, getFieldInfo().goalWidth/2);
    );
}

Vector2d RoleDecisionPositionDynamicGoalie::calculateEllipsePoint(const Vector2d& ball)
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
