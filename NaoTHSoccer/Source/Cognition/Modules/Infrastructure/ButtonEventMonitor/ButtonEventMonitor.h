/**
 * @file ButtonEventMonitor.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef _ButtonEventMonitor_H
#define _ButtonEventMonitor_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/ButtonState.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(ButtonEventMonitor)
  REQUIRE(FrameInfo)
  REQUIRE(ButtonData)

  PROVIDE(ButtonState)
END_DECLARE_MODULE(ButtonEventMonitor)

class ButtonEventMonitor : public ButtonEventMonitorBase
{
public:
  ButtonEventMonitor(){}
  virtual ~ButtonEventMonitor(){}

  virtual void execute();

private:
  void update(ButtonEvent& buttonEvent, bool pressed);
};

#endif  /* _ButtonEventMonitor_H */

