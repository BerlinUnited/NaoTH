/**
 * @file SimSparkGameInfo.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief the game information in SimSpark
 */

#ifndef _SIMSPARK_GAME_INFO_H
#define  _SIMSPARK_GAME_INFO_H

#include <Representations/Infrastructure/GameData.h>
#include <string>

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

  static naoth::GameData::TeamColor getTeamColorByName(const std::string& name);

  static PlayMode getPlayModeByName(const std::string& name);
  static std::string getPlayModeName(SimSparkGameInfo::PlayMode pm);

  static naoth::GameData::PlayMode covertPlayMode(PlayMode pm, naoth::GameData::TeamColor team);
};

#endif  /* _SIMSPARK_GAME_INFO_H */

