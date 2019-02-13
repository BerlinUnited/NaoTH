#include "RoleDecisionPositionDynamic.h"
#include "Tools/Math/Common.h"

RoleDecisionPositionDynamic::RoleDecisionPositionDynamic()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:draw_positions_dynamic", "draw role positions on the field", false);
}

RoleDecisionPositionDynamic::~RoleDecisionPositionDynamic()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionDynamic::execute() {
    // set the static default positions
    for (const auto& d : getRoles().defaults) {
        if(d.first == Roles::goalie) {
            calculateGoalieDefensivePosition();
        } else {
            getRoleDecisionModel().roles_position[d.first] = d.second;
        }
    }

    DEBUG_REQUEST("RoleDecision:Position:draw_positions_dynamic",
        FIELD_DRAWING_CONTEXT;
        PEN("666666", 20);
        for(const auto& r : getRoleDecisionModel().roles_position) {
            LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
            LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
        }
    );
}

void RoleDecisionPositionDynamic::calculateGoalieDefensivePosition()
{
    double x = 100.0;
    double y = 0.0;
    // we're using the teamball, so it has to be valid
    if(getTeamBallModel().valid && getTeamBallModel().positionOnField.x <= params.goalie_defense_min_x) {
        // Calculates the defensive position of the goalie
        // The position is on an ellipse within the penalty area.
        // The position is calculated in such a way, that a direct shot to the middle of the goal is prevented
        const Vector2d c(getTeamBallModel().positionOnField.x - getFieldInfo().xPosOwnGroundline, getTeamBallModel().positionOnField.y);
        // teamball is valid and ball is NOT behind our ground line
        if(c.x > 0) {
            // direct line from goal center to the ball: f(x) = mx
            double m = c.y / c.x;
            double a = getFieldInfo().xPosOwnPenaltyArea - getFieldInfo().xPosOwnGroundline;
            double b = getFieldInfo().goalWidth / 2.0;
            // position on the ellipse: x = \frac{ab}{\sqrt{b^2 + m^2 a^2}}
            x = (a*b) / std::sqrt(b*b + m*m * a*a);
            // y = m*x
            y = m*x;
            // prevent walking against goal post
            x = Math::clamp(x, 100.0, getFieldInfo().xPosOwnPenaltyArea - getFieldInfo().xPosOwnGroundline);
            y = Math::clamp(y, getFieldInfo().yPosRightGoalpost + 100, getFieldInfo().yPosLeftGoalpost - 100);
        }
    }

    // apply the position to the model
    getRoleDecisionModel().roles_position[Roles::goalie].home.x = x + getFieldInfo().xPosOwnGroundline;
    getRoleDecisionModel().roles_position[Roles::goalie].home.y = y;
    // keep the default kickoff positions
    getRoleDecisionModel().roles_position[Roles::goalie].own = getRoles().defaults.at(Roles::goalie).own;
    getRoleDecisionModel().roles_position[Roles::goalie].opp = getRoles().defaults.at(Roles::goalie).opp;

}
