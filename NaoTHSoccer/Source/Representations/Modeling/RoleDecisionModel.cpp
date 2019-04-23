#include "RoleDecisionModel.h"
#include <iomanip>

Roles::Role RoleDecisionModel::getRole(unsigned int playerNumber) const
{
    const auto& it = roles.find(playerNumber);
    if(it != roles.cend()) {
        return it->second;
    }
    return {};
}

Roles::Position RoleDecisionModel::getStaticRolePosition(Roles::Static role) const
{
    const auto& it = roles_position.find(role);
    if(it != roles_position.cend()) {
        return it->second;
    }
    return {{},{},{}};
}

bool RoleDecisionModel::isStriker(unsigned int playerNumber) const
{
    if(roles.find(playerNumber) != roles.cend()) {
        return roles.at(playerNumber).dynamic == Roles::striker;
    }
    return false;
}

void RoleDecisionModel::print(std::ostream& stream) const {

    if (firstStriker != std::numeric_limits<unsigned int>::max()) {
        stream << "First Striker: " << "Robot No. " << firstStriker << "\n";
    }
    else {
        stream << "Currently no first striker in use.\n";
    }

    if (secondStriker != std::numeric_limits<unsigned int>::max()) {
        stream << "Second Striker: " << "Robot No. " << secondStriker << "\n";
    }
    else {
        stream << "Currently no second striker in use.\n\n";
    }

    if (wantsToBeStriker) {
        stream << "Robot wants to be a striker in the next round.\n\n";
    }
    else {
        stream << "Robot does not want to be a striker in the next round.\n\n";
    }
    stream << "----------------------------------------------------\n\n";
    for(const auto& it : roles) {
        stream << std::setw(2)  << std::right << it.first << " : "
               << std::setw(16) << std::right << Roles::getName(it.second.role) << " : "
               << std::setw(16) << std::left  << Roles::getName(it.second.dynamic)
               << "\n";
    }
    stream << "\n----------------------------------------------------\n\n";
    for(const auto& it : roles_position) {
        stream << std::setw(17) << std::right << Roles::getName(it.first) << " : "
               << std::setw(5) << it.second.home.x << "/" << std::setw(5) << it.second.home.y << ", "
               << std::setw(5) << it.second.own.x  << "/" << std::setw(5) << it.second.own.y  << ", "
               << std::setw(5) << it.second.opp.x  << "/" << std::setw(5) << it.second.opp.y  << " \n";
    }
}
