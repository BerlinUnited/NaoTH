#include "RoleDecisionPositionFormation.h"
#include "Eigen/Eigen"
#include <iomanip>
#include <limits>

RoleDecisionPositionFormation::RoleDecisionPositionFormation()
{
    getDebugParameterList().add(&params);

    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:reset_positions", "", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_default_position", "", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_active_positions", "", false);
    DEBUG_REQUEST_REGISTER("RoleDecision:Position:formation:draw_inactive_positions", "", false);

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

        //Vector2d anchor(getFieldInfo().xPosOwnGroundline, 0);
        const auto& anchor = getRoles().defaults.at(Roles::goalie).home;
        const auto angle = (getTeamBallModel().positionOnField - Vector2d(getFieldInfo().xPosOwnGroundline, 0)).angle();

        for(const auto& r : getRoles().defaults) {
            if(r.first == Roles::goalie) { continue; }

            Vector2d shift;
            switch (r.first / 3) {
                case 0: // defender
                    shift.x = (r.second.home.x - anchor.x) * Math::clamp(std::sqrt(2.2*factor), params.defenderMinFactorX, params.defenderMaxFactorX);
                    shift.y = (r.second.home.y - anchor.y) * Math::clamp(1.2-1.8*std::exp(-4.2*factor), params.defenderMinFactorY, params.defenderMaxFactorY);
                    shift.rotate(Math::clamp(angle, params.defenderMinAngle, params.defenderMaxAngle));
                    shift += anchor;
                    break;
                case 1: // midfielder
                    //0.3+\frac{1}{1+e^{-\left(x-0.5\right)d}}\cdot0.99
                    //0.4+\frac{1}{1+e^{-xd}}\cdot0.99
                    shift.x = (r.second.home.x - anchor.x) * Math::clamp(0.3+(1/(1+std::exp(-3.2*(factor-0.5))))*1.0, params.midfielderMinFactorX, params.midfielderMaxFactorX);
                    shift.y = (r.second.home.y - anchor.y) * Math::clamp(1.2-1.8*std::exp(-4.2*factor), params.midfielderMinFactorY, params.midfielderMaxFactorY);
                    shift.rotate(Math::clamp(angle, params.midfielderMinAngle, params.midfielderMaxAngle));
                    shift += anchor;
                    break;
                case 2: // forward
                    //0.3+\frac{1}{1+e^{-\left(x-0.5\right)d}}\cdot1.1
                    shift.x = (r.second.home.x - anchor.x) * Math::clamp(0.3+(1/(1+std::exp(-2.4*(factor-0.5))))*1.1, params.forwardMinFactorX, params.forwardMaxFactorX);
                    shift.y = (r.second.home.y - anchor.y) * Math::clamp(1.2-1.8*std::exp(-4.2*factor), params.forwardMinFactorY, params.forwardMaxFactorY);
                    shift.rotate(Math::clamp(angle, params.forwardMinAngle, params.forwardMaxAngle));
                    shift += anchor;
                    break;
                default:
                    shift = r.second.home;
            }
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

    DEBUG_REQUEST("RoleDecision:Position:formation:draw_active_positions",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(getRoles().isRoleActive(r.first)) {
                // more functional presentation
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
                // mark active positions with a circle
                PEN("666666", 10);
                CIRCLE(r.second.home.x, r.second.home.y, 40);

                // nicer representation (eg. for images)
                //PEN("0000ff", 20);
                //ROBOT(r.second.home.x, r.second.home.y, 0);
                //TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
            }
        }
    );

    DEBUG_REQUEST("RoleDecision:Position:formation:draw_inactive_positions",
        FIELD_DRAWING_CONTEXT;
        for(const auto& r : getRoleDecisionModel().roles_position) {
            if(!getRoles().isRoleActive(r.first)) {
                // more functional presentation
                PEN("666666", 20);
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);

                // nicer representation (eg. for images)
                //PEN("0000ff", 20);
                //ROBOT(r.second.home.x, r.second.home.y, 0);
                //TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
            }
        }
    );
}
