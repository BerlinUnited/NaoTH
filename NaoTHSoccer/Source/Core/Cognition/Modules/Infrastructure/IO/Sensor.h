/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _SENSOR_H
#define	_SENSOR_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>
//#include "Representations/SwapSpace.h"

// input representations
// sensors
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/BumperData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
// others
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/CameraSettings.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(Sensor)
  PROVIDE(AccelerometerData)
  PROVIDE(BatteryData)
  PROVIDE(BumperData)
  PROVIDE(ButtonData)
  PROVIDE(GyrometerData)
  PROVIDE(IRReceiveData)
  PROVIDE(Image)
  PROVIDE(InertialSensorData)
  PROVIDE(FSRData)
  PROVIDE(SensorJointData)
  PROVIDE(UltraSoundReceiveData)
  
  PROVIDE(FrameInfo)
  PROVIDE(CurrentCameraSettings)
END_DECLARE_MODULE(Sensor)

class Sensor : public SensorBase
{
public:
  Sensor();
  ~Sensor();

  virtual void execute();

  void init(naoth::PlatformDataInterface& platformInterface);
};

#endif	/* _SENSOR_H */

