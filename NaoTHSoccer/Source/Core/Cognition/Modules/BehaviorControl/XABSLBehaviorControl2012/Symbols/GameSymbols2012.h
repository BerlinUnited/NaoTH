/**
* @file GameSymbols2012.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GameSymbols2012
*/

#ifndef __GameSymbols2012_H_
#define __GameSymbols2012_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/PlayerInfo.h"

BEGIN_DECLARE_MODULE(GameSymbols2012)
  REQUIRE(PlayerInfo);
END_DECLARE_MODULE(GameSymbols2012)

class GameSymbols2012: public GameSymbols2012Base
{

public:
  GameSymbols2012()
    :
    playerInfo(getPlayerInfo())
  {
      theInstance = this;
    };
  virtual ~GameSymbols2012(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static GameSymbols2012* theInstance;
  static bool getOwnKickOff();
  static double getPlayerNumber();
  static double getMsecsRemaining();
  static int getPlayMode();
  static int getOpponentTeamColor();
  static int getOwnTeamColor();

  const PlayerInfo& playerInfo;

};//end class GameSymbols2012

#endif // __GameSymbols2012_H_
