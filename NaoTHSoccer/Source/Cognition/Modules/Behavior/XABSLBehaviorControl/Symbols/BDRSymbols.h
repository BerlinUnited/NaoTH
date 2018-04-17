/**
* @file BDRSymbols.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BDRSymbols
*/

#ifndef _BDRSymbols_H_
#define _BDRSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Modeling/BDRPlayerState.h"
#include "Representations/Modeling/TeamMessage.h"

BEGIN_DECLARE_MODULE(BDRSymbols)
  REQUIRE(TeamMessage)
  PROVIDE(BDRPlayerState)
END_DECLARE_MODULE(BDRSymbols)

class BDRSymbols: public BDRSymbolsBase
{

public:
  BDRSymbols() {
    theInstance = this;
  }
  virtual ~BDRSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute(){}

private:
  static BDRSymbols* theInstance;

  static void setBDRActivity(int value);
  static int  getBDRActivity();
  static double getNumberOfPlayersInPlay();

};//end class BDRSymbols

#endif // _BDRSymbols_H_
