#ifndef ROLEDECISIONPOSITIONSTATIC_H
#define ROLEDECISIONPOSITIONSTATIC_H

#include <ModuleFramework/Module.h>
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/StringTools.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/Roles.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionPositionStatic)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)

  REQUIRE(FieldInfo)
  REQUIRE(Roles)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionStatic);


class RoleDecisionPositionStatic : public RoleDecisionPositionStaticBase
{
public:
    RoleDecisionPositionStatic()
    {
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:static:draw_active_positions", "draws the ACTIVE role positions on the field as simple cross and a circle around it", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:static:draw_active_positions_robots", "draws the active role positions on the field as robot with role name", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:static:draw_inactive_positions", "draws the INACTIVE role positions on the field as simple cross", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:static:draw_inactive_positions_robots", "draws the INACTIVE role positions on the field as robot with role name", false);
    }

    virtual void execute() {
        // set the static default positions
        for (const auto& d : getRoles().defaults) {
            getRoleDecisionModel().roles_position[d.first] = d.second;
        }

        debugDrawings();
    }

    void debugDrawings() const
    {
        DEBUG_REQUEST("RoleDecision:Position:static:draw_active_positions",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoleDecisionModel().roles_position) {
                if(getRoles().isRoleActive(r.first)) {
                    PEN("666666", 20);
                    LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                    LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
                    PEN("666666", 10);
                    CIRCLE(r.second.home.x, r.second.home.y, 40);
                }
            }
        );

        DEBUG_REQUEST("RoleDecision:Position:static:draw_active_positions_robots",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoleDecisionModel().roles_position) {
                if(getRoles().isRoleActive(r.first)) {
                    // nicer representation (eg. for images)
                    PEN("0000ff", 20);
                    ROBOT(r.second.home.x, r.second.home.y, 0);
                    TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );

        DEBUG_REQUEST("RoleDecision:Position:static:draw_inactive_positions",
            FIELD_DRAWING_CONTEXT;
            PEN("666666", 20);
            for(const auto& r : getRoleDecisionModel().roles_position) {
                if(!getRoles().isRoleActive(r.first) && r.first != Roles::unknown) {
                    LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                    LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
                }
            }
        );

        DEBUG_REQUEST("RoleDecision:Position:static:draw_inactive_positions_robots",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoleDecisionModel().roles_position) {
                if(!getRoles().isRoleActive(r.first) && r.first != Roles::unknown) {
                    // nicer representation (eg. for images)
                    PEN("0000ff", 20);
                    ROBOT(r.second.home.x, r.second.home.y, 0);
                    TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );
    }
};

#endif // ROLEDECISIONPOSITIONSTATIC_H
