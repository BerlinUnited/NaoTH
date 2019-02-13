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
#include "Representations/Infrastructure/Roles.h"

class RoleDecisionModel: public naoth::Printable
{
public:
    RoleDecisionModel() {}

    std::map<unsigned int, Roles::Role> roles;
    std::map<Roles::Static, Roles::Position> roles_position;

    Roles::Position getStaticRolePosition(Roles::Static role) const;

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

