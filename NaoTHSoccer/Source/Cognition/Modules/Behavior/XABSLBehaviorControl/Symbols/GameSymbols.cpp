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
  // enum for the competition type
  engine.registerEnumElement("game.type", "game.type.normal"  , GameData::competition_normal);
  engine.registerEnumElement("game.type", "game.type.game_1v1", GameData::competition_1v1);
  engine.registerEnumElement("game.type", "game.type.passing" , GameData::competition_passing);

  engine.registerEnumeratedInputSymbol("game.type", "game.type", &getCompetitionType);


  // enum type for game state (spl)
  engine.registerEnumElement("game.state","game.state._initial_", PlayerInfo::initial);
  engine.registerEnumElement("game.state","game.state.ready", PlayerInfo::ready);
  engine.registerEnumElement("game.state","game.state.set", PlayerInfo::set);
  engine.registerEnumElement("game.state","game.state.playing", PlayerInfo::playing);
  engine.registerEnumElement("game.state","game.state.penalized", PlayerInfo::penalized);
  engine.registerEnumElement("game.state","game.state.finished", PlayerInfo::finished);
  engine.registerEnumElement("game.state","game.state.unstiff", PlayerInfo::unstiff);

  // current game state
  engine.registerEnumeratedInputSymbol("game.state", "game.state", &getGameState);


  engine.registerDecimalInputSymbol("game.player_number", &getPlayerNumber);
  engine.registerDecimalInputSymbol("game.msecsRemaining", &getMsecsRemaining);
  engine.registerDecimalInputSymbol("game.msecsRemainingSecondary", &getMsecsRemainingSecondary);
  engine.registerBooleanInputSymbol("game.own_kickoff", &getOwnKickOff);

  // HACK: is only true when the game state plaing was set by the game controller
  //       we assume this message comes with a delay of >10s
  //       it's used to prevent waiting of additional 10s in case it's opponents kickoff
  engine.registerBooleanInputSymbol("game.state.playing_is_set_by_game_controller", &getPlayingIsSetByGameController);


  engine.registerEnumElement("game.set_play","game.set_play.none", PlayerInfo::set_none);
  engine.registerEnumElement("game.set_play","game.set_play.goal_free_kick", PlayerInfo::goal_free_kick);
  engine.registerEnumElement("game.set_play","game.set_play.pushing_free_kick", PlayerInfo::pushing_free_kick);
  engine.registerEnumElement("game.set_play","game.set_play.corner_kick", PlayerInfo::corner_kick);
  engine.registerEnumElement("game.set_play","game.set_play.kick_in", PlayerInfo::kick_in);

  engine.registerEnumeratedInputSymbol("game.set_play", "game.set_play", &getSetPlay);

  // enum type for game phase
  engine.registerEnumElement("game.phase", "game.phase.normal", PlayerInfo::normal);
  engine.registerEnumElement("game.phase", "game.phase.penaltyshoot", PlayerInfo::penaltyshoot);
  engine.registerEnumElement("game.phase", "game.phase.overtime", PlayerInfo::overtime);
  engine.registerEnumElement("game.phase", "game.phase.timeout", PlayerInfo::timeout);

  // current game phase
  engine.registerEnumeratedInputSymbol("game.phase", "game.phase", &getGamePhase);
  

}//end registerSymbols

GameSymbols* GameSymbols::theInstance = NULL;
bool GameSymbols::playingIsSetByGameController = false;

int GameSymbols::getCompetitionType() {
  return theInstance->getGameData().competitionType;
}

bool GameSymbols::getPlayingIsSetByGameController() {
  if (theInstance->getGameData().valid) {
    playingIsSetByGameController = (theInstance->getGameData().gameState == GameData::playing);
  }
  return playingIsSetByGameController;
}

bool GameSymbols::getOwnKickOff() {
  return theInstance->getPlayerInfo().kickoff;
}

double GameSymbols::getPlayerNumber() {
  return theInstance->getPlayerInfo().playerNumber;
}

double GameSymbols::getMsecsRemaining() {
  return theInstance->getGameData().secsRemaining*1000.0;
}

double GameSymbols::getMsecsRemainingSecondary() {
  return theInstance->getGameData().secondaryTime*1000.0;
}

int GameSymbols::getGameState() {
  return theInstance->getPlayerInfo().robotState;
}

int GameSymbols::getSetPlay() {
  return theInstance->getPlayerInfo().robotSetPlay;
}

int GameSymbols::getGamePhase(){
  return theInstance->getPlayerInfo().gamePhase;
}