/**
 * @file SimSparkGameInfo.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief the game information in SimSpark
 */

#ifndef _SIMSPARK_GAME_INFO_H
#define _SIMSPARK_GAME_INFO_H

#include <Representations/Infrastructure/GameData.h>
#include <string>

#define STR_TI_LEFT "left"
#define STR_TI_RIGHT "right"
#define LEFT_TEAM_COLOR naoth::GameData::blue
#define RIGHT_TEAM_COLOR naoth::GameData::red

class SimSparkGameInfo
{
public:

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

  static naoth::GameData::TeamColor getTeamColorBySide(const std::string& name);

  static PlayMode getPlayModeByName(const std::string& name);
  static std::string getPlayModeName(SimSparkGameInfo::PlayMode pm);
  static naoth::GameData::GameState covertToGameState(PlayMode pm);

public:
  SimSparkGameInfo() :
      valid(false),
      gameTime(0),
      teamNumber(0),
      playerNumber(0),
      // HACK: this number sed to be set by a parameter
      playersPerTeam(7),
      teamName("unknown"),
      playLeftSide(true),
      kickoff(false),
      score(0,0),
      ownPlayers(playersPerTeam),
      oppPlayers(playersPerTeam),
      gameState(naoth::GameData::unknown_game_state)
  {}

public:
  bool valid;
  unsigned int gameTime;
  unsigned int teamNumber;
  unsigned int playerNumber;
  int playersPerTeam;
  std::string teamName;
  bool playLeftSide;
  bool kickoff;
  std::pair<int, int> score;
  std::vector<naoth::GameData::RobotInfo> ownPlayers;
  std::vector<naoth::GameData::RobotInfo> oppPlayers;

  naoth::GameData::GameState gameState;


  static const unsigned int half_time = 5 * 60;

  bool firstHalf() const { return gameTime < half_time; }

  unsigned int getRemainingTimeInHalf() const {
    if ( firstHalf() ) {
      return half_time - gameTime;
    } else {
     return half_time*2 - gameTime;
    }
  }

  unsigned int getTeamIdx() const
  {
    if ( playLeftSide ) {
      return 0;
    } else {
      return 1;
    }
  }

  void updateBySideName(const std::string& name)
  {
    if ( name == STR_TI_LEFT ) {
      playLeftSide = true;
    } else if ( name == STR_TI_RIGHT ) {
      playLeftSide = false;
    } else {
      ASSERT(false);
    }
  }

  naoth::GameData::TeamColor getOwnTeamColor() const { return playLeftSide?LEFT_TEAM_COLOR:RIGHT_TEAM_COLOR; }
  naoth::GameData::TeamColor getOppTeamColor() const { return playLeftSide?RIGHT_TEAM_COLOR:LEFT_TEAM_COLOR; }

  int getOwnScore() const { return playLeftSide?score.first:score.second; }
  int getOppScore() const { return playLeftSide?score.second:score.first; }

};

#endif  /* _SIMSPARK_GAME_INFO_H */

