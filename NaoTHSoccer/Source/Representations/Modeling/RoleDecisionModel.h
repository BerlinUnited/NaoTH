/**
* @file RoleDecisionModel.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef ROLEDECISIONMODEL_H
#define ROLEDECISIONMODEL_H

#include <limits>
#include <ostream>
#include <cstring>
#include <map>

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"

class RoleDecisionModel: public naoth::Printable
{
public:
    RoleDecisionModel() {}

    enum StaticRole {
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

    enum DynamicRole {
        none,
        supporter,
        goalie_supporter,
        striker,
        // ------------
        numOfDynamicRoles
    };

    struct RolePosition
    {
        Vector2d home; // home position
        Vector2d own; // position for own kickoff
        Vector2d opp; // position for opp kickoff
    };

    struct Role
    {
        Role():role(unknown){}
        Role(StaticRole r):role(r){}

        StaticRole role; // the actual role
        DynamicRole dynamic; // the actual role
    };

    std::map<unsigned int, Role> roles;
    std::map<StaticRole, RolePosition> roles_position;

    static std::string getName(StaticRole role);
    static std::string getName(DynamicRole role);

    static StaticRole getStaticRole(std::string name);
    static DynamicRole getDynamicRole(std::string name);

    RolePosition getStaticRolePosition(StaticRole role) const;

    bool isStriker(unsigned int playerNumber) const;

    /* OLD STUFF ------------------------------------------------ */

    unsigned int firstStriker = std::numeric_limits<unsigned int>::max();
    unsigned int secondStriker = std::numeric_limits<unsigned int>::max();
    bool wantsToBeStriker = false;

    virtual void print(std::ostream& stream) const;

    void resetStriker() {
        firstStriker = std::numeric_limits<unsigned int>::max();
        secondStriker = std::numeric_limits<unsigned int>::max();
    }
};


#endif  /* ROLEDECISIONMODEL_H */

