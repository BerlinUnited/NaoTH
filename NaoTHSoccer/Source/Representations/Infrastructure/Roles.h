#ifndef ROLES_H
#define ROLES_H

#include "Tools/StringTools.h"
#include "Tools/Math/Vector2.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/Printable.h"

class Roles : public ParameterList, public naoth::Printable
{
public:
    Roles();

    // NOTE: the order of the roles is important!
    //       it's used to determine the field grid in RoleDecisionPositionDynamic
    enum Static {
        defender_left = 0,
        defender_center = 1,
        defender_right = 2,
        midfielder_left = 3,
        midfielder_center = 4,
        midfielder_right = 5,
        forward_left = 6,
        forward_center = 7,
        forward_right = 8,
        goalie = 9,
        unknown = 10,
        // ------------
        numOfStaticRoles
    };

    enum Dynamic {
        none,
        supporter,
        goalie_supporter,
        striker,
        // ------------
        numOfDynamicRoles
    };

    struct Position
    {
        Vector2d home; // home position
        Vector2d own; // position for own kickoff
        Vector2d opp; // position for opp kickoff
    };

    struct Role
    {
        Role():role(unknown){}
        Role(Static r):role(r){}

        Static role; // the actual role
        Dynamic dynamic; // the actual role
    };

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

    std::map<Static, Position> defaults;

    std::string active_str;
    std::vector<Static> active;

    void parsePositionGoalie(std::string pos)           { parsePosition(pos, goalie); }
    void parsePositionDefenderLeft(std::string pos)     { parsePosition(pos, defender_left); }
    void parsePositionDefenderCenter(std::string pos)   { parsePosition(pos, defender_center); }
    void parsePositionDefenderRight(std::string pos)    { parsePosition(pos, defender_right); }
    void parsePositionMidfielderLeft(std::string pos)   { parsePosition(pos, midfielder_left); }
    void parsePositionMidfielderCenter(std::string pos) { parsePosition(pos, midfielder_center); }
    void parsePositionMidfielderRight(std::string pos)  { parsePosition(pos, midfielder_right); }
    void parsePositionForwardLeft(std::string pos)      { parsePosition(pos, forward_left); }
    void parsePositionForwardCenter(std::string pos)    { parsePosition(pos, forward_center); }
    void parsePositionForwardRight(std::string pos)     { parsePosition(pos, forward_right); }

    void parsePosition(std::string& positions, Static role)
    {
        auto& r = defaults[role];

        std::vector<std::string> parts = naoth::StringTools::split(positions, ';');
        ASSERT(parts.size() == 3);

        std::vector<std::string> pos_part;

        pos_part = naoth::StringTools::split(parts[0], ',');
        ASSERT(pos_part.size() == 2);

        r.home.x = std::stod(pos_part[0]) / base_x * field_x;
        r.home.y = std::stod(pos_part[1]) / base_y * field_y;

        pos_part = naoth::StringTools::split(parts[1], ',');
        ASSERT(pos_part.size() == 2);
        r.own.x = std::stod(pos_part[0]) / base_x * field_x;
        r.own.y = std::stod(pos_part[1]) / base_y * field_y;

        pos_part = naoth::StringTools::split(parts[2], ',');
        ASSERT(pos_part.size() == 2);
        r.opp.x = std::stod(pos_part[0]) / base_x * field_x;
        r.opp.y = std::stod(pos_part[1]) / base_y * field_y;
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
        active.clear();
        if(a.compare("all") == 0) {
            // user all static roles, but ignore the "unknown" one
            for(int i = 0; i < Roles::numOfStaticRoles-1; ++i)
            {
                active.push_back(static_cast<Roles::Static>(i));
            }
        } else {
            // only use the defined roles
            std::vector<std::string> parts = naoth::StringTools::split(a, ';');
            for(const std::string& part : parts) {
                Roles::Static r = Roles::getStaticRole(part);
                ASSERT(Roles::unknown != r);
                active.push_back(r);
            }
        }

    }

    static std::string getName(Static role);
    static std::string getName(Dynamic role);

    static Static getStaticRole(std::string name);
    static Dynamic getDynamicRole(std::string name);

    bool isRoleActive(Static r) const;

    virtual void print(std::ostream& stream) const;
};

#endif // ROLES_H
