#ifndef ROLEDECISIONASSIGNMENTDISTANCE_H
#define ROLEDECISIONASSIGNMENTDISTANCE_H

#include <ModuleFramework/Module.h>

#include "Tools/StringTools.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Roles.h"
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessageStatistics.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionAssignmentDistance)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(GameData)
  REQUIRE(Roles)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamState)
  REQUIRE(TeamMessageStatistics)
  REQUIRE(TeamMessagePlayersState)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionAssignmentDistance);


class RoleDecisionAssignmentDistance : public RoleDecisionAssignmentDistanceBase
{
public:
    RoleDecisionAssignmentDistance();
    virtual ~RoleDecisionAssignmentDistance();
    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("RoleDecisionAssignmentDistance")
        {
            // the initial assignments, they don't have to be the active ones
            PARAMETER_REGISTER(assignment, &Parameters::parseAssignment) = "1:goalie;2:defender_left;3:forward_center;4:defender_right;5:midfielder_right;6:midfielder_left";
            PARAMETER_REGISTER(variant, &Parameters::setVariantFunction) = "priodist"; // "distance", "priority", "priodist"
            PARAMETER_REGISTER(changing, &Parameters::setChangingFunction) = "time"; // "cycle", "time"

            PARAMETER_REGISTER(minChangingCycles) = 30;
            PARAMETER_REGISTER(minChangingTime) = 1.5; // upper bound, when to change

            // load from the file after registering all parameters
            syncWithConfig();
        }

        int minChangingCycles;
        double minChangingTime;

        std::string assignment;
        std::map<unsigned int, Roles::Static> assignment_role;

        std::string variant;
        void (RoleDecisionAssignmentDistance::*variantFunc)(std::map<unsigned int, Roles::Static>&);

        std::string changing;
        void (RoleDecisionAssignmentDistance::*changingFunc)(std::map<unsigned int, Roles::Static>&);
    private:
        /**
         * @brief If the 'assignment' parameter changes, the new value is parsed and the new role
         *        assignments are applied. An assignment is 'playernumber:role' and every assignment
         *        is seperated by a ';'.
         *        Eg.: "1:goalie;2:defender_left"
         *
         * @param assign
         */
        void parseAssignment(std::string assign) {
            // clear the 'old' assignments
            assignment_role.clear();
            std::vector<std::string> parts = StringTools::split(assign, ';');
            for(const std::string& part : parts) {
                std::vector<std::string> assign_part = StringTools::split(part, ':');
                ASSERT(assign_part.size() == 2);
                assignment_role[static_cast<unsigned int>(std::stoul(assign_part[0]))] = Roles::getStaticRole(assign_part[1]);
            }
        }

        /**
         * @brief If the 'variant' parameter changes, the corresponding function pointer is set
         *        to the correct method.
         *        Recognized values: "distance", "priority" (default)
         *
         * @param variant   new parameter value
         */
        void setVariantFunction(std::string variant) {
            if(variant.compare("distance") == 0) { // distance
                variantFunc = &RoleDecisionAssignmentDistance::withDistance;
            } else if(variant.compare("priodist") == 0) { // priodist
                variantFunc = &RoleDecisionAssignmentDistance::withPriorityDistance;
            } else { // priority
                variantFunc = &RoleDecisionAssignmentDistance::withPriority;
            }
        }

        /**
         * @brief If the 'changing' parameter changes, the corresponding function pointer is set
         *        to the correct method.
         *        Recognized values: "time", "cycle" (default)
         *
         * @param changing  new parameter value
         */
        void setChangingFunction(std::string changing) {
            if(changing.compare("time") == 0) { // time
                changingFunc = &RoleDecisionAssignmentDistance::roleChangeByTime;
            } else { // cycle
                changingFunc = &RoleDecisionAssignmentDistance::roleChangeByCycle;
            }
        }
    } params;

    /** Holds the decision of the last cycles and a 'counter' for how long this decision is made. */
    std::map<unsigned int, std::pair<double, Roles::Static>> role_changes;

    /**
     * @brief Changes the role of the RoleDecisionModel of the given player to the given role
     *        and resets the internal role change counter (see role_changes).
     *
     * @param playernumber  the player, who should get the new role
     * @param role          the role the player should get
     */
    void roleChange(unsigned int playernumber, Roles::Static role);

    /**
     * @brief Finds the 'goalie' role in the current role assignment and keeps the role for the
     *        next decision.
     *
     * @param new_roles the role decision for the current cycle
     */
    void keepGoalie(std::map<unsigned int, Roles::Static>& new_roles);

    /**
     * @brief Returns the time in seconds, when a player should take its new role.
     *        Therefore the parameter 'minChangingTime' or the avg. message receiving time
     *        (if available) is used.
     *
     * @param player    the player number for which the avg. receiving time should be used
     * @return          time in fractions of a second
     */
    double getMinChangingTime(unsigned int player);

    /**
     * @brief Determines for each role, which player is closest and sets the role for that
     *        player.
     *
     * @param new_roles the role decision for the current cycle
     */
    void withPriority(std::map<unsigned int, Roles::Static>& new_roles);

    /**
     * @brief Determines the optimal role assignment for the whole team in that the sum
     *        of distance the team has to move is small.
     *
     * @param new_roles the role decision for the current cycle
     */
    void withDistance(std::map<unsigned int, Roles::Static>& new_roles);

    /**
     * @brief Determines the optimal role assignment for the whole team in that the sum
     *        of distance the team has to move is small and also the roles with the highest
     *        priority are assigned.
     *
     * @param new_roles the role decision for the current cycle
     */
    void withPriorityDistance(std::map<unsigned int, Roles::Static>& new_roles);

    /**
     * @brief Finds the optimal role assignments for the given roles and players.
     * @param roles     the roles, which should be assigned
     * @param players   the players, which should be roles assigned
     * @param new_roles the players with the newly assigned roles
     */
    void findOptimalTeamAssignment(const std::vector<Roles::Static> &roles, const std::vector<unsigned int> &players, std::map<unsigned int, Roles::Static>& new_roles);

    /**
     * @brief The role decision for this cycle is only applied, if the role decicion was
     *        was the same for the last 'minChangingCycles' (see parameters).
     *        This should prevent oscillations in the role assignments.
     *
     * @param new_roles the role decision for the current cycle
     */
    void roleChangeByCycle(std::map<unsigned int, Roles::Static>& new_roles);

    /**
     * @brief The role decision for this cycle is only applied, if the role decicion was
     *        was the same for the last 'minChangingTime' seconds (see parameters &
     *        getMinChangingTime()).
     *        This should prevent oscillations in the role assignments.
     *
     * @param new_roles the role decision for the current cycle
     */
    void roleChangeByTime(std::map<unsigned int, Roles::Static>& new_roles);
};

#endif // ROLEDECISIONASSIGNMENTDISTANCE_H
