#include "Roles.h"
#include <iomanip>

Roles::Roles() : ParameterList("Roles")
{
    PARAMETER_REGISTER(base_x) = 9000;
    PARAMETER_REGISTER(base_y) = 6000;

    // TODO: how to get that from the FieldInfo?!
    field_x = 9000; //fi.xLength;
    field_y = 6000; //fi.yLength;

    // x,y home position; x,y own kickoff position; x,y opponent kickoff position
    PARAMETER_REGISTER(goalie_str,           &Roles::parsePositionGoalie)          = "-4300,    0; -4300,   0;  -4300,    0";
    PARAMETER_REGISTER(defender_left_str,    &Roles::parsePositionDefenderLeft)    = "-2500, 1100; -3000, 1500; -3000, 1300";
    PARAMETER_REGISTER(defender_center_str,  &Roles::parsePositionDefenderCenter)  = "-2500,    0; -3000,    0; -3000,    0";
    PARAMETER_REGISTER(defender_right_str,   &Roles::parsePositionDefenderRight)   = "-2500,-1100; -3000,-1500; -3000, -450";
    PARAMETER_REGISTER(midfielder_left_str,  &Roles::parsePositionMidfielderLeft)  = "    0, 1500; -1500,  900; -2000,  500";
    PARAMETER_REGISTER(midfielder_center_str,&Roles::parsePositionMidfielderCenter)= "    0,    0; -1500,    0; -2000,    0";
    PARAMETER_REGISTER(midfielder_right_str, &Roles::parsePositionMidfielderRight) = "    0,-1500; -1500, -900; -2000, -500";
    PARAMETER_REGISTER(forward_left_str,     &Roles::parsePositionForwardLeft)     = " 1500, 1500;  -500, 1500; -1000,  750";
    PARAMETER_REGISTER(forward_center_str,   &Roles::parsePositionForwardCenter)   = " 1500,    0;  -500,    0; -1000,  250";
    PARAMETER_REGISTER(forward_right_str,    &Roles::parsePositionForwardRight)    = " 1500,-1500;  -500,-1500; -1000, -750";

    // load from the file after registering all parameters
    syncWithConfig();
}

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


void Roles::print(std::ostream& stream) const
{
    stream << "Position base: " << base_x << "/" << base_y << "\n"
           << "Field size: " << field_x << "/" << field_y << "\n"
           << "\n----------------------------------------------------\n\n"
           << "Default role positions:\n";

    for(const auto& it : defaults) {
        stream << std::setw(17) << std::right << Roles::getName(it.first) << " : "
               << std::setw(5) << it.second.home.x << "/" << std::setw(5) << it.second.home.y << ", "
               << std::setw(5) << it.second.own.x  << "/" << std::setw(5) << it.second.own.y  << ", "
               << std::setw(5) << it.second.opp.x  << "/" << std::setw(5) << it.second.opp.y  << " \n";
    }
}
