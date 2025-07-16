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
#include "Tools/DataStructures/Serializer.h"
#include "Tools/Math/Vector2.h"
#include "Representations/Infrastructure/Roles.h"

class RoleDecisionModel: public naoth::Printable
{
public:
    RoleDecisionModel() {}

    std::map<unsigned int, Roles::Role> roles;
    std::map<Roles::Static, Roles::Position> roles_position;

    /** The dynamic position. */
    Vector2d dynamic_position;

    Roles::Role getRole(unsigned int playerNumber) const;
    Roles::Position getStaticRolePosition(Roles::Static role) const;

    /** Returns the player number of the given role, 0 if not found */
    unsigned int getPlayerNumber(Roles::Dynamic role) const;

    bool isStriker(unsigned int playerNumber) const;

    /* OLD STUFF ------------------------------------------------ */

    unsigned int firstStriker = std::numeric_limits<unsigned int>::max();
    unsigned int secondStriker = std::numeric_limits<unsigned int>::max();
    bool wantsToBeStriker = false;

    virtual void print(std::ostream& stream) const;

    void resetStriker() {
        firstStriker = std::numeric_limits<unsigned int>::max();
        secondStriker = std::numeric_limits<unsigned int>::max();
        for(auto& r : roles) { if(r.second.dynamic == Roles::striker) { r.second.dynamic = Roles::none; } }
    }
};

template<> class naoth::Serializer<RoleDecisionModel>
{
    public:
        static void serialize(const RoleDecisionModel& representation, std::ostream& stream);
        static void deserialize(std::istream& stream, RoleDecisionModel& representation);
};

#endif  /* ROLEDECISIONMODEL_H */

