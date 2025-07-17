#ifndef ROLEDECISIONASSIGNMENTSTATIC_H
#define ROLEDECISIONASSIGNMENTSTATIC_H

#include <ModuleFramework/Module.h>

#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/StringTools.h"

#include "Representations/Modeling/TeamState.h"
#include "Representations/Infrastructure/Roles.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionAssignmentStatic)
  REQUIRE(TeamState)
  REQUIRE(Roles)
  PROVIDE(DebugParameterList)
  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionAssignmentStatic);


class RoleDecisionAssignmentStatic : public RoleDecisionAssignmentStaticBase
{
public:
    RoleDecisionAssignmentStatic()
    {}

    virtual ~RoleDecisionAssignmentStatic()
    {}

    virtual void execute()
    {
        // simply set the role for each player based on the assignments
        for (const auto& i: getTeamState().players)
        {
            const auto& r = getRoles().assignments.find(i.first);
            if(r == getRoles().assignments.cend())
            {
                getRoleDecisionModel().roles[i.first].role = Roles::unknown;
            } else {
                getRoleDecisionModel().roles[i.first].role = r->second;
            }
        }
    }
};

#endif // ROLEDECISIONASSIGNMENTSTATIC_H
