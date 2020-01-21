#include "RoleDecisionPositionFormation.h"
#include "Eigen/Eigen"
#include <iomanip>
#include <limits>

RoleDecisionPositionFormation::RoleDecisionPositionFormation()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:reset_positions", "resets the role positions to its defaults.", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_default_position", "draws the default role positions.", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_active_positions", "draws the ACTIVE role positions on the field as simple cross and a circle around it", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_active_positions_robots", "draws the ACTIVE role positions on the field as robot with role name", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_inactive_positions", "draws the INACTIVE role positions on the field as simple cross", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_inactive_positions_robots", "draws the INACTIVE role positions on the field as robot with role name", false);

    // init positions with default
    resetPositions();
}

RoleDecisionPositionFormation::~RoleDecisionPositionFormation()
{
    getDebugParameterList().remove(&params);
}

void RoleDecisionPositionFormation::resetPositions() const
{
    for (const auto& d : getRoles().defaults) {
        getRoleDecisionModel().roles_position[d.first] = d.second;
    }
}

void RoleDecisionPositionFormation::execute()
{
    DEBUG_REQUEST("RoleDecision:Position:formation:reset_positions", resetPositions(); );
    // TODO: make factor function configurable

    if(getTeamBallModel().valid) {
        double factor = Math::clamp((getTeamBallModel().positionOnField.x - getFieldInfo().xPosOwnGroundline) / getFieldInfo().xLength, 0.0, 1.0);

        // an alternative anchor could be simply the center of the own groundline: getFieldInfo().xPosOwnGroundline, 0
        const auto& anchor = getRoles().defaults.at(Roles::goalie).home;
        const auto angle = (getTeamBallModel().positionOnField - Vector2d(getFieldInfo().xPosOwnGroundline, 0)).angle();

        double x, y;

        for(const auto& r : getRoles().defaults) {
            if(r.first == Roles::goalie) { continue; }

            Vector2d shift = r.second.home - anchor;
            // NOTE: the assumption is that the roles are each divided into three groups!
            switch (r.first / 3) {
                case 0: // defender
                    x = std::sqrt(2.2*factor);         // \sqrt{2.2 \cdot \text{factor}}
                    y = 1.2-1.8*std::exp(-4.2*factor); // 1.2 - 1.8 \cdot e^{-4.2 \cdot \text{factor}}

                    shift.x *= Math::clamp(x, params.defenderMinFactorX, params.defenderMaxFactorX);
                    shift.y *= Math::clamp(y, params.defenderMinFactorY, params.defenderMaxFactorY);
                    shift.rotate(Math::clamp(angle, params.defenderMinAngle, params.defenderMaxAngle));
                    break;
                case 1: // midfielder
                    x = 0.3+(1/(1+std::exp(-3.2*(factor-0.5))))*1.0; // 0.3 + \frac{1}{1+e^{-3.2 \cdot \left( \text{factor} - 0.5 \right)}} \cdot 1.0
                    y = 1.2-1.8*std::exp(-4.2*factor);               // 1.2 - 1.8 \cdot e^{-4.2 \cdot \text{factor}}

                    shift.x *= Math::clamp(x, params.midfielderMinFactorX, params.midfielderMaxFactorX);
                    shift.y *= Math::clamp(y, params.midfielderMinFactorY, params.midfielderMaxFactorY);
                    shift.rotate(Math::clamp(angle, params.midfielderMinAngle, params.midfielderMaxAngle));
                    break;
                case 2: // forward
                    x = 0.3+(1/(1+std::exp(-2.4*(factor-0.5))))*1.1; // 0.3 + \frac{1}{1+e^{-2.4 \cdot \left( \text{factor} - 0.5 \right)}} \cdot 1.1
                    y = 1.2-1.8*std::exp(-4.2*factor);               // 1.2 - 1.8 \cdot e^{-4.2 \cdot \text{factor}}

                    shift.x *= Math::clamp(x, params.forwardMinFactorX, params.forwardMaxFactorX);
                    shift.y *= Math::clamp(y, params.forwardMinFactorY, params.forwardMaxFactorY);
                    shift.rotate(Math::clamp(angle, params.forwardMinAngle, params.forwardMaxAngle));
                    break;
                default:
                    // unknown role, do nothing
                    break;
            }
            shift += anchor;
            getRoleDecisionModel().roles_position[r.first].home = shift;
        }
    }

    debugDrawings();
}

void RoleDecisionPositionFormation::debugDrawings() const
{
    DEBUG_REQUEST("RoleDecision:Position:formation:draw_default_position",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoles().defaults) {
            if(getRoles().isRoleActive(r.first)) {
                PEN("969300", 10);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
            }
        }
    );

    // more functional presentation
    DEBUG_REQUEST("RoleDecision:Position:formation:draw_active_positions",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(getRoles().isRoleActive(r.first)) {
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
                // mark active positions with a circle
                PEN("666666", 10);
                CIRCLE(r.second.home.x, r.second.home.y, 40);
            }
        }
    );

    // nicer representation (eg. for images)
    DEBUG_REQUEST("RoleDecision:Position:formation:draw_active_positions_robots",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(getRoles().isRoleActive(r.first)) {
                PEN("0000ff", 20);
                ROBOT(r.second.home.x, r.second.home.y, 0);
                TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
            }
        }
    );

    // more functional presentation
    DEBUG_REQUEST("RoleDecision:Position:formation:draw_inactive_positions",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(!getRoles().isRoleActive(r.first) && r.first != Roles::unknown) {
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
            }
        }
    );

    // nicer representation (eg. for images)
    DEBUG_REQUEST("RoleDecision:Position:formation:draw_inactive_positions_robots",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(!getRoles().isRoleActive(r.first) && r.first != Roles::unknown) {
                PEN("0000ff", 20);
                ROBOT(r.second.home.x, r.second.home.y, 0);
                TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
            }
        }
    );
}
