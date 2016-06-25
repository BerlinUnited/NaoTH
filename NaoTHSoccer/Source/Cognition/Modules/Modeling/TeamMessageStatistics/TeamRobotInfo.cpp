#include "TeamRobotInfo.h"

TeamRobotInfo::TeamRobotInfo()
{
    // init Module
}

TeamRobotInfo::~TeamRobotInfo()
{
    // clean Module
}

void TeamRobotInfo::execute()
{
//    getTeamMessage().print(std::cout);
//    std::cout << "" << std::endl;
    for (std::map<unsigned int, TeamMessage::Data>::const_iterator i = getTeamMessage().data.begin(); i != getTeamMessage().data.end(); ++i) {
//        std::cout << "player: " << i->first << std::endl;
    }
}
