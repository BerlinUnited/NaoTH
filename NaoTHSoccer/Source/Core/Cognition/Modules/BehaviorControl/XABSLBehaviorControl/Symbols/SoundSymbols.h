/**
* @file SoundSymbols.h
*
* Definition of class SoundSymbols
*/

#ifndef __SoundSymbols_H_
#define __SoundSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Common.h"
#include <math.h>

// representations
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/SoundData.h"

BEGIN_DECLARE_MODULE(SoundSymbols)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(SoundSymbols)

class SoundSymbols: public SoundSymbolsBase
{

public:
  SoundSymbols()
    :
    soundData(getSoundPlayData())
  {
    theInstance = this;
  };
  virtual ~SoundSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static SoundSymbols* theInstance;

  // representations
  naoth::SoundPlayData& soundData;

  // seter and getter
  static void setSoundRequest(int value);
  static int getSoundRequest();

};//end class SoundSymbols

#endif // __SoundSymbols_H_
