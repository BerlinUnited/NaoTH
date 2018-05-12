
#ifndef _PlayerInfo_h_
#define _PlayerInfo_h_

#include <string>

#include "Representations/Infrastructure/GameData.h"
#include "Tools/DataStructures/Printable.h"
#include <Tools/Debug/NaoTHAssert.h>

/** Any relevant information about the game state for the player */
class PlayerInfo : public naoth::Printable
{
public:

  // ACHTUNG: the RobotState is not the same as GameState, 
  //          as GameState is indicating the overall state of the game whereas
  //          RobotState reflects the state of the current robot.
  //          It's defined according to the Rulebook Chapter 3 "Game Process" 
  //          (as of 25.05.2016)
  enum RobotState 
  {
    initial   = naoth::GameData::initial,
    ready     = naoth::GameData::ready,
    set       = naoth::GameData::set,
    playing   = naoth::GameData::playing,
    finished  = naoth::GameData::finished,
    penalized  // <== GameState doesn't have this :)
  };

  void update(const naoth::GameData& gameData) 
  {
    // update only if player number is set correctly
    ASSERT(playerNumber > 0);

    naoth::GameData::RobotInfo penaltyInfo = gameData.getOwnRobotInfo(playerNumber);

    if(penaltyInfo.penalty != naoth::GameData::penalty_none) {
      robotState = penalized;
    } else {
      robotState = (RobotState)gameData.gameState;
    }

    teamColor = gameData.ownTeam.teamColor;
    kickoff = (gameData.kickingTeam == teamNumber);
    playersPerTeam = gameData.playersPerTeam;
  }

  PlayerInfo();
  ~PlayerInfo();
  
  // set from config
  unsigned int playersPerTeam; // number of players in the team
  unsigned int playerNumber;
  unsigned int teamNumber;
  std::string teamName;

  // set by game controller
  naoth::GameData::TeamColor teamColor;
  bool kickoff;
  RobotState robotState;

  // TODO: move somewhere else (it's a strategic decision)?
  /** Whether the behavior decided to play as striker */
  bool isPlayingStriker;

  /** the currently used scheme. */
  std::string scheme;

  bool isGoalie() const {
      return playerNumber == 1;
  }

  static std::string toString(RobotState value);

  virtual void print(std::ostream& stream) const;
};

#endif  // _PlayerInfo_h_

