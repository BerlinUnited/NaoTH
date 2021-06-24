/**
* @file SoundSymbols.h
*
* Definition of class SoundSymbols
*/

#ifndef SoundSymbols_H
#define SoundSymbols_H

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Common.h"

// representations
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/SoundData.h"

#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotPose.h"

BEGIN_DECLARE_MODULE(SoundSymbols)
  REQUIRE(BallModel)

  REQUIRE(RobotPose)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(SoundSymbols)

class SoundSymbols: public SoundSymbolsBase
{

public:
  SoundSymbols()
  {
    theInstance = this;
  }
  virtual ~SoundSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static SoundSymbols* theInstance;

  // seter and getter
  static void setSoundRequest(int value);
  static int getSoundRequest();

};//end class SoundSymbols

#endif // __SoundSymbols_H_
