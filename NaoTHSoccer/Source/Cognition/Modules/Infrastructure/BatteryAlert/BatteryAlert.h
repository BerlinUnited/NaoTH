/**
 * @file BatteryAlert.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 */

#ifndef _BATTERYALERT_H
#define _BATTERYALERT_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/BatteryData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Modeling/BodyState.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(BatteryAlert)
  REQUIRE(FrameInfo)
  REQUIRE(BatteryData)
  REQUIRE(BodyState)

  PROVIDE(SoundPlayData)
END_DECLARE_MODULE(BatteryAlert)

class BatteryAlert : public BatteryAlertBase
{
public:
  BatteryAlert();
  virtual ~BatteryAlert(){};

  virtual void execute();
  
private:
  unsigned int timeWhenLastPlayedSound;
};

#endif  /* _BATTERYALERT_H */

