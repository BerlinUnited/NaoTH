/**
* @file GameSymbols.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GameSymbols
*/

#ifndef _GameSymbols_H_
#define _GameSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/PlayerInfo.h"

BEGIN_DECLARE_MODULE(GameSymbols)
  REQUIRE(PlayerInfo);
END_DECLARE_MODULE(GameSymbols)

class GameSymbols: public GameSymbolsBase
{

public:
  GameSymbols() {
    theInstance = this;
  }
  virtual ~GameSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute(){}

private:
  static GameSymbols* theInstance;

  static bool getOwnKickOff();
  static double getPlayerNumber();
  static double getMsecsRemaining();
  static int getGameState();
  static int getPlayMode();
  static int getOpponentTeamColor();
  static int getOwnTeamColor();

};//end class GameSymbols

#endif // _GameSymbols_H_
