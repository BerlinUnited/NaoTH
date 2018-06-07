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
#include "Representations/Modeling/RobotPose.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/SoundData.h"


BEGIN_DECLARE_MODULE(BDRSymbols)
  REQUIRE(FrameInfo)
  REQUIRE(TeamMessage)
  REQUIRE(RobotPose)
  PROVIDE(BDRPlayerState)
  //PROVIDE(SayRequest)         // say the message if it has changed
  PROVIDE(SoundPlayData)
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
  static void setBDRMessage(int value);
  static int  getBDRMessage();
  static void setBDRReason(int value);
  static int  getBDRReason();
  static double getNumberOfPlayersInPlay();
  static double getNumberOfPlayersInServicing();

  static double getGoalsOwn();
  static double getGoalsOpp();

};//end class BDRSymbols

#endif // _BDRSymbols_H_
