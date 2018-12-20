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

    enum RoleEnum {
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
        numOfRoles
    };

    struct Role
    {
        Role():role(unknown){}
        Role(RoleEnum r):role(r){}

        RoleEnum role; // the actual role
        Vector2d home; // home position
        Vector2d own; // position for own kickoff
        Vector2d opp; // position for opp kickoff
    };

    std::map<unsigned int, Role> roles;

    static std::string getName(RoleEnum role);
    static RoleEnum getRole(std::string name);

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

