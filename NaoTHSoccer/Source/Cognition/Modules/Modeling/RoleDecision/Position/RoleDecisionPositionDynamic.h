#ifndef ROLEDECISIONPOSITIONDYNAMIC_H
#define ROLEDECISIONPOSITIONDYNAMIC_H

#include <ModuleFramework/Module.h>
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/StringTools.h"

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/RoleDecisionModel.h"
#include "Representations/Modeling/TeamBallModel.h"


BEGIN_DECLARE_MODULE(RoleDecisionPositionDynamic)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
  REQUIRE(FieldInfo)
  REQUIRE(TeamBallModel)
  PROVIDE(RoleDecisionModel)
END_DECLARE_MODULE(RoleDecisionPositionDynamic);


typedef RoleDecisionModel RM;


class RoleDecisionPositionDynamic : public RoleDecisionPositionDynamicBase
{
public:
    RoleDecisionPositionDynamic();
    virtual ~RoleDecisionPositionDynamic();
    virtual void execute();

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters(const FieldInfo& fi) : ParameterList("RoleDecisionPositionDynamic")
        {
            PARAMETER_REGISTER(goalie_defense_min_x) = 0;

            PARAMETER_REGISTER(base_x) = 9000;
            PARAMETER_REGISTER(base_y) = 6000;
            field_x = fi.xLength;
            field_y = fi.yLength;

            // x,y home position; x,y own kickoff position; x,y opponent kickoff position
            PARAMETER_REGISTER(goalie_str,           &Parameters::parsePositionGoalie)          = "-4300,    0; -4300,   0;  -4300,    0";
            PARAMETER_REGISTER(defender_left_str,    &Parameters::parsePositionDefenderLeft)    = "-2500, 1100; -3000, 1500; -3000, 1300";
            PARAMETER_REGISTER(defender_center_str,  &Parameters::parsePositionDefenderCenter)  = "-2500,    0; -3000,    0; -3000,    0";
            PARAMETER_REGISTER(defender_right_str,   &Parameters::parsePositionDefenderRight)   = "-2500,-1100; -3000,-1500; -3000, -450";
            PARAMETER_REGISTER(midfielder_left_str,  &Parameters::parsePositionMidfielderLeft)  = "    0, 1500; -1500,  900; -2000,  500";
            PARAMETER_REGISTER(midfielder_center_str,&Parameters::parsePositionMidfielderCenter)= "    0,    0; -1500,    0; -2000,    0";
            PARAMETER_REGISTER(midfielder_right_str, &Parameters::parsePositionMidfielderRight) = "    0,-1500; -1500, -900; -2000, -500";
            PARAMETER_REGISTER(forward_left_str,     &Parameters::parsePositionForwardLeft)     = " 1500, 1500;  -500, 1500; -1000,  750";
            PARAMETER_REGISTER(forward_center_str,   &Parameters::parsePositionForwardCenter)   = " 1500,    0;  -500,    0; -1000,  250";
            PARAMETER_REGISTER(forward_right_str,    &Parameters::parsePositionForwardRight)    = " 1500,-1500;  -500,-1500; -1000, -750";

            // load from the file after registering all parameters
            syncWithConfig();
        }

        double goalie_defense_min_x;

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

        void parsePositionGoalie(std::string pos)           { parsePosition(pos, RM::goalie); }
        void parsePositionDefenderLeft(std::string pos)     { parsePosition(pos, RM::defender_left); }
        void parsePositionDefenderCenter(std::string pos)   { parsePosition(pos, RM::defender_center); }
        void parsePositionDefenderRight(std::string pos)    { parsePosition(pos, RM::defender_right); }
        void parsePositionMidfielderLeft(std::string pos)   { parsePosition(pos, RM::midfielder_left); }
        void parsePositionMidfielderCenter(std::string pos) { parsePosition(pos, RM::midfielder_center); }
        void parsePositionMidfielderRight(std::string pos)  { parsePosition(pos, RM::midfielder_right); }
        void parsePositionForwardLeft(std::string pos)      { parsePosition(pos, RM::forward_left); }
        void parsePositionForwardCenter(std::string pos)    { parsePosition(pos, RM::forward_center); }
        void parsePositionForwardRight(std::string pos)     { parsePosition(pos, RM::forward_right); }
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

    void calculateGoalieDefensivePosition();
};

#endif // ROLEDECISIONPOSITIONDYNAMIC_H
