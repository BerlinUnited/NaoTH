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
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:static:draw_positions_static", "draw role positions on the field", false);
    }

    virtual void execute() {
        // set the static default positions
        for (const auto& d : getRoles().defaults) {
            getRoleDecisionModel().roles_position[d.first] = d.second;
        }

        DEBUG_REQUEST("RoleDecision:Position:static:draw_positions_static",
            FIELD_DRAWING_CONTEXT;
            PEN("666666", 20);
            for(const auto& r : getRoleDecisionModel().roles_position) {
                LINE(r.second.home.x, r.second.home.y-20, r.second.home.x, r.second.home.y+20);
                LINE(r.second.home.x-20, r.second.home.y, r.second.home.x+20, r.second.home.y);
            }
        );
    }
};

#endif // ROLEDECISIONPOSITIONSTATIC_H
