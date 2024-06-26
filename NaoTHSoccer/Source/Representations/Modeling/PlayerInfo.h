
#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <string>

#include "Representations/Infrastructure/GameData.h"
#include "Tools/DataStructures/Printable.h"
#include <Tools/Debug/NaoTHAssert.h>

typedef unsigned int PlayerNumber;

/** Any relevant information about the game state for the player */
class PlayerInfo : public naoth::Printable
{
public:

  // ACHTUNG: the RobotState is not the same as GameState:
  //   - GameState is indicating the overall state of the game;
  //   - RobotState reflects the state of the current robot;
  // 
  //   It's defined according to the Rulebook Chapter 3 "Game Process" 
  //   (as of 25.05.2016)
  // TODO: maybe rename to RobotGameState
  enum RobotState 
  {
    initial   = naoth::GameData::initial,
    ready     = naoth::GameData::ready,
    set       = naoth::GameData::set,
    playing   = naoth::GameData::playing,
    finished  = naoth::GameData::finished,
    penalized,  // <== GameState doesn't have this :)
    unstiff     // <== GameState doesn't have this :)
  };

  // Same as SetPlay of the GameData, but can be different for each robot and set (e.g.) via DebugRequest
  enum RobotSetPlay
  {
    set_none          = naoth::GameData::set_none,
    goal_kick         = naoth::GameData::goal_kick,
    pushing_free_kick = naoth::GameData::pushing_free_kick,
    corner_kick       = naoth::GameData::corner_kick,
    kick_in           = naoth::GameData::kick_in,
    penalty_kick      = naoth::GameData::penalty_kick
  };

  enum GamePhase
  {
    normal            = naoth::GameData::normal,
    penaltyshoot      = naoth::GameData::penaltyshoot,
    overtime          = naoth::GameData::overtime,
    timeout           = naoth::GameData::timeout
  };

  // update from GameData (usually it was received from the game controller)
  void update(const naoth::GameData& gameData) 
  {
    // update only if player number is set correctly
    ASSERT(playerNumber > 0);

    naoth::GameData::RobotInfo penaltyInfo = gameData.getOwnRobotInfo(playerNumber);

    if(penaltyInfo.penalty == naoth::GameData::penalty_none) {
      robotState = (RobotState)gameData.gameState;
    } else {
      robotState = RobotState::penalized;
    }

    // state of the game
    robotSetPlay = static_cast<RobotSetPlay>(gameData.setPlay);
    gamePhase    = static_cast<GamePhase>(gameData.gamePhase);
    kickoff      = (gameData.kickingTeam == teamNumber);

    // this is usually set only once on start of the game controller
    teamColor      = gameData.ownTeam.teamColor;
    playersPerTeam = gameData.playersPerTeam;
  }

  PlayerInfo();
  ~PlayerInfo() {}
  
  // set from config and does not change after start of the binary
  unsigned int playerNumber;
  unsigned int teamNumber;

  // this is only used by simulation (e.g., SimSpark)
  std::string teamName;

  //
  naoth::GameData::TeamColor teamColor;
  unsigned int playersPerTeam;

  // state of the game and internal state of the robot. 
  // set by game controller
  RobotState robotState;
  RobotSetPlay robotSetPlay;
  GamePhase gamePhase;
  bool kickoff;

  // TODO: move somewhere else (it's a strategic decision)?
  /** Whether the behavior decided to play as striker */
  bool isPlayingStriker;

  /** for debug reasons: the currently used scheme. */
  // TODO: move this to a separate representation 
  std::string scheme;

  static std::string toString(RobotState value);
  static std::string toString(RobotSetPlay value);
  static std::string toString(GamePhase value);

  virtual void print(std::ostream& stream) const;
};

#endif  // PLAYER_INFO_H

