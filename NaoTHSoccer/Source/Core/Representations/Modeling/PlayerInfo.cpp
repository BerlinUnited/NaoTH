// 
// File:   PlayerInfo.cpp
// Author: thomas
//
// Created on 12. march 2008, 13:41
//

#include "PlayerInfo.h"

using namespace naoth;
using namespace std;

PlayerInfo::PlayerInfo() 
:
  timeSincePlayModeChanged(0),
  timeSinceGameStateChanged(0),
  isPlayingStriker(false)
{
}

PlayerInfo::~PlayerInfo()
{
}

void PlayerInfo::print(ostream& stream) const
{
  stream << gameData;
  stream << "isPlayingStriker = " << (isPlayingStriker?"yes":"no") << endl;
}//end print

