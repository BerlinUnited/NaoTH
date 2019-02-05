#ifndef ROLEDECISIONASSIGNMENTDISTANCE_H
#define ROLEDECISIONASSIGNMENTDISTANCE_H

#include <ModuleFramework/Module.h>

#include "Tools/StringTools.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageStatistics.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionAssignmentDistance)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(GameData)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamMessage)
  REQUIRE(TeamMessageStatistics)
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
            PARAMETER_REGISTER(assignment, &Parameters::parseAssignment) = "1:goalie;2:defender_left;3:forward_center;4:defender_right;5:midfielder_right;6:midfielder_left";
            PARAMETER_REGISTER(active, &Parameters::parseActive) = "all";
            PARAMETER_REGISTER(variant, &Parameters::setVariantFunction) = "distance"; // "distance", "priority"
            PARAMETER_REGISTER(changing, &Parameters::setChangingFunction) = "time"; // "cycle", "time"

            PARAMETER_REGISTER(minChangingCycles) = 30;
            PARAMETER_REGISTER(minChangingTime) = 1.5; // upper bound, when to change

            // load from the file after registering all parameters
            syncWithConfig();
        }

        int minChangingCycles;
        double minChangingTime;

        std::string assignment;
        std::map<unsigned int, RM::StaticRole> assignment_role;

        std::string active;
        std::vector<RM::StaticRole> active_roles;

        std::string variant;
        void (RoleDecisionAssignmentDistance::*variantFunc)(std::map<unsigned int, RM::StaticRole>&);

        std::string changing;
        void (RoleDecisionAssignmentDistance::*changingFunc)(std::map<unsigned int, RM::StaticRole>&);
    private:
        void parseAssignment(std::string assign) {
            std::vector<std::string> parts = StringTools::split(assign, ';');
            for(const std::string& part : parts) {
                std::vector<std::string> assign_part = StringTools::split(part, ':');
                ASSERT(assign_part.size() == 2);
                assignment_role[static_cast<unsigned int>(std::stoul(assign_part[0]))] = RoleDecisionModel::getStaticRole(assign_part[1]);
            }
        }

        void parseActive(std::string active) {
            if(active.compare("all") == 0) {
                // user all static roles, but ignore the "unknown" one
                for(int i = 1; i < RM::numOfStaticRoles; ++i)
                {
                    active_roles.push_back(static_cast<RM::StaticRole>(i));
                }
            } else {
                // only use the defined roles
                std::vector<std::string> parts = StringTools::split(active, ';');
                for(const std::string& part : parts) {
                    RM::StaticRole r = RM::getStaticRole(part);
                    ASSERT(RoleDecisionModel::unknown != r);
                    active_roles.push_back(r);
                }
            }
        }

        void setVariantFunction(std::string variant) {
            if(variant.compare("distance") == 0) {
                variantFunc = &RoleDecisionAssignmentDistance::withDistance;
            } else { // priority
                variantFunc = &RoleDecisionAssignmentDistance::withPriority;
            }
        }

        void setChangingFunction(std::string changing) {
            if(changing.compare("cycle") == 0) {
                changingFunc = &RoleDecisionAssignmentDistance::roleChangeByCycle;
            } else { // time
                changingFunc = &RoleDecisionAssignmentDistance::roleChangeByTime;
            }
        }
    } params;

    std::map<unsigned int, std::pair<double, RM::StaticRole>> role_changes;

    void roleChange(unsigned int playernumber, RM::StaticRole role);
    void keepGoalie(std::map<unsigned int, RM::StaticRole>& new_roles);
    double getMinChangingTime(unsigned int player);

    void withPriority(std::map<unsigned int, RM::StaticRole>& new_roles);
    void withDistance(std::map<unsigned int, RM::StaticRole>& new_roles);

    void roleChangeByCycle(std::map<unsigned int, RM::StaticRole>& new_roles);
    void roleChangeByTime(std::map<unsigned int, RM::StaticRole>& new_roles);
};

#endif // ROLEDECISIONASSIGNMENTDISTANCE_H
