#ifndef ROLEDECISIONASSIGNMENTSTATIC_H
#define ROLEDECISIONASSIGNMENTSTATIC_H

#include <ModuleFramework/Module.h>

#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/StringTools.h"

#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionAssignmentStatic)
  REQUIRE(TeamState)
  PROVIDE(DebugParameterList)
  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionAssignmentStatic);


class RoleDecisionAssignmentStatic : public RoleDecisionAssignmentStaticBase
{
public:
    RoleDecisionAssignmentStatic()
    {
        getDebugParameterList().add(&params);
    }

    virtual ~RoleDecisionAssignmentStatic()
    {
        getDebugParameterList().remove(&params);
    }

    virtual void execute() {
        for (const auto& i: getTeamState().players) {
            const auto& r = params.assignment_role.find(i.first);
            if(r == params.assignment_role.cend()) {
                getRoleDecisionModel().roles[i.first].role = Roles::unknown;
            } else {
                getRoleDecisionModel().roles[i.first].role = r->second;
            }
        }
    }

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("RoleDecisionAssignmentStatic")
        {
            // the used assignments, they don't have to be the active roles
            PARAMETER_REGISTER(assignment, &Parameters::parseAssignment) = "1:goalie;2:defender_left;3:midfielder_center;4:midfielder_left;5:defender_right;6:midfielder_right;7:forward_center";
            // load from the file after registering all parameters
            syncWithConfig();
        }

        std::string assignment;
        std::map<unsigned int, Roles::Static> assignment_role;

    private:
        void parseAssignment(std::string assignment) {
            std::vector<std::string> parts = StringTools::split(assignment, ';');
            for(const std::string& part : parts) {
                std::vector<std::string> assign_part = StringTools::split(part, ':');
                ASSERT(assign_part.size() == 2);
                assignment_role[static_cast<unsigned int>(std::stoul(assign_part[0]))] = Roles::getStaticRole(assign_part[1]);
            }
        }
    } params;
};

#endif // ROLEDECISIONASSIGNMENTSTATIC_H
