#ifndef ROLES_H
#define ROLES_H

#include "Tools/StringTools.h"
#include "Tools/Math/Vector2.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/Printable.h"

class Roles : public naoth::Printable
{
public:
    Roles() {
        // init with empty
        for(int i = 0; i < Roles::numOfStaticRoles; ++i) {
            defaults[static_cast<Static>(i)] = {{},{},{}};
        }
    }

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
        Role():role(unknown),dynamic(none){}
        Role(Static r, Dynamic d):role(r),dynamic(d){}

        Static role; // the actual role
        Dynamic dynamic; // the actual role
    };

    std::vector<Static> active;
    std::map<Static, Position> defaults;

    static std::string getName(Static role);
    static std::string getName(Dynamic role);

    static Static getStaticRole(std::string name);
    static Dynamic getDynamicRole(std::string name);

    bool isRoleActive(Static r) const;
    static bool isGoalie(Static r);
    static bool isDefender(Static r);
    static bool isMidfielder(Static r);
    static bool isForward(Static r);

    virtual void print(std::ostream& stream) const;
};

#endif // ROLES_H
