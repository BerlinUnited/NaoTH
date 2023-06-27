#include "Roles.h"
#include <iomanip>

std::string Roles::getName(Roles::Static role) {
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

Roles::Static Roles::getStaticRole(std::string name)
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

std::string Roles::getName(Roles::Dynamic role) {
    switch (role) {
        case none:              return "none";
        case supporter:         return "supporter";
        case goalie_supporter:  return "goalie_supporter";
        case striker:           return "striker";
        default:                return "invalid";
    }
}

Roles::Dynamic Roles::getDynamicRole(std::string name)
{
         if (std::strcmp(name.c_str(), "none")             == 0) { return none; }
    else if (std::strcmp(name.c_str(), "supporter")        == 0) { return supporter; }
    else if (std::strcmp(name.c_str(), "goalie_supporter") == 0) { return goalie_supporter; }
    else if (std::strcmp(name.c_str(), "striker")          == 0) { return striker; }
    return none;
}

bool Roles::isRoleActive(Roles::Static r) const
{
    return std::find(active.begin(), active.end(), r) != active.end();
}

bool Roles::isDefender(Static r)
{
    return r == defender_left || r == defender_center || r == defender_right;
}

bool Roles::isMidfielder(Static r)
{
    return r == midfielder_left || r == midfielder_center || r == midfielder_right;
}

bool Roles::isForward(Static r)
{
    return r == midfielder_left || r == midfielder_center || r == midfielder_right;
}

void Roles::print(std::ostream& stream) const
{
    stream << "----------------------------------------------------\n"
           << "Active roles ("<<active.size()<<"):\n";
    for(const auto& r : active) { stream << getName(r) << ", "; }
    stream << "\n----------------------------------------------------\n"
           << "Default role positions:\n";

    for(const auto& it : defaults) {
        stream << std::setw(17) << std::right << Roles::getName(it.first) << " : "
               << std::setw(5) << it.second.home.x << "/" << std::setw(5) << it.second.home.y << ", "
               << std::setw(5) << it.second.own.x  << "/" << std::setw(5) << it.second.own.y  << ", "
               << std::setw(5) << it.second.opp.x  << "/" << std::setw(5) << it.second.opp.y  << " \n";
    }
}
