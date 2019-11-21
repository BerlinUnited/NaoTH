#ifndef ROLESPROVIDER_H
#define ROLESPROVIDER_H

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/Roles.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(RolesProvider)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)

  REQUIRE(FieldInfo)

  PROVIDE(Roles)
END_DECLARE_MODULE(RolesProvider);


class RolesProvider : public RolesProviderBase
{
public:
    RolesProvider() : params(this)
    {
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:defaults:draw_active_home_positions", "draws the ACTIVE home role positions on the field as robot with role name", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:defaults:draw_inactive_home_positions", "draws the INACTIVE home role positions on the field as robot with role name", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:defaults:draw_active_own_kickoff_positions", "draws the ACTIVE own kickoff role positions on the field as robot with role name", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:defaults:draw_inactive_own_kickoff_positions", "draws the INACTIVE own kickoff role positions on the field as robot with role name", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:defaults:draw_active_opp_kickoff_positions", "draws the ACTIVE opp kickoff role positions on the field as robot with role name", false);
        DEBUG_REQUEST_REGISTER("RoleDecision:Position:defaults:draw_inactive_opp_kickoff_positions", "draws the INACTIVE opp kickoff role positions on the field as robot with role name", false);

        getDebugParameterList().add(&params);
    }

    virtual ~RolesProvider()
    {
        getDebugParameterList().remove(&params);
    }

    virtual void execute()
    {
        debugDrawings();
    }

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters(RolesProvider *p): ParameterList("Roles")
        {
            provider = p;

            PARAMETER_REGISTER(base_x) = 9000;
            PARAMETER_REGISTER(base_y) = 6000;

            // x,y home position; x,y own kickoff position; x,y opponent kickoff position
            PARAMETER_REGISTER(goalie_str,           &Parameters::parsePositionGoalie)          = "-4300,    0; -4300,    0; -4300,    0";
            PARAMETER_REGISTER(defender_left_str,    &Parameters::parsePositionDefenderLeft)    = "-2500, 1100; -3000,  800; -3000, 1300";
            PARAMETER_REGISTER(defender_center_str,  &Parameters::parsePositionDefenderCenter)  = "-2500,    0; -3000,    0; -3000,    0";
            PARAMETER_REGISTER(defender_right_str,   &Parameters::parsePositionDefenderRight)   = "-2500,-1100; -3000, -800; -3000, -450";
            PARAMETER_REGISTER(midfielder_left_str,  &Parameters::parsePositionMidfielderLeft)  = " -750, 1500; -1500,  900; -2000,  500";
            PARAMETER_REGISTER(midfielder_center_str,&Parameters::parsePositionMidfielderCenter)= " -750,    0; -1500,  300; -2000,  300";
            PARAMETER_REGISTER(midfielder_right_str, &Parameters::parsePositionMidfielderRight) = " -750,-1500; -1500, -900; -2000, -500";
            PARAMETER_REGISTER(forward_left_str,     &Parameters::parsePositionForwardLeft)     = " 1500, 1500;  -500, 1500; -1000,  750";
            PARAMETER_REGISTER(forward_center_str,   &Parameters::parsePositionForwardCenter)   = " 1500,    0;  -500,    0; -1100,    0";
            PARAMETER_REGISTER(forward_right_str,    &Parameters::parsePositionForwardRight)    = " 1500,-1500;  -500,-1500; -1000, -750";

            PARAMETER_REGISTER(active_str, &Parameters::parseActive) = "goalie;defender_left;forward_center;defender_right;midfielder_center"; //"all";

            // load from the file after registering all parameters
            syncWithConfig();
        }

        RolesProvider* provider;

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

        std::string active_str;

        void parsePositionGoalie(std::string pos)           { parsePosition(pos, Roles::goalie); }
        void parsePositionDefenderLeft(std::string pos)     { parsePosition(pos, Roles::defender_left); }
        void parsePositionDefenderCenter(std::string pos)   { parsePosition(pos, Roles::defender_center); }
        void parsePositionDefenderRight(std::string pos)    { parsePosition(pos, Roles::defender_right); }
        void parsePositionMidfielderLeft(std::string pos)   { parsePosition(pos, Roles::midfielder_left); }
        void parsePositionMidfielderCenter(std::string pos) { parsePosition(pos, Roles::midfielder_center); }
        void parsePositionMidfielderRight(std::string pos)  { parsePosition(pos, Roles::midfielder_right); }
        void parsePositionForwardLeft(std::string pos)      { parsePosition(pos, Roles::forward_left); }
        void parsePositionForwardCenter(std::string pos)    { parsePosition(pos, Roles::forward_center); }
        void parsePositionForwardRight(std::string pos)     { parsePosition(pos, Roles::forward_right); }

