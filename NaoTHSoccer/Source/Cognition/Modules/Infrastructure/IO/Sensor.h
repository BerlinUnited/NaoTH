/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _SENSOR_H
#define _SENSOR_H

#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <PlatformInterface/PlatformInterface.h>

// input representations
// sensors
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/BatteryData.h>
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
#include <Representations/Infrastructure/RobotInfo.h>
#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/DebugMessage.h>
//#include "Representations/Infrastructure/CalibrationData.h"

#include "Representations/Infrastructure/GPSData.h"
//#include "Representations/Infrastructure/SerialSensorData.h"
//#include "Representations/Motion/MotionStatus.h"
//#include "Representations/Modeling/OdometryData.h"
#include <Representations/Modeling/InertialModel.h>
//#include <Representations/Perception/CameraMatrix.h>
//#include <Representations/Perception/CameraMatrixBuffer.h>

// others
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/CameraSettings.h>

using namespace naoth;

BEGIN_DECLARE_MODULE(Sensor)
  PROVIDE(FrameInfo)
  
  //PROVIDE(SerialSensorData)
  PROVIDE(Image)
  PROVIDE(ImageTop)
  PROVIDE(FSRData)
  PROVIDE(AccelerometerData)
  PROVIDE(GyrometerData)
  PROVIDE(InertialSensorData)
  PROVIDE(SensorJointData)
  PROVIDE(IRReceiveData)
  PROVIDE(CurrentCameraSettings)
  PROVIDE(CurrentCameraSettingsTop)
  PROVIDE(ButtonData)
  PROVIDE(BatteryData)
  PROVIDE(UltraSoundReceiveData)
  PROVIDE(SoundCaptureData)

  PROVIDE(TeamMessageDataIn)

  PROVIDE(GPSData)
  PROVIDE(GameData)
  PROVIDE(RobotInfo)
  PROVIDE(VirtualVision)
  PROVIDE(VirtualVisionTop)
  PROVIDE(DebugMessageInCognition)

  // from motion
  //PROVIDE(MotionStatus)
  //PROVIDE(OdometryData)
  PROVIDE(InertialModel)
  //PROVIDE(CameraMatrixBuffer)
  //PROVIDE(CameraMatrixBuffer2)
  //PROVIDE(CalibrationData)
  
END_DECLARE_MODULE(Sensor)

class Sensor : public SensorBase
{
public:
  Sensor();
  virtual ~Sensor();

  virtual void execute();

  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);
};

#endif  /* _SENSOR_H */

