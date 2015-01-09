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
  // enum type for team color
  for(int i = 0; i < GameData::numOfTeamColor; i++)
  {
    string str("game.team_color");
    str.append(GameData::teamColorToString((GameData::TeamColor)i));
    engine.registerEnumElement("game.team_color", str.c_str(), i);
  }//end for

  engine.registerEnumeratedInputSymbol("game.own_team_color", "game.team_color", &getOwnTeamColor);
  engine.registerEnumeratedInputSymbol("game.opponent_team_color", "game.team_color", &getOpponentTeamColor);


  // enum type for game state (spl)
  for(int i = 0; i < GameData::numOfGameState; i++)
  {
    string str("game.state.");
    if(i == (int)GameData::initial)
    {
      str.append("_");
      str.append(GameData::gameStateToString((GameData::GameState)i));
      str.append("_");
    } else {
      str.append(GameData::gameStateToString((GameData::GameState)i));
    }
    engine.registerEnumElement("game.state", str.c_str(), i);
  }//end for

  // current game state
  // (int*)&getPlayerInfo().gameData.gameState
  engine.registerEnumeratedInputSymbol("game.state", "game.state", &getGameState);


  // enum type for play mode (simulation)
  for(int i = 0; i < GameData::numOfPlayMode; i++)
  {
    string str("game.playmode.");
    //str.append(playerInfo.getPlayModeName((GameData::PlayMode)i));
    str.append(GameData::playModeToString((GameData::PlayMode)i));
    engine.registerEnumElement("game.playmode", str.c_str(), i);
  }//end for

  // register the play mode
  // (int*)&playerInfo.playMode
  engine.registerEnumeratedInputSymbol("game.playmode", "game.playmode", &getPlayMode);

  engine.registerDecimalInputSymbol("game.player_number", &getPlayerNumber);
  engine.registerDecimalInputSymbol("game.msecsRemaining", &getMsecsRemaining);
  engine.registerBooleanInputSymbol("game.own_kickoff", &getOwnKickOff);

}//end registerSymbols

GameSymbols* GameSymbols::theInstance = NULL;


bool GameSymbols::getOwnKickOff()
{
  return theInstance->getPlayerInfo().gameData.playMode == GameData::kick_off_own
      || theInstance->getPlayerInfo().gameData.playMode == GameData::penalty_kick_own;
}

double GameSymbols::getPlayerNumber() {
  return theInstance->getPlayerInfo().gameData.playerNumber;
}

double GameSymbols::getMsecsRemaining() {
  return theInstance->getPlayerInfo().gameData.msecsRemaining;
}

int GameSymbols::getGameState() {
  return theInstance->getPlayerInfo().gameData.gameState;
}

int GameSymbols::getPlayMode() {
  return theInstance->getPlayerInfo().gameData.playMode;
}

int GameSymbols::getOwnTeamColor() {
  return theInstance->getPlayerInfo().gameData.teamColor;
}

int GameSymbols::getOpponentTeamColor() {
  return !(theInstance->getPlayerInfo().gameData.teamColor);
}
