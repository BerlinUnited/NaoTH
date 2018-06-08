
#ifndef _WifiModeSetter_H
#define _WifiModeSetter_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/ButtonState.h"
#include "Representations/Infrastructure/WifiMode.h"
#include <Representations/Infrastructure/LEDRequest.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(WifiModeSetter)
  REQUIRE(FrameInfo)
  REQUIRE(ButtonState)
  REQUIRE(LEDData)

  PROVIDE(WifiMode)
  PROVIDE(WifiModeSetterLEDRequest)

END_DECLARE_MODULE(WifiModeSetter)

class WifiModeSetter : public WifiModeSetterBase
{
public:
  WifiModeSetter() : frameWhenColorChanged(0) {}
  virtual ~WifiModeSetter(){}

  virtual void execute();

private:

  unsigned int frameWhenColorChanged;
};

#endif  /* _WifiModeSetter_H */

