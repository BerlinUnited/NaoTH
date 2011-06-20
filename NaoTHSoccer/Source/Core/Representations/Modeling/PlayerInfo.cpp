// 
// File:   PlayerInfo.cpp
// Author: thomas
//
// Created on 12. März 2008, 13:41
//

#include "PlayerInfo.h"

PlayerInfo::PlayerInfo() 
  :
  gameState(unknown),
  playMode(numOfPlayMode),
  localPlayMode(numOfLocalPlayMode),
  playerNumber(1),
  teamColor(numOfTeamColor),
  teamNumber(0),
  ownKickOff(false),
  numOfPlayers(3),
  isPlayingStriker(false)
{
}

PlayerInfo::PlayerInfo(unsigned char playerNumber, TeamColor teamColor, unsigned char teamNumber, bool isPlayingStriker)
  :
  gameState(unknown),
  playMode(numOfPlayMode),
  localPlayMode(numOfLocalPlayMode),
  playerNumber(playerNumber),
  teamColor(teamColor),
  teamNumber(teamNumber),
  ownKickOff(false),
  numOfPlayers(3),
  isPlayingStriker(isPlayingStriker)
{

}

PlayerInfo::~PlayerInfo()
{

}

PlayerInfo::TeamColor PlayerInfo::teamColorFromString(std::string teamColor)
{
  for(int i = 0; i < numOfTeamColor; i++)
  {
    if(teamColorToString((TeamColor)i) == teamColor)
      return (TeamColor)i;
  }//end for
  return numOfTeamColor;
}//end teamColorFromString


PlayerInfo::GameState PlayerInfo::gameStateFromString(std::string gameState)
{
  for(int i = 0; i < numOfGameState; i++)
  {
    if(gameStateToString((GameState)i) == gameState)
      return (GameState)i;
  }//end for
  return numOfGameState;
}//end gameStateFromString


std::string PlayerInfo::teamColorToString(TeamColor teamColor)
{
  switch (teamColor)
  {
    case red: return "red";
    case blue: return "blue";
    default: return "unknown";
  }//end switch
}//end teamColorToString


std::string PlayerInfo::gameStateToString(PlayerInfo::GameState gameState)
{
  switch (gameState)
  {
    case inital: return "initial";
    case ready: return "ready";
    case set: return "set";
    case playing: return "playing";
    case penalized: return "penalized";
    case finished: return "finished";
    default: return "unknown";
  }//end switch
}//end gameStateToString


PlayerInfo::TeamColor operator! (const PlayerInfo::TeamColor& color)
{
  switch(color)
  {
    case PlayerInfo::blue: return PlayerInfo::red;
    case PlayerInfo::red: return PlayerInfo::blue;
    default: return PlayerInfo::numOfTeamColor;
  }//end switch
}//end operator !

/** mapping from PlayMode to string constants */
#define STR_PM_BeforeKickOff "BeforeKickOff"
#define STR_PM_KickOff_Left "KickOff_Left"
#define STR_PM_KickOff_Right "KickOff_Right"
#define STR_PM_PlayOn "PlayOn"
#define STR_PM_KickIn_Left "KickIn_Left"
#define STR_PM_KickIn_Right "KickIn_Right"
#define STR_PM_CORNER_KICK_LEFT "corner_kick_left"
#define STR_PM_CORNER_KICK_RIGHT "corner_kick_right"
#define STR_PM_GOAL_KICK_LEFT "goal_kick_left"
#define STR_PM_GOAL_KICK_RIGHT "goal_kick_right"
#define STR_PM_OFFSIDE_LEFT "offside_left"
#define STR_PM_OFFSIDE_RIGHT "offside_right"
#define STR_PM_GAME_OVER "GameOver"
#define STR_PM_GOAL_LEFT "Goal_Left"
#define STR_PM_GOAL_RIGHT "Goal_Right"
#define STR_PM_FREE_KICK_LEFT "free_kick_left"
#define STR_PM_FREE_KICK_RIGHT "free_kick_right"
#define STR_PM_UNKNOWN "unknown"

PlayerInfo::PlayMode PlayerInfo::getPlayModeByName(const string& name)
{
  if (STR_PM_PlayOn == name)
    return PM_PLAY_ON;
  if (STR_PM_BeforeKickOff == name)
    return PM_BEFORE_KICK_OFF;
  if (STR_PM_KickOff_Left == name)
    return PM_KICK_OFF_LEFT;
  if (STR_PM_KickOff_Right == name)
    return PM_KICK_OFF_RIGHT;
  if (STR_PM_KickIn_Left == name)
    return PM_KICK_IN_LEFT;
  if (STR_PM_KickIn_Right == name)
    return PM_KICK_IN_RIGHT;
  if (STR_PM_CORNER_KICK_LEFT == name)
    return PM_CORNER_KICK_LEFT;
  if (STR_PM_CORNER_KICK_RIGHT == name)
    return PM_CORNER_KICK_RIGHT;
  if (STR_PM_GOAL_KICK_LEFT == name)
    return PM_GOAL_KICK_LEFT;
  if (STR_PM_GOAL_KICK_RIGHT == name)
    return PM_GOAL_KICK_RIGHT;
  if (STR_PM_OFFSIDE_LEFT == name)
    return PM_OFFSIDE_LEFT;
  if (STR_PM_OFFSIDE_RIGHT == name)
    return PM_OFFSIDE_RIGHT;
  if (STR_PM_GAME_OVER == name)
    return PM_GAME_OVER;
  if (STR_PM_GOAL_LEFT == name)
    return PM_GOAL_LEFT;
  if (STR_PM_GOAL_RIGHT == name)
    return PM_GOAL_RIGHT;
  if (STR_PM_FREE_KICK_LEFT == name)
    return PM_FREE_KICK_LEFT;
  if (STR_PM_FREE_KICK_RIGHT == name)
    return PM_FREE_KICK_RIGHT;

  return numOfPlayMode;
}

