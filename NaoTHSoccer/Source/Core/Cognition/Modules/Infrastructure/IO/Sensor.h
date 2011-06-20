/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _SENSOR_H
#define	_SENSOR_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>

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
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/GameData.h>

#include "Representations/Infrastructure/SerialSensorData.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/OdometryData.h"

// others
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/CameraSettings.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(Sensor)
  PROVIDE(FrameInfo)
  PROVIDE(SerialSensorData)
  PROVIDE(Image)
  PROVIDE(FSRData)
  PROVIDE(AccelerometerData)
  PROVIDE(GyrometerData)
  PROVIDE(InertialSensorData)
  PROVIDE(SensorJointData)
  PROVIDE(BumperData)
  PROVIDE(IRReceiveData)
  PROVIDE(MotionStatus)
  PROVIDE(OdometryData)
  PROVIDE(CurrentCameraSettings)
  PROVIDE(ButtonData)
  PROVIDE(BatteryData)
  PROVIDE(UltraSoundReceiveData)
  PROVIDE(SoundCaptureData)
  PROVIDE(TeamMessageData)
  PROVIDE(GameData)
END_DECLARE_MODULE(Sensor)

class Sensor : public SensorBase
{
public:
  Sensor();
  ~Sensor();

  virtual void execute();

  void init(naoth::PlatformInterfaceBase& platformInterface);
  
private:
  MessageReader* theMotionStatusReader;
  MessageReader* theOdometryDataReader;
};

#endif	/* _SENSOR_H */

