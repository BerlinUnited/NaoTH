#ifndef ROLEDECISIONPOSITIONSTATIC_H
#define ROLEDECISIONPOSITIONSTATIC_H

#include <ModuleFramework/Module.h>
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/StringTools.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionPositionStatic)
  PROVIDE(DebugParameterList)
  REQUIRE(FieldInfo)
  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionStatic);


typedef RoleDecisionModel RM;


class RoleDecisionPositionStatic : public RoleDecisionPositionStaticBase
{
public:
    RoleDecisionPositionStatic() : params(getFieldInfo())
    {
        getDebugParameterList().add(&params);
    }

    virtual ~RoleDecisionPositionStatic()
    {
        getDebugParameterList().remove(&params);
    }

    virtual void execute() {
        // set the static default positions
        for (const auto& d : params.defaults) {
            getRoleDecisionModel().roles_position[d.first] = d.second;
        }
    }

private:

    class Parameters: public ParameterList
    {
    public:
        Parameters(const FieldInfo& fi) : ParameterList("RoleDecisionPositionStatic")
        {
            PARAMETER_REGISTER(base_x) = 9000;
            PARAMETER_REGISTER(base_y) = 6000;
            field_x = fi.xLength;
            field_y = fi.yLength;

            // x,y home position; x,y own kickoff position; x,y opponent kickoff position
            PARAMETER_REGISTER(goalie_str)              = "-4300,    0; -4300,   0;  -4300,    0";
            PARAMETER_REGISTER(defender_left_str)       = "-2500, 1100; -3000, 1500; -3000, 1300";
            PARAMETER_REGISTER(defender_center_str)     = "-2500,    0; -3000,    0; -3000,    0";
            PARAMETER_REGISTER(defender_right_str)      = "-2500,-1100; -3000,-1500; -3000, -450";
            PARAMETER_REGISTER(midfielder_left_str)     = "    0, 1500; -1500,  900; -2000,  500";
            PARAMETER_REGISTER(midfielder_center_str)   = "    0,    0; -1500,    0; -2000,    0";
            PARAMETER_REGISTER(midfielder_right_str)    = "    0,-1500; -1500, -900; -2000, -500";
            PARAMETER_REGISTER(forward_left_str)        = " 1500, 1500;  -500, 1500; -1000,  750";
            PARAMETER_REGISTER(forward_center_str)      = " 1500,    0;  -500,    0; -1000,  250";
            PARAMETER_REGISTER(forward_right_str)       = " 1500,-1500;  -500,-1500; -1000, -750";

            // load from the file after registering all parameters
            syncWithConfig();

            // after loading config, parse role positions
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

        std::map<RM::StaticRole, RM::RolePosition> defaults;

        void parsePosition(std::string& positions, RM::StaticRole role)
        {
            auto& r = defaults[role];

            std::vector<std::string> parts = StringTools::split(positions, ';');
            ASSERT(parts.size() == 3);

            std::vector<std::string> pos_part;

            pos_part = StringTools::split(parts[0], ',');
            ASSERT(pos_part.size() == 2);

            r.home.x = std::stod(pos_part[0]) / base_x * field_x;
            r.home.y = std::stod(pos_part[1]) / base_y * field_y;

            pos_part = StringTools::split(parts[1], ',');
            ASSERT(pos_part.size() == 2);
            r.own.x = std::stod(pos_part[0]) / base_x * field_x;
            r.own.y = std::stod(pos_part[1]) / base_y * field_y;

            pos_part = StringTools::split(parts[2], ',');
            ASSERT(pos_part.size() == 2);
            r.opp.x = std::stod(pos_part[0]) / base_x * field_x;
            r.opp.y = std::stod(pos_part[1]) / base_y * field_y;
        }

    } params;
};

#endif // ROLEDECISIONPOSITIONSTATIC_H
