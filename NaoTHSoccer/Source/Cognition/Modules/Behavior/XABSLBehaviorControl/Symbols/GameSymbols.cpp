/**
* @file GameSymbols.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class GameSymbols
*/

#include "GameSymbols.h"

using namespace naoth;
using namespace std;

void GameSymbols::registerSymbols(xabsl::Engine& engine)
{

  // enum type for game state (spl)
  engine.registerEnumElement("game.state","game.state._initial_", PlayerInfo::initial);
  engine.registerEnumElement("game.state","game.state.ready", PlayerInfo::ready);
  engine.registerEnumElement("game.state","game.state.set", PlayerInfo::set);
  engine.registerEnumElement("game.state","game.state.playing", PlayerInfo::playing);
  engine.registerEnumElement("game.state","game.state.penalized", PlayerInfo::penalized);
  engine.registerEnumElement("game.state","game.state.finished", PlayerInfo::finished);

  // current game state
  engine.registerEnumeratedInputSymbol("game.state", "game.state", &getGameState);


  // enum type for play mode (simulation)
  /*
  for(int i = 0; i < GameData::numOfPlayMode; i++)
  {
    string str("game.playmode.");
    str.append(GameData::playModeToString((GameData::PlayMode)i));
    engine.registerEnumElement("game.playmode", str.c_str(), i);
  }*/

  // register the play mode
  // (int*)&playerInfo.playMode
  //engine.registerEnumeratedInputSymbol("game.playmode", "game.playmode", &getPlayMode);

  engine.registerDecimalInputSymbol("game.player_number", &getPlayerNumber);
  engine.registerDecimalInputSymbol("game.msecsRemaining", &getMsecsRemaining);
  engine.registerBooleanInputSymbol("game.own_kickoff", &getOwnKickOff);

}//end registerSymbols

GameSymbols* GameSymbols::theInstance = NULL;


bool GameSymbols::getOwnKickOff()
{
  return theInstance->getPlayerInfo().kickoff;
}

double GameSymbols::getPlayerNumber() {
  return theInstance->getPlayerInfo().playerNumber;
}

double GameSymbols::getMsecsRemaining() {
  return theInstance->getGameData().secsRemaining*1000.0;
}

int GameSymbols::getGameState() {
  return theInstance->getPlayerInfo().robotState;
}

/*
int GameSymbols::getPlayMode() {
  return theInstance->getPlayerInfo().gameData.playMode;
}*/