string PlayerInfo::getPlayModeName(PlayerInfo::PlayMode pm)
{
  switch (pm) {
  case PM_BEFORE_KICK_OFF:
    return STR_PM_BeforeKickOff;
  case PM_KICK_OFF_LEFT:
    return STR_PM_KickOff_Left;
  case PM_KICK_OFF_RIGHT:
    return STR_PM_KickOff_Right;
  case PM_PLAY_ON:
    return STR_PM_PlayOn;
  case PM_KICK_IN_LEFT:
    return STR_PM_KickIn_Left;
  case PM_KICK_IN_RIGHT:
    return STR_PM_KickIn_Right;
  case PM_CORNER_KICK_LEFT:
    return STR_PM_CORNER_KICK_LEFT;
  case PM_CORNER_KICK_RIGHT:
    return STR_PM_CORNER_KICK_RIGHT;
  case PM_GOAL_KICK_LEFT:
    return STR_PM_GOAL_KICK_LEFT;
  case PM_GOAL_KICK_RIGHT:
    return STR_PM_GOAL_KICK_RIGHT;
  case PM_OFFSIDE_LEFT:
    return STR_PM_OFFSIDE_LEFT;
  case PM_OFFSIDE_RIGHT:
    return STR_PM_OFFSIDE_RIGHT;
  case PM_GAME_OVER:
    return STR_PM_GAME_OVER;
  case PM_GOAL_LEFT:
    return STR_PM_GOAL_LEFT;
  case PM_GOAL_RIGHT:
    return STR_PM_GOAL_RIGHT;
  case PM_FREE_KICK_LEFT:
    return STR_PM_FREE_KICK_LEFT;
  case PM_FREE_KICK_RIGHT:
    return STR_PM_FREE_KICK_RIGHT;
  default: return STR_PM_UNKNOWN;
  }
}

string PlayerInfo::getLocalPlayModename(PlayerInfo::LocalPlayMode mode)
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
      default: return "unknown";
    }//end switch
  }//end getLocalPlayModename

PlayerInfo::LocalPlayMode PlayerInfo::getLocalPlayMode() const
{
  if(teamColor == PlayerInfo::red)
  {
    switch (playMode)
    {
      case PlayerInfo::PM_BEFORE_KICK_OFF: return before_kick_off;
      case PlayerInfo::PM_KICK_OFF_LEFT: return kick_off_opp;
      case PlayerInfo::PM_KICK_OFF_RIGHT: return kick_off_own;
      case PlayerInfo::PM_PLAY_ON: return play_on;
      case PlayerInfo::PM_KICK_IN_LEFT: return kick_in_opp;
      case PlayerInfo::PM_KICK_IN_RIGHT: return kick_in_own;
      case PlayerInfo::PM_CORNER_KICK_LEFT: return corner_kick_opp;
      case PlayerInfo::PM_CORNER_KICK_RIGHT: return corner_kick_own;
      case PlayerInfo::PM_GOAL_KICK_LEFT: return goal_kick_opp;
      case PlayerInfo::PM_GOAL_KICK_RIGHT: return goal_kick_own;
      case PlayerInfo::PM_OFFSIDE_LEFT: return offside_opp;
      case PlayerInfo::PM_OFFSIDE_RIGHT: return offside_own;
      case PlayerInfo::PM_GAME_OVER: return game_over;
      case PlayerInfo::PM_GOAL_LEFT: return goal_opp;
      case PlayerInfo::PM_GOAL_RIGHT: return goal_own;
      case PlayerInfo::PM_FREE_KICK_LEFT: return free_kick_opp;
      case PlayerInfo::PM_FREE_KICK_RIGHT: return free_kick_own;
      default: return unknownLocalPlayMode;
    }//end switch
  }//end red

  switch (playMode)
  {
    case PlayerInfo::PM_BEFORE_KICK_OFF: return before_kick_off;
    case PlayerInfo::PM_KICK_OFF_LEFT: return kick_off_own;
    case PlayerInfo::PM_KICK_OFF_RIGHT: return kick_off_opp;
    case PlayerInfo::PM_PLAY_ON: return play_on;
    case PlayerInfo::PM_KICK_IN_LEFT: return kick_in_own;
    case PlayerInfo::PM_KICK_IN_RIGHT: return kick_in_opp;
    case PlayerInfo::PM_CORNER_KICK_LEFT: return corner_kick_own;
    case PlayerInfo::PM_CORNER_KICK_RIGHT: return corner_kick_opp;
    case PlayerInfo::PM_GOAL_KICK_LEFT: return goal_kick_own;
    case PlayerInfo::PM_GOAL_KICK_RIGHT: return goal_kick_opp;
    case PlayerInfo::PM_OFFSIDE_LEFT: return offside_own;
    case PlayerInfo::PM_OFFSIDE_RIGHT: return offside_opp;
    case PlayerInfo::PM_GAME_OVER: return game_over;
    case PlayerInfo::PM_GOAL_LEFT: return goal_own;
    case PlayerInfo::PM_GOAL_RIGHT: return goal_opp;
    case PlayerInfo::PM_FREE_KICK_LEFT: return free_kick_own;
    case PlayerInfo::PM_FREE_KICK_RIGHT: return free_kick_opp;
    default: return unknownLocalPlayMode;
  }//end switch
}//end getLocalPlayMode
