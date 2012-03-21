/**
* @file GameSymbols.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GameSymbols
*/

#ifndef __GameSymbols2011_H_
#define __GameSymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Modeling/PlayerInfo.h"

BEGIN_DECLARE_MODULE(GameSymbols2011)
  REQUIRE(PlayerInfo);
END_DECLARE_MODULE(GameSymbols2011)

class GameSymbols2011: public GameSymbols2011Base
{

public:
  GameSymbols2011()
    :
    playerInfo(getPlayerInfo())
  {
      theInstance = this;
    };
  ~GameSymbols2011(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static GameSymbols2011* theInstance;
  static bool getOwnKickOff();
  static double getPlayerNumber();
  static int getPlayMode();
  static int getOpponentTeamColor();
  static int getOwnTeamColor();

  const PlayerInfo& playerInfo;

};//end class GameSymbols

#endif // __GameSymbols2011_H_
