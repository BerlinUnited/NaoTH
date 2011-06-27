
#ifndef __PlayerInfo_h_
#define  __PlayerInfo_h_

#include <string>

#include "Representations/Infrastructure/GameData.h"
#include "Tools/DataStructures/Printable.h"

/** Any relevant information about the game state for the player */
class PlayerInfo : public naoth::Printable
{
public:

  enum TeamColor
  {
    red,
    blue,
    numOfTeamColor
  };

  PlayerInfo();
  //PlayerInfo(unsigned int playerNumber, naoth::GameData::TeamColor teamColor, unsigned int teamNumber, bool isPlayingStriker);
  ~PlayerInfo();
  
  /** */
  naoth::GameData gameData;

  /** */
  unsigned int timeSincePlayModeChanged; // ms

  /** */
  unsigned int timeSinceGameStateChanged; // ms

  /** Whether the behavior decided to play as striker */
  bool isPlayingStriker;

  virtual void print(ostream& stream) const;

};

#endif  // __PlayerInfo_h_

