// 
// File:   PlayerInfo.cpp
// Author: thomas
// Author: Heinrich Mellmann
//

#include "PlayerInfo.h"
#include "PlatformInterface/Platform.h"

using namespace naoth;
using namespace std;

PlayerInfo::PlayerInfo() 
:
  playersPerTeam(0),
  playerNumber(0),
  teamNumber(0),
  teamColor(naoth::GameData::unknown_team_color),
  kickoff(false),
  robotState(unstiff), // this was changed in 2021 from initial to unstiff
  robotSetPlay(set_none),
  gamePhase(normal),
  isPlayingStriker(false),
  scheme(Platform::getInstance().theScheme)
{
}

PlayerInfo::~PlayerInfo()
{
}


#define RETURN_VALUE_TO_STR(v) case v: return #v
std::string PlayerInfo::toString(RobotState value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(initial);
    RETURN_VALUE_TO_STR(ready);
    RETURN_VALUE_TO_STR(set);
    RETURN_VALUE_TO_STR(playing);
    RETURN_VALUE_TO_STR(finished);
    RETURN_VALUE_TO_STR(penalized);
    RETURN_VALUE_TO_STR(unstiff);
  }
  
  ASSERT(false);
  return "invalid RobotState";
}

std::string PlayerInfo::toString(RobotSetPlay value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(set_none);
    RETURN_VALUE_TO_STR(goal_free_kick);
    RETURN_VALUE_TO_STR(pushing_free_kick);
    RETURN_VALUE_TO_STR(corner_kick);
    RETURN_VALUE_TO_STR(kick_in);
  }

  ASSERT(false);
  return "invalid RobotSetPlay";
}

std::string PlayerInfo::toString(GamePhase value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(normal);
    RETURN_VALUE_TO_STR(penaltyshoot);
    RETURN_VALUE_TO_STR(overtime);
    RETURN_VALUE_TO_STR(timeout);
  }

  ASSERT(false);
  return "invalid SecondaryGameState";
}


void PlayerInfo::print(ostream& stream) const
{
  stream << "playerNumber = " << playerNumber << endl;
  stream << "teamNumber = " << teamNumber << endl;
  stream << "teamName = " << teamName << endl;
  stream << "teamColor = " << naoth::GameData::toString(teamColor) << endl;
  stream << "kickoff = " << (kickoff?"yes":"no") << endl;
  stream << "robotState = " << toString(robotState) << endl;
  stream << "robotSetPlay = " << toString(robotSetPlay) << endl;
  stream << "gamephase = " << toString(gamePhase) << endl;
  stream << "isPlayingStriker = " << (isPlayingStriker?"yes":"no") << endl;
  stream << "active scheme = " << (scheme.empty()?"-":scheme) << std::endl;
}

