/**
* @file SoundSymbols2012.h
*
* Definition of class SoundSymbols2012
*/

#ifndef __SoundSymbols2012_H_
#define __SoundSymbols2012_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Common.h"
#include <math.h>

// representations
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/SoundData.h"

BEGIN_DECLARE_MODULE(SoundSymbols2012)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(SoundSymbols2012)

class SoundSymbols2012: public SoundSymbols2012Base
{

public:
  SoundSymbols2012()
    :
    soundData(getSoundPlayData())
  {
    theInstance = this;
  };
  virtual ~SoundSymbols2012(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static SoundSymbols2012* theInstance;

  // representations
  naoth::SoundPlayData& soundData;

  // seter and getter
  static void setSoundRequest(int value);
  static int getSoundRequest();

};//end class SoundSymbols2012

#endif // __SoundSymbols2012_H_
