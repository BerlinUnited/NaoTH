#ifndef ROLEDECISIONASSIGNMENTDISTANCE_H
#define ROLEDECISIONASSIGNMENTDISTANCE_H

#include <ModuleFramework/Module.h>

#include "Tools/StringTools.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionAssignmentDistance)
  PROVIDE(DebugParameterList)

  REQUIRE(GameData)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamMessage)
  REQUIRE(TeamMessagePlayersState)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionAssignmentDistance);


typedef RoleDecisionModel RM;


class RoleDecisionAssignmentDistance : public RoleDecisionAssignmentDistanceBase
{
public:
    RoleDecisionAssignmentDistance()
    {
        getDebugParameterList().add(&params);
    }

    virtual ~RoleDecisionAssignmentDistance()
    {
        getDebugParameterList().remove(&params);
    }

    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionAssignmentDistance")
        {
            PARAMETER_REGISTER(assignment) = "1:goalie;2:defender_left;3:forward_center;4:defender_right;5:midfielder_left;6:midfielder_right";
            PARAMETER_REGISTER(active_priority) = "forward_right;defender_left;defender_right;midfielder_center;forward_left";
            PARAMETER_REGISTER(minChangingCycles) = 30;
            PARAMETER_REGISTER(minChangingTime) = 1.0;
            // load from the file after registering all parameters
            syncWithConfig();
            // after loading config, parse it
            parseAssignment();
        }
        virtual ~Parameters() {}

        int minChangingCycles;
        double minChangingTime;

        std::string assignment;
        std::map<unsigned int, RoleDecisionModel::StaticRole> assignment_role;

        std::string active_priority;
        std::vector<RM::StaticRole> priority_role;
    private:
        void parseAssignment() {
            std::vector<std::string> parts = split(assignment, ';');
            for(const std::string& part : parts) {
                std::vector<std::string> assign_part = split(part, ':');
                ASSERT(assign_part.size() == 2);
                assignment_role[static_cast<unsigned int>(std::stoul(assign_part[0]))] = RoleDecisionModel::getStaticRole(assign_part[1]);
            }
        }

        void parsePriority() {
            std::vector<std::string> parts = split(active_priority, ';');
            for(const std::string& part : parts) {
                RM::StaticRole r = RM::getStaticRole(part);
                ASSERT(RoleDecisionModel::unknown != r);
                priority_role.push_back(r);
            }
        }

        std::vector<std::string> split(const std::string& strToSplit, char delimeter)
        {
            std::stringstream ss(strToSplit);
            std::string item;
            std::vector<std::string> splittedStrings;
            while (std::getline(ss, item, delimeter))
            {
               splittedStrings.push_back(item);
            }
            return splittedStrings;
        }
    } params;

    std::map<unsigned int, std::pair<double, RM::StaticRole>> role_changes;

    void roleAssignmentBySmallestDistance(std::map<unsigned int, RM::StaticRole>& new_roles);
    void roleAssignmentBySmallestTeamDistance(std::map<unsigned int, RM::StaticRole>& new_roles);

    inline void roleChange(unsigned int playernumber, RM::StaticRole role);
    inline void keepGoalie(std::map<unsigned int, RM::StaticRole>& new_roles);

    void roleChangeByCycle(std::map<unsigned int, RM::StaticRole>& new_roles);
    void roleChangeByTime(std::map<unsigned int, RM::StaticRole>& new_roles);
};

#endif // ROLEDECISIONASSIGNMENTDISTANCE_H
