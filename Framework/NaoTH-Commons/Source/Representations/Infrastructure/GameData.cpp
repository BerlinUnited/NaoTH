/**
 * @file GameInfo.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief the game information in RoboCup
 */

#include <iostream>
#include "GameData.h"

using namespace naoth;

GameData::GameData()
  :frameNumber(0),
    gameState(numOfGameState),
    timeWhenGameStateChanged(0),
    penaltyState(none),
    secsTillUnpenalised(0),
    secsRemaining(600),
    firstHalf(true),
    playMode(numOfPlayMode),
    gameTime(0),
    timeWhenPlayModeChanged(0),
    playerNumber(0),
    teamColor(numOfTeamColor),
    teamNumber(0),
    teamName("NaoTH"),
    halfTime(0),
    numOfPlayers(0)
{
}

GameData::GameState GameData::gameStateFromString(const std::string& name)
{
  for(int i = 0; i < numOfGameState; i++)
  {
    if(gameStateToString((GameState)i) == name)
      return (GameState)i;
  }//end for
  return numOfGameState;
}//end gameStateFromString

std::string GameData::gameStateToString(GameData::GameState gameState)
{
  switch (gameState)
  {
    case initial: return "initial";
    case ready: return "ready";
    case set: return "set";
    case playing: return "playing";
    case penalized: return "penalized";
    case finished: return "finished";
    default: return "unknown";
  }//end switch
}//end gameStateToString

GameData::PlayMode GameData::playModeFromString(const std::string& name)
{
  for(int i = 0; i < numOfPlayMode; i++)
  {
    if(playModeToString((PlayMode)i) == name)
      return (PlayMode)i;
  }//end for
  return numOfPlayMode;
}//end playModeFromString

std::string GameData::playModeToString(GameData::PlayMode mode)
{
  switch(mode)
  {
  case before_kick_off: return "before_kick_off";
  case kick_off_own: return "kick_off_own";
  case kick_off_opp: return "kick_off_opp";
  case play_on: return "play_on";
  case kick_in_own: return "kick_in_own";
  case kick_in_opp: return "kick_in_opp";
  case corner_kick_own: return "corner_kick_own";
  case corner_kick_opp: return "corner_kick_opp";
  case goal_kick_own: return "goal_kick_own";
  case goal_kick_opp: return "goal_kick_opp";
  case offside_own: return "offside_own";
  case offside_opp: return "offside_opp";
  case game_over: return "game_over";
  case goal_own: return "goal_own";
  case goal_opp: return "goal_opp";
  case free_kick_own: return "free_kick_own";
  case free_kick_opp: return "free_kick_opp";
  case penalty_kick_own: return "penalty_kick_own";
  case penalty_kick_opp: return "penalty_kick_opp";
  default: return "unknown";
  }//end switch
}//end playModeToString

std::string GameData::teamColorToString(TeamColor teamColor)
{
  switch (teamColor)
  {
    case red: return "red";
    case blue: return "blue";
    default: return "unknown";
  }//end switch
}//end teamColorToString

GameData::TeamColor GameData::teamColorFromString(const std::string& teamColor)
{
  for(int i = 0; i < numOfTeamColor; i++)
  {
    if(teamColorToString((TeamColor)i) == teamColor)
      return (TeamColor)i;
  }//end for
  return numOfTeamColor;
}//end teamColorFromString

GameData::TeamColor operator! (const GameData::TeamColor& color)
{
  switch(color)
  {
    case GameData::blue: return GameData::red;
    case GameData::red: return GameData::blue;
    default: return GameData::numOfTeamColor;
  }//end switch
}//end operator !

void GameData::print(ostream& stream) const
{
  stream << "GameData @" << frameNumber<<"\n";
  stream << "gameState = " << gameStateToString(gameState) << " since "<< timeWhenGameStateChanged << "\n";
  if ( gameState == penalized )
  {
    stream << penaltyStateToString(penaltyState) << " [secsTillUnpenalised = " << secsTillUnpenalised << "]\n";
  }
  stream << (firstHalf?"first":"second") <<" half remains "<< secsRemaining << "ms\n";
  stream << "PlayMode = " << playModeToString(playMode) << " since " << timeWhenPlayModeChanged <<"\n";
  stream << "playerNumber = " << playerNumber << "\n";
  stream << "teamColor = " << teamColorToString(teamColor) << "\n";
  stream << "teamNumber = " << teamNumber << "\n";
  stream << "teamName = "<< teamName << "\n";
  stream << "numOfPlayers = " << numOfPlayers << "\n";
}//end print

void GameData::loadFromCfg(Configuration& config)
{
  if (config.hasKey("player", "NumOfPlayer"))
  {
    numOfPlayers = config.getInt("player", "NumOfPlayer");
  } else
  {
    std::cerr << "No number of players (NumOfPlayers) given" << std::endl;
  }

  if (config.hasKey("player", "PlayerNumber"))
  {
    playerNumber = config.getInt("player", "PlayerNumber");
  } else
  {
    std::cerr << "No player number (PlayerNumber) given" << std::endl;
    playerNumber = 3;
  }

  if (config.hasKey("player", "TeamColor"))
  {
    teamColor = teamColorFromString(config.getString("player", "TeamColor"));
    if (teamColor == GameData::numOfTeamColor)
    {
      teamColor = GameData::red;
      std::cerr << "Invalid team color (TeamColor) \""
        << config.getString("player", "TeamColor") << "\" given" << std::endl;
    }
  } else
  {
    std::cerr << "No team color (TeamColor) given" << std::endl;
    teamColor = GameData::red;
  }

  if (config.hasKey("player", "TeamNumber"))
  {
    teamNumber = config.getInt("player", "TeamNumber");
  } else
  {
    std::cerr << "No team number (TeamNumber) given" << std::endl;
    teamNumber = 0;
  }
} // end loadPlayerInfoFromFile

std::string GameData::penaltyStateToString(PenaltyState state)
{
  switch (state)
  {
  case ball_holding: return "ball_holding";
  case player_pushing: return "player_pushing";
  case obstruction: return "obstruction";
  case inactive_player: return "inactive_player";
  case illegal_defender: return "illegal_defender";
  case leaving_the_field: return "leaving_the_field";
  case playing_with_hands: return "playing_with_hands";
  case request_for_pickup: return "request_for_pickup";
  case manual: return "manual";
  default: return "none";
  }//end switch
}//end gameStateToString

string GameReturnData::messageToString(GameReturnData::Message msg)
{
  switch(msg)
  {
  case manual_penalise: return "manual_penalise";
  case manual_unpenalise: return "manual_unpenalise";
  default: return "alive";
  }
}

