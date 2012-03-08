/**
* @file SoundSymbols.h
*
* Definition of class SoundSymbols
*/

#ifndef __SoundSymbols2011_H_
#define __SoundSymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

#include "Tools/Math/Common.h"
#include <math.h>

// representations
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/SoundData.h"

BEGIN_DECLARE_MODULE(SoundSymbols2011)
  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(SoundSymbols2011)

class SoundSymbols2011: public SoundSymbols2011Base
{

public:
  SoundSymbols2011()
    :
    soundData(getSoundPlayData())
  {
    theInstance = this;
  };
  ~SoundSymbols2011(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static SoundSymbols2011* theInstance;

  // representations
  naoth::SoundPlayData& soundData;

  // seter and getter
  static void setSoundRequest(int value);
  static int getSoundRequest();

};//end class SoundSymbols

#endif // __SoundSymbols2011_H_
