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


class RoleDecision : public RoleDecisionBase
{
public:
    RoleDecision() { instance = this; }
    virtual ~RoleDecision() {}

    virtual void execute();

private:
    // HACK: for the parameters to get access to the FieldInfo.
    static RoleDecision* instance;

    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("RoleDecision")
        {
            PARAMETER_REGISTER(assignment) = "1:goalie;2:defender_left;3:forward_center;4:defender_right;5:midfielder_left;6:midfielder_right";
            PARAMETER_REGISTER(active_priority) = "forward_right;defender_left;defender_right;midfielder_center;forward_left";

            PARAMETER_REGISTER(base_x) = 9000;
            PARAMETER_REGISTER(base_y) = 6000;

            PARAMETER_REGISTER(goalie_str) = "-4500,0;-4500,0;-4500,0";
            PARAMETER_REGISTER(defender_left_str) = "-2500,1500;-2500,1500;-2500,1500";
            PARAMETER_REGISTER(defender_center_str) = "-2500,0;-2500,0;-2500,0";
            PARAMETER_REGISTER(defender_right_str) = "-2500,-1500;-2500,-1500;-2500,-1500";
            PARAMETER_REGISTER(midfielder_left_str) = "-2500,1500;-2500,1500;-2500,1500";
            PARAMETER_REGISTER(midfielder_center_str) = "0,0;0,0;0,0";
            PARAMETER_REGISTER(midfielder_right_str) = "0,-1500;0,-1500;0,-1500";
            PARAMETER_REGISTER(forward_left_str) = "1500,1500;1500,1500;1500,1500";
            PARAMETER_REGISTER(forward_center_str) = "1500,0;1500,0;1500,0";
            PARAMETER_REGISTER(forward_right_str) = "1500,-1500;1500,-1500;1500,-1500";
            // load from the file after registering all parameters
            syncWithConfig();
            // after loading config, parse it
            parseAssignment();
            parsePriority();
            // parse role positions
            parsePosition(goalie_str, RoleDecisionModel::goalie);
            parsePosition(defender_left_str, RoleDecisionModel::defender_left);
            parsePosition(defender_center_str, RoleDecisionModel::defender_center);
            parsePosition(defender_right_str, RoleDecisionModel::defender_right);
            parsePosition(midfielder_left_str, RoleDecisionModel::midfielder_left);
            parsePosition(midfielder_center_str, RoleDecisionModel::midfielder_center);
            parsePosition(midfielder_right_str, RoleDecisionModel::midfielder_right);
            parsePosition(forward_left_str, RoleDecisionModel::forward_left);
            parsePosition(forward_center_str, RoleDecisionModel::forward_center);
            parsePosition(forward_right_str, RoleDecisionModel::forward_right);
        }
        virtual ~Parameters() {}

        std::string assignment;
        std::map<unsigned int, RoleDecisionModel::StaticRole> assignment_role;

        std::string active_priority;
        std::vector<RoleDecisionModel::StaticRole> priority_role;

        double base_x;
        double base_y;

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

        std::map<RoleDecisionModel::StaticRole, RoleDecisionModel::Role> default_roles;

    private:
        void parseAssignment() {
            std::vector<std::string> parts = split(assignment, ';');
            for(const std::string& part : parts) {
                std::vector<std::string> assign_part = split(part, ':');
                ASSERT(assign_part.size() == 2);
                assignment_role[static_cast<unsigned int>(std::stoul(assign_part[0]))] = RoleDecisionModel::getStaticRole(assign_part[1]);
            }
        }

        void parsePosition(std::string& positions, RoleDecisionModel::StaticRole role)
        {
            auto& r = default_roles[role];
            r.role = role;

            std::vector<std::string> parts = split(positions, ';');
            ASSERT(parts.size() == 3);

            std::vector<std::string> pos_part;

            pos_part = split(parts[0], ',');
            ASSERT(pos_part.size() == 2);
            r.home.x = std::stod(pos_part[0]) / base_x * instance->getFieldInfo().xLength;
            r.home.y = std::stod(pos_part[1]) / base_y * instance->getFieldInfo().yLength;

            pos_part = split(parts[1], ',');
            ASSERT(pos_part.size() == 2);
            r.own.x = std::stod(pos_part[0]) / base_x * instance->getFieldInfo().xLength;
            r.own.y = std::stod(pos_part[1]) / base_y * instance->getFieldInfo().yLength;

            pos_part = split(parts[2], ',');
            ASSERT(pos_part.size() == 2);
            r.opp.x = std::stod(pos_part[0]) / base_x * instance->getFieldInfo().xLength;
            r.opp.y = std::stod(pos_part[1]) / base_y * instance->getFieldInfo().yLength;
        }

        void parsePriority() {
            std::vector<std::string> parts = split(active_priority, ';');
            for(const std::string& part : parts) {
                RoleDecisionModel::StaticRole r = RoleDecisionModel::getStaticRole(part);
                ASSERT(RoleDecisionModel::unknown != r);
                priority_role.push_back(r);
            }
        }
    } params;

    void simpleAssignment();

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