        void parsePosition(std::string& positions, Roles::Static role)
        {
            auto& r = provider->getRoles().defaults[role];

            std::vector<std::string> parts = naoth::StringTools::split(positions, ';');
            ASSERT(parts.size() == 3);

            std::vector<std::string> pos_part;

            pos_part = naoth::StringTools::split(parts[0], ',');
            ASSERT(pos_part.size() == 2);

            r.home.x = std::stod(pos_part[0]) / base_x * provider->getFieldInfo().xLength;
            r.home.y = std::stod(pos_part[1]) / base_y * provider->getFieldInfo().yLength;

            pos_part = naoth::StringTools::split(parts[1], ',');
            ASSERT(pos_part.size() == 2);
            r.own.x = std::stod(pos_part[0]) / base_x * provider->getFieldInfo().xLength;
            r.own.y = std::stod(pos_part[1]) / base_y * provider->getFieldInfo().yLength;

            pos_part = naoth::StringTools::split(parts[2], ',');
            ASSERT(pos_part.size() == 2);
            r.opp.x = std::stod(pos_part[0]) / base_x * provider->getFieldInfo().xLength;
            r.opp.y = std::stod(pos_part[1]) / base_y * provider->getFieldInfo().yLength;
        }

        /**
         * @brief If the 'active' parameter changes, the new value is parsed and the new active
         *        roles are applied. The "all" shortcut is used to activate all roles otherwise
         *        all roles which should be activated are listed and seperated by a ';'.
         *        Eg.: "goalie;defender_left;forward_center"
         *
         * @param active    the new roles to activate
         */
        void parseActive(std::string a) {
            // clear the 'old' active roles before pushing the new
            provider->getRoles().active.clear();
            if(a.compare("all") == 0) {
                // user all static roles, but ignore the "unknown" one
                for(int i = 0; i < Roles::numOfStaticRoles-1; ++i)
                {
                    provider->getRoles().active.push_back(static_cast<Roles::Static>(i));
                }
            } else {
                // only use the defined roles
                std::vector<std::string> parts = naoth::StringTools::split(a, ';');
                for(const std::string& part : parts) {
                    Roles::Static r = Roles::getStaticRole(part);
                    ASSERT_MSG(Roles::unknown != r, "Role is unknown: '" + part + "'");
                    provider->getRoles().active.push_back(r);
                }
            }

        }
    } params;

    void inline debugDrawings()
    {
        DEBUG_REQUEST("RoleDecision:Position:defaults:draw_active_home_positions",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoles().defaults) {
                if(getRoles().isRoleActive(r.first)) {
                    PEN("0000ff", 20);
                    ROBOT(r.second.home.x, r.second.home.y, 0);
                    TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );
        DEBUG_REQUEST("RoleDecision:Position:defaults:draw_inactive_home_positions",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoles().defaults) {
                if(!getRoles().isRoleActive(r.first)) {
                    PEN("0000ff", 20);
                    ROBOT(r.second.home.x, r.second.home.y, 0);
                    TEXT_DRAWING2(r.second.home.x, r.second.home.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );
        DEBUG_REQUEST("RoleDecision:Position:defaults:draw_active_own_kickoff_positions",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoles().defaults) {
                if(getRoles().isRoleActive(r.first)) {
                    PEN("0000ff", 20);
                    ROBOT(r.second.own.x, r.second.own.y, 0);
                    TEXT_DRAWING2(r.second.own.x, r.second.own.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );
        DEBUG_REQUEST("RoleDecision:Position:defaults:draw_inactive_own_kickoff_positions",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoles().defaults) {
                if(!getRoles().isRoleActive(r.first)) {
                    PEN("0000ff", 20);
                    ROBOT(r.second.own.x, r.second.own.y, 0);
                    TEXT_DRAWING2(r.second.own.x, r.second.own.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );
        DEBUG_REQUEST("RoleDecision:Position:defaults:draw_active_opp_kickoff_positions",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoles().defaults) {
                if(getRoles().isRoleActive(r.first)) {
                    PEN("0000ff", 20);
                    ROBOT(r.second.opp.x, r.second.opp.y, 0);
                    TEXT_DRAWING2(r.second.opp.x, r.second.opp.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );
        DEBUG_REQUEST("RoleDecision:Position:defaults:draw_inactive_opp_kickoff_positions",
            FIELD_DRAWING_CONTEXT;
            for(const auto& r : getRoles().defaults) {
                if(!getRoles().isRoleActive(r.first)) {
                    PEN("0000ff", 20);
                    ROBOT(r.second.opp.x, r.second.opp.y, 0);
                    TEXT_DRAWING2(r.second.opp.x, r.second.opp.y-250, 0.6, Roles::getName(r.first));
                }
            }
        );
    }
};

#endif // ROLESPROVIDER_H
