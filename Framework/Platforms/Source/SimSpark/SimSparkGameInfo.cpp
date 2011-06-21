/**
 * @file SimSparkGameInfo.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief the game information in SimSpark
 */

#include "SimSparkGameInfo.h"
#include <iostream>

using namespace std;
using namespace naoth;

#define STR_TI_LEFT "left"
#define STR_TI_RIGHT "right"
#define LEFT_TEAM_COLOR GameData::blue
#define RIGHT_TEAM_COLOR GameData::red

GameData::TeamColor SimSparkGameInfo::getTeamColorByName(const string& name)
{
  if ( name == STR_TI_LEFT )
  {
    return LEFT_TEAM_COLOR;
  }
  else if ( name == STR_TI_RIGHT )
  {
    return RIGHT_TEAM_COLOR;
  }
  return GameData::numOfTeamColor;
}

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

SimSparkGameInfo::PlayMode SimSparkGameInfo::getPlayModeByName(const string& name)
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

string SimSparkGameInfo::getPlayModeName(SimSparkGameInfo::PlayMode pm)
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

GameData::PlayMode SimSparkGameInfo::covertPlayMode(PlayMode pm, naoth::GameData::TeamColor team)
{
  switch (pm) {
  case PM_BEFORE_KICK_OFF:
    return GameData::before_kick_off;
  case PM_KICK_OFF_LEFT:
    return team==LEFT_TEAM_COLOR ? GameData::kick_in_own : GameData::kick_off_opp;
  case PM_KICK_OFF_RIGHT:
    return team==RIGHT_TEAM_COLOR ? GameData::kick_in_own : GameData::kick_off_opp;
  case PM_PLAY_ON:
    return GameData::play_on;
  case PM_KICK_IN_LEFT:
    return team==LEFT_TEAM_COLOR ? GameData::kick_in_own : GameData::kick_in_opp;
  case PM_KICK_IN_RIGHT:
    return team==RIGHT_TEAM_COLOR ? GameData::kick_in_own : GameData::kick_in_opp;
  case PM_CORNER_KICK_LEFT:
    return team==LEFT_TEAM_COLOR ? GameData::corner_kick_own : GameData::corner_kick_opp;
  case PM_CORNER_KICK_RIGHT:
    return team==RIGHT_TEAM_COLOR ? GameData::corner_kick_own : GameData::corner_kick_opp;
  case PM_GOAL_KICK_LEFT:
    return team==LEFT_TEAM_COLOR ? GameData::goal_kick_own : GameData::goal_kick_opp;
  case PM_GOAL_KICK_RIGHT:
    return team==RIGHT_TEAM_COLOR ? GameData::goal_kick_own : GameData::goal_kick_opp;
  case PM_OFFSIDE_LEFT:
    return team==LEFT_TEAM_COLOR ? GameData::offside_own : GameData::offside_opp;
  case PM_OFFSIDE_RIGHT:
    return team==RIGHT_TEAM_COLOR ? GameData::offside_own : GameData::offside_opp;
  case PM_GAME_OVER:
    return GameData::game_over;
  case PM_GOAL_LEFT:
    return team==LEFT_TEAM_COLOR ? GameData::goal_own : GameData::goal_opp;
  case PM_GOAL_RIGHT:
    return team==RIGHT_TEAM_COLOR ? GameData::goal_own : GameData::goal_opp;
  case PM_FREE_KICK_LEFT:
    return team==LEFT_TEAM_COLOR ? GameData::free_kick_own : GameData::free_kick_opp;
  case PM_FREE_KICK_RIGHT:
    return team==RIGHT_TEAM_COLOR ? GameData::free_kick_own : GameData::free_kick_opp;
  default: return GameData::numOfPlayMode;
  }
}
