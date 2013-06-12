/**
* @file GameSymbols2012.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class GameSymbols2012
*/

#include "GameSymbols2012.h"

using namespace naoth;
using namespace std;

void GameSymbols2012::registerSymbols(xabsl::Engine& engine)
{

  for(int i = 0; i < GameData::numOfTeamColor; i++)
  {
    string str("game.team_color");
    str.append(GameData::teamColorToString((GameData::TeamColor)i));
    engine.registerEnumElement("game.team_color", str.c_str(), i);
  }//end for

  engine.registerEnumeratedInputSymbol("game.own_team_color", "game.team_color", &getOwnTeamColor);
  engine.registerEnumeratedInputSymbol("game.opponent_team_color", "game.team_color", &getOpponentTeamColor);


  // register the enum type for game state
  for(int i = 0; i < GameData::numOfGameState; i++)
  {
    string str("game.state.");
    if(i == (int)GameData::initial)
    {
      str.append("_");
      str.append(GameData::gameStateToString((GameData::GameState)i));
      str.append("_");
    }
    else
    {
      str.append(GameData::gameStateToString((GameData::GameState)i));
    }
    engine.registerEnumElement("game.state", str.c_str(), i);
  }//end for

  // register the game state
  engine.registerEnumeratedInputSymbol("game.state", "game.state", (int*)&playerInfo.gameData.gameState);

  // register the enum type for play mode
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

GameSymbols2012* GameSymbols2012::theInstance = NULL;

void GameSymbols2012::execute()
{
}

bool GameSymbols2012::getOwnKickOff()
{
  return theInstance->playerInfo.gameData.playMode == GameData::kick_off_own
      || theInstance->playerInfo.gameData.playMode == GameData::penalty_kick_own;
}//end getOwnKickOff

double GameSymbols2012::getPlayerNumber()
{
  return theInstance->playerInfo.gameData.playerNumber;
}//end setWalkSpeedRot

double GameSymbols2012::getMsecsRemaining()
{
  return theInstance->playerInfo.gameData.msecsRemaining;
}

int GameSymbols2012::getPlayMode()
{
  return theInstance->playerInfo.gameData.playMode;
  //return 1;
}//end getPlayMode

int GameSymbols2012::getOwnTeamColor()
{
  return theInstance->playerInfo.gameData.teamColor;
}//end getOwnTeamColor

int GameSymbols2012::getOpponentTeamColor()
{
  return !(theInstance->playerInfo.gameData.teamColor);
}//end getOpponentTeamColor
