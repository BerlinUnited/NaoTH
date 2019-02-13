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

    enum Static {
        unknown,
        goalie,
        defender_left,
        defender_center,
        defender_right,
        midfielder_left,
        midfielder_center,
        midfielder_right,
        forward_left,
        forward_center,
        forward_right,
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


    static std::string getName(Static role);
    static std::string getName(Dynamic role);

    static Static getStaticRole(std::string name);
    static Dynamic getDynamicRole(std::string name);

    virtual void print(std::ostream& stream) const;
};

#endif // ROLES_H
