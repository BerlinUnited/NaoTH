
#ifndef __PlayerInfo_h_
#define  __PlayerInfo_h_

#include <string>

#include "Representations/Infrastructure/GameData.h"
#include "Tools/DataStructures/Printable.h"

/** Any relevant information about the game state for the player */
class PlayerInfo : public naoth::Printable
{
  
public:

  // the representation of the game mode of Simsparc in xabsl


  PlayerInfo();
  //PlayerInfo(unsigned int playerNumber, naoth::GameData::TeamColor teamColor, unsigned int teamNumber, bool isPlayingStriker);
  ~PlayerInfo();
  
  naoth::GameData gameData;

  /** Whether the behavior decided to play as striker */
  bool isPlayingStriker;

  virtual void print(ostream& stream) const;

};

#endif  // __PlayerInfo_h_

