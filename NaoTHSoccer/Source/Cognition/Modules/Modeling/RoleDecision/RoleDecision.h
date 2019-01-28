#ifndef ROLEDECISION_H
#define ROLEDECISION_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/SoccerStrategy.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"


#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"


BEGIN_DECLARE_MODULE(RoleDecision)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(GameData)
  REQUIRE(PlayerInfo)
  REQUIRE(SoccerStrategy)
  REQUIRE(TeamMessage)
  REQUIRE(TeamMessagePlayersState)

  REQUIRE(TeamMessageData)

  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecision);


typedef RoleDecisionModel RM;


class RoleDecision : public RoleDecisionBase
{
public:
    RoleDecision() : params(getFieldInfo())
    {
        getDebugParameterList().add(&params);
    }

    virtual ~RoleDecision()
    {
        getDebugParameterList().remove(&params);
    }

    virtual void execute();

private:

    class Parameters: public ParameterList
    {
    public:
        Parameters(const FieldInfo& fi): ParameterList("RoleDecision")
        {
            PARAMETER_REGISTER(assignment) = "1:goalie;2:defender_left;3:forward_center;4:defender_right;5:midfielder_left;6:midfielder_right";
            PARAMETER_REGISTER(active_priority) = "forward_right;defender_left;defender_right;midfielder_center;forward_left";

            PARAMETER_REGISTER(base_x) = 9000;
            PARAMETER_REGISTER(base_y) = 6000;
            field_x = fi.xLength;
            field_y = fi.yLength;

            // x,y home position; x,y own kickoff position; x,y opponent kickoff position
            PARAMETER_REGISTER(goalie_str)              = "-4300,    0; -4300,   0;  -4300,    0";
            PARAMETER_REGISTER(defender_left_str)       = "-2500, 1100; -3000, 1500; -3000, 1300";
            PARAMETER_REGISTER(defender_center_str)     = "-2500,    0; -3000,    0; -3000,    0";
            PARAMETER_REGISTER(defender_right_str)      = "-2500,-1100; -3000,-1500; -3000,-1100";
            PARAMETER_REGISTER(midfielder_left_str)     = "    0, 1500; -1500,  900; -2000,  500";
            PARAMETER_REGISTER(midfielder_center_str)   = "    0,    0; -1500,    0; -2000,    0";
            PARAMETER_REGISTER(midfielder_right_str)    = "    0,-1500; -1500, -900; -2000, -500";
            PARAMETER_REGISTER(forward_left_str)        = " 1500, 1500;  -500, 1500; -1000,  750";
            PARAMETER_REGISTER(forward_center_str)      = " 1500,    0;  -500,    0; -1000,  250";
            PARAMETER_REGISTER(forward_right_str)       = " 1500,-1500;  -500,-1500; -1000, -750";

            PARAMETER_REGISTER(minChangingCycles) = 30;
            PARAMETER_REGISTER(minChangingTime) = 1.0;
            // load from the file after registering all parameters
            syncWithConfig();
            // after loading config, parse it
            parseAssignment();
            parsePriority();
            // parse role positions
            parsePosition(goalie_str, RM::goalie);
            parsePosition(defender_left_str, RM::defender_left);
            parsePosition(defender_center_str, RM::defender_center);
            parsePosition(defender_right_str, RM::defender_right);
            parsePosition(midfielder_left_str, RM::midfielder_left);
            parsePosition(midfielder_center_str, RM::midfielder_center);
            parsePosition(midfielder_right_str, RM::midfielder_right);
            parsePosition(forward_left_str, RM::forward_left);
            parsePosition(forward_center_str, RM::forward_center);
            parsePosition(forward_right_str, RM::forward_right);
        }
        virtual ~Parameters() {}

        std::string assignment;
        std::map<unsigned int, RM::StaticRole> assignment_role;

        std::string active_priority;
        std::vector<RM::StaticRole> priority_role;

        double base_x;
        double base_y;

        double field_x;
        double field_y;

        std::string goalie_str;
        std::string defender_left_str;
        std::string defender_center_str;
        std::string defender_right_str;
        std::string midfielder_left_str;
        std::string midfielder_center_str;
        std::string midfielder_right_str;
        std::string forward_left_str;
        std::string forward_center_str;
        std::string forward_right_str;

        std::map<RM::StaticRole, RM::Role> default_roles;

        int minChangingCycles;
        double minChangingTime;
    private:
        void parseAssignment() {
            std::vector<std::string> parts = split(assignment, ';');
            for(const std::string& part : parts) {
                std::vector<std::string> assign_part = split(part, ':');
                ASSERT(assign_part.size() == 2);
                assignment_role[static_cast<unsigned int>(std::stoul(assign_part[0]))] = RM::getStaticRole(assign_part[1]);
            }
        }

        void parsePosition(std::string& positions, RM::StaticRole role)
        {
            auto& r = default_roles[role];
            r.role = role;

            std::vector<std::string> parts = split(positions, ';');
            ASSERT(parts.size() == 3);

            std::vector<std::string> pos_part;

            pos_part = split(parts[0], ',');
            ASSERT(pos_part.size() == 2);
            r.home.x = std::stod(pos_part[0]) / base_x * field_x;
            r.home.y = std::stod(pos_part[1]) / base_y * field_y;

            pos_part = split(parts[1], ',');
            ASSERT(pos_part.size() == 2);
            r.own.x = std::stod(pos_part[0]) / base_x * field_x;
            r.own.y = std::stod(pos_part[1]) / base_y * field_y;

            pos_part = split(parts[2], ',');
            ASSERT(pos_part.size() == 2);
            r.opp.x = std::stod(pos_part[0]) / base_x * field_x;
            r.opp.y = std::stod(pos_part[1]) / base_y * field_y;
        }

        void parsePriority() {
            std::vector<std::string> parts = split(active_priority, ';');
            for(const std::string& part : parts) {
                RM::StaticRole r = RM::getStaticRole(part);
                ASSERT(RoleDecisionModel::unknown != r);
                priority_role.push_back(r);
            }
        }
    } params;

    std::map<unsigned int, std::pair<double, RM::StaticRole>> role_changes;

    void roleAssignmentBySmallestDistance(std::map<unsigned int, RM::StaticRole>& new_roles);
    void roleAssignmentBySmallestTeamDistance(std::map<unsigned int, RM::StaticRole>& new_roles);

    inline void roleChange(unsigned int playernumber, RM::StaticRole role);
    inline void keepGoalie(std::map<unsigned int, RM::StaticRole>& new_roles);

    void roleChangeByCycle(std::map<unsigned int, RM::StaticRole>& new_roles);
    void roleChangeByTime(std::map<unsigned int, RM::StaticRole>& new_roles);

    // TODO: put this into commons?!
    static std::vector<std::string> split(std::string strToSplit, char delimeter)
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

};

#endif // ROLEDECISION_H
