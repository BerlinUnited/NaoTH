/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _ACTUATOR_H
#define	_ACTUATOR_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>
//#include "Representations/SwapSpace.h"


#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/SoundData.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(Actuator)
  USE(CameraSettingsRequest)
  USE(LEDData)
  USE(IRSendData)
  USE(UltraSoundSendData)
  USE(SoundData)
END_DECLARE_MODULE(Actuator)

class Actuator : public ActuatorBase
{
public:
  Actuator();
  ~Actuator();

  virtual void execute();

  void init(naoth::PlatformDataInterface& platformInterface);
};

#endif	/* _ACTUACTOR_H */

