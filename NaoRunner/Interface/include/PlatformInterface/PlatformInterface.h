/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _PLATFORMINTERFACE_H
#define	_PLATFORMINTERFACE_H

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/BumperData.h"
#include "Representations/Infrastructure/IRData.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/BatteryData.h"
#include "PlatformInterchangeable.h"
#include "Callable.h"

/*  the platform interface responses for 4 kinds of functionalities:
 * - get sensor data
 * - set action data
 * - initialize the cognition module and motion module
 * - main loop to call cognition and motion
 */
class PlatformInterface
{
public:
  virtual ~PlatformInterface();

  virtual void getCognitionInput();

  virtual void getMotionInput();

  virtual bool registerCognitionInput(PlatformInterchangeable* data , const std::string& name);

  virtual bool registerMotionInput(PlatformInterchangeable* data, const std::string& name);

  virtual void setCognitionOutput();

  virtual void setMotionOutput();

  virtual bool registerCognitionOutput(const PlatformInterchangeable* data, const std::string& name);

  virtual bool registerMotionOutput(const PlatformInterchangeable* data, const std::string& name);

  //////// register own main loop callbacks /////////

  virtual void registerCallbacks(Callable* motionCallback, Callable* cognitionCallback);

  /////////////////////// get ///////////////////////
  unsigned int getBasicTimeStep() const { return theBasicTimeStep; }

  virtual string getHardwareIdentity() const = 0;

  virtual string getBodyID() = 0;

  virtual string getBodyNickName() = 0;

  const string& getName() const { return platformName; }

  //////////////////// update data /////////////////////
  virtual void updateData();

protected:
    /////////////////////// get ///////////////////////
  virtual void get(AccelerometerData& data) = 0;

  virtual void get(FrameInfo& data) = 0;

  virtual void get(SensorJointData& data) = 0;

  virtual void get(Image& data) = 0;

  virtual void get(FSRData& data) = 0;

  virtual void get(GyrometerData& data) = 0;

  virtual void get(InertialSensorData& data) = 0;

  virtual void get(BumperData& data) = 0;

  virtual void get(IRReceiveData& data) = 0;

  virtual void get(CurrentCameraSettings& data) = 0;

  virtual void get(ButtonData& data) = 0;

  virtual void get(BatteryData& data) = 0;

  virtual void get(UltraSoundReceiveData& data) = 0;

  /////////////////////// set ///////////////////////
  virtual void set(const CameraSettingsRequest& data) = 0;

  virtual void set(const LEDData& data) = 0;

  virtual void set(const IRSendData& data) = 0;

  virtual void set(const UltraSoundSendData& data) = 0;

  virtual void set(const SoundData& data) = 0;

  virtual void set(const MotorJointData& data) = 0;

protected:
  PlatformInterface(const std::string& name, unsigned int basicTimeStep);

  std::string platformName;

  unsigned int theBasicTimeStep;

  struct {
    SensorJointData* theSensorJointData;
    FrameInfo* theFrameInfo;
    AccelerometerData* theAccelerometerData;
    Image* theImage;
    FSRData* theFSRData;
    GyrometerData* theGyrometerData;
    InertialSensorData* theInertialSensorData;
    BumperData* theBumperData;
    IRReceiveData* theIRReceiveData;
    CurrentCameraSettings* theCurrentCameraSettings;
    ButtonData* theButtonData;
    BatteryData* theBatteryData;
    UltraSoundReceiveData* theUltraSoundReceiveData;

    const CameraSettingsRequest* theCameraSettingsRequest;
    const LEDData* theLEDData;
    const IRSendData* theIRSendData;
    const UltraSoundSendData* theUltraSoundSendData;
    const SoundData* theSoundData;
  } theCognition;

  struct {
    FrameInfo* theFrameInfo;
    SensorJointData* theSensorJointData;
    AccelerometerData* theAccelerometerData;
    FSRData* theFSRData;
    InertialSensorData* theInertialSensorData;
    BumperData* theBumperData;
    GyrometerData* theGyrometerData;

    const MotorJointData* theMotorJointData;
  } theMotion;

  Callable* motionCallback;
  Callable* cognitionCallback;
  
private:
  unsigned int lastUltraSoundSendTime;
  unsigned int lastUltraSoundReceiveTime;
};

#endif	/* _PLATFORMINTERFACE_H */

