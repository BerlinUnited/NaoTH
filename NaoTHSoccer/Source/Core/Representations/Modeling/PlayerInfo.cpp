// 
// File:   PlayerInfo.cpp
// Author: thomas
//
// Created on 12. März 2008, 13:41
//

#include "PlayerInfo.h"

using namespace naoth;

PlayerInfo::PlayerInfo() 
  :isPlayingStriker(false)
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

