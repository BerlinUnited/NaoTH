#include "RoleDecisionModel.h"
#include <iomanip>

std::string RoleDecisionModel::getName(StaticRole role) {
    switch (role) {
        case unknown:           return "unknown";
        case goalie:            return "goalie";
        case defender_left:     return "defender_left";
        case defender_center:   return "defender_center";
        case defender_right:    return "defender_right";
        case midfielder_left:   return "midfielder_left";
        case midfielder_center: return "midfielder_center";
        case midfielder_right:  return "midfielder_right";
        case forward_left:      return "forward_left";
        case forward_center:    return "forward_center";
        case forward_right:     return "forward_right";
        default:                return "invalid";
    }
}

RoleDecisionModel::StaticRole RoleDecisionModel::getStaticRole(std::string name)
{
         if (std::strcmp(name.c_str(), "unknown")           == 0) { return unknown; }
    else if (std::strcmp(name.c_str(), "goalie")            == 0) { return goalie; }
    else if (std::strcmp(name.c_str(), "defender_left")     == 0) { return defender_left; }
    else if (std::strcmp(name.c_str(), "defender_center")   == 0) { return defender_center; }
    else if (std::strcmp(name.c_str(), "defender_right")    == 0) { return defender_right; }
    else if (std::strcmp(name.c_str(), "midfielder_left")   == 0) { return midfielder_left; }
    else if (std::strcmp(name.c_str(), "midfielder_center") == 0) { return midfielder_center; }
    else if (std::strcmp(name.c_str(), "midfielder_right")  == 0) { return midfielder_right; }
    else if (std::strcmp(name.c_str(), "forward_left")      == 0) { return forward_left; }
    else if (std::strcmp(name.c_str(), "forward_center")    == 0) { return forward_center; }
    else if (std::strcmp(name.c_str(), "forward_right")     == 0) { return forward_right; }
    return unknown;
}

std::string RoleDecisionModel::getName(DynamicRole role) {
    switch (role) {
        case none:              return "none";
        case supporter:         return "supporter";
        case goalie_supporter:  return "goalie_supporter";
        case striker:           return "striker";
        default:                return "invalid";
    }
}

RoleDecisionModel::DynamicRole RoleDecisionModel::getDynamicRole(std::string name)
{
         if (std::strcmp(name.c_str(), "none")             == 0) { return none; }
    else if (std::strcmp(name.c_str(), "supporter")        == 0) { return supporter; }
    else if (std::strcmp(name.c_str(), "goalie_supporter") == 0) { return goalie_supporter; }
    else if (std::strcmp(name.c_str(), "striker")          == 0) { return striker; }
    return none;
}

RoleDecisionModel::RolePosition RoleDecisionModel::getStaticRolePosition(StaticRole role) const
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
        return roles.at(playerNumber).dynamic == striker;
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
               << std::setw(16) << std::right << getName(it.second.role) << " : "
               << std::setw(16) << std::left  << getName(it.second.dynamic)
               << "\n";
    }
    stream << "\n----------------------------------------------------\n\n";
    for(const auto& it : roles_position) {
        stream << std::setw(17) << std::right << getName(it.first) << " : "
               << std::setw(5) << it.second.home.x << "/" << std::setw(5) << it.second.home.y << ", "
               << std::setw(5) << it.second.own.x  << "/" << std::setw(5) << it.second.own.y  << ", "
               << std::setw(5) << it.second.opp.x  << "/" << std::setw(5) << it.second.opp.y  << " \n";
    }
}
