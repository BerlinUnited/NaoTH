// 
// File:   RobotInfo.h
// Author: thomas
//
// Created on 12. März 2008, 13:40
//

#ifndef __PlayerInfo_h_
#define  __PlayerInfo_h_

#include <string>

#include "Tools/DataStructures/Printable.h"

/** Any relevant information about the game state for the player */
class PlayerInfo : public naoth::Printable
{
  
public:  
  enum GameState
  {
    inital,
    ready,
    set,
    playing,
    penalized,
    finished,
    unknown,
    numOfGameState
  };

  enum PlayMode
  {
    // the order of the first 3 play modes should not be changed.
    PM_BEFORE_KICK_OFF = 0, /*!< before_kick_off:   before the match  */
    PM_KICK_OFF_LEFT = 1, /*!< kick_off_left:     kick off for the left team  */
    PM_KICK_OFF_RIGHT = 2, /*!< kick_off_right:    kick off for the right team */
    PM_PLAY_ON, /*!< play_on:           regular game play */
    PM_KICK_IN_LEFT,
    PM_KICK_IN_RIGHT,
    PM_CORNER_KICK_LEFT, /*!< corner_kick_l:     corner kick left team   */
    PM_CORNER_KICK_RIGHT, /*!< corner_kick_r:     corner kick right team  */
    PM_GOAL_KICK_LEFT, /*!< goal_kick_l:       goal kick for left team */
    PM_GOAL_KICK_RIGHT, /*!< goal_kick_r:       goal kick for right team*/
    PM_OFFSIDE_LEFT, /*!< offside_l:         offside for left team   */
    PM_OFFSIDE_RIGHT, /*!< offside_r:         offside for right team  */
    PM_GAME_OVER,
    PM_GOAL_LEFT,
    PM_GOAL_RIGHT,
    PM_FREE_KICK_LEFT, /*!< free_kick_l:       free kick for left team */
    PM_FREE_KICK_RIGHT, /*!< free_kick_r:       free kick for right team*/
    numOfPlayMode, /*!< no play mode, this must be the last entry */
  };

  enum TeamColor
  {
    red,
    blue,
    numOfTeamColor
  };

  // the representation of the game mode of Simsparc in xabsl
  enum LocalPlayMode
  {
    before_kick_off,
    kick_off_own,
    kick_off_opp,
    play_on,
    kick_in_own,
    kick_in_opp,
    corner_kick_own,
    corner_kick_opp,
    goal_kick_own,
    goal_kick_opp,
    offside_own,
    offside_opp,
    game_over,
    goal_own,
    goal_opp,
    free_kick_own,
    free_kick_opp,
    unknownLocalPlayMode,
    numOfLocalPlayMode
  };

  PlayerInfo();
  PlayerInfo(unsigned char playerNumber, TeamColor teamColor, unsigned char teamNumber, bool isPlayingStriker);
  ~PlayerInfo();

  /** Personal game state */
  GameState gameState;

  /** play mode */
  PlayMode playMode;

  /** game play mode */
  LocalPlayMode localPlayMode;
  
  /** player number from 0 to 255. 255 means unknown */
  unsigned char playerNumber;
  
  /** the color of the current team */
  TeamColor teamColor;

  /** current team number (needed for the teamcomm */
  unsigned char teamNumber;

  /** True if we had kickoff */
  bool ownKickOff;

  /** number of player per team */
  unsigned int numOfPlayers;

  /** Whether the behavior decided to play as striker */
  bool isPlayingStriker;

  static std::string getLocalPlayModename(PlayerInfo::LocalPlayMode mode);
  LocalPlayMode getLocalPlayMode() const;

  static TeamColor teamColorFromString(std::string teamColor);
  static std::string teamColorToString(TeamColor teamColor);

  static PlayerInfo::GameState gameStateFromString(std::string gameState);
  static std::string gameStateToString(PlayerInfo::GameState gameState);

  static PlayMode getPlayModeByName(const std::string& name);
  static std::string getPlayModeName(PlayMode pm);

  virtual void print(ostream& stream) const
  {
    stream << "gameState = " << gameStateToString(gameState) << "\n";
    stream << "PlayMode = " << getPlayModeName(playMode) <<"\n";
    stream << "LocalPlayMode = " << getLocalPlayModename(getLocalPlayMode()) <<"\n";
    stream << "playerNumber = " << (int)playerNumber << "\n";
    stream << "teamColor = " << teamColorToString(teamColor) << "\n";
    stream << "teamNumber = " << (int)teamNumber << "\n";
    stream << "ownKickOff = " << (ownKickOff?"yes":"no") << "\n";
    stream << "numOfPlayers = " << (int)numOfPlayers << "\n";
    stream << "isPlayingStriker = " << (isPlayingStriker?"yes":"no") << endl;
  }//end print

};

/**
 * the Operator ! inverts the team color, i.e.,
 * !red = blue, !blue = red, !unknownTeamColor = unknownTeamColor
 * e.g., the color of the opponent team is !thePlayerInfo.teamColor 
 */
PlayerInfo::TeamColor operator! (const PlayerInfo::TeamColor& color);

#endif  // __PlayerInfo_h_

