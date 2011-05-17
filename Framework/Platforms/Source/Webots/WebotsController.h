/* 
 * File:   WebotsController.h
 * Author: Oliver Welter
 *
 * Created on 11. Dezember 2008, 17:24
 */

#ifndef _WEBOTSCONTROLLER_H
#define	_WEBOTSCONTROLLER_H

#include <webots/robot.h>
#include <webots/camera.h>
#include <webots/touch_sensor.h>
#include <webots/gyro.h>
#include <webots/accelerometer.h>
#include <webots/pen.h>
#include <webots/servo.h>
#include <webots/led.h>
#include <webots/gps.h>

/////////////// this function comes from <device/gps.h>, which is deprecated
#ifdef __cplusplus
extern "C" {
#endif
const float *gps_get_matrix(unsigned char);
void gps_euler(const float *matrix, float *euler);
#ifdef __cplusplus
}
#endif
///////////////

#include "PlatformInterface/PlatformInterface.h"

// Representations
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

using namespace std;

namespace naoth
{

  class WebotsController : public PlatformInterface<WebotsController>
  {
  public:

    enum webots_leds
    {
      EarLeft,
      EarRight,
      FaceLeft,
      FaceRight,
      ChestButton,
      FootLeft,
      FootRight,
      numOfWebots_Leds
    };

    //private:

    WbDeviceTag camera,
    cameraSelect,
    joint[JointData::numOfJoint],
    fsr[FSRData::numOfFSR],
    leds[7],
    accelerometer,
    gyrometer,
    gps,
    bumper[BumperData::numOfBumper];

    int key;

  public:
    WebotsController();
    virtual ~WebotsController();

    virtual WebotsController& getPlatform(){return *this;}

    virtual string getHardwareIdentity() const { return "webots"; }

    virtual string getBodyID() { return "naoth-webots"; }

    virtual string getBodyNickName() {return "naoth"; }

    virtual void getCognitionInput();

    /////////////////////// init ///////////////////////
    void init();

    void main();

  public:
    /////////////////////// get ///////////////////////
    void get(AccelerometerData& data);
    void get(FrameInfo& data);
    void get(SensorJointData& data);
    void get(Image& data);
    void get(FSRData& data);
    void get(GyrometerData& data);
    void get(InertialSensorData& data);
    void get(BumperData& data);
    void get(IRReceiveData& data);
    void get(CurrentCameraSettings& data);
    void get(ButtonData& data);
    void get(BatteryData& data) { data.charge = 1.0; }
    void get(UltraSoundReceiveData& data)
    {
      // TODO
    };

    //void get(GPSData& data);

    /////////////////////// set ///////////////////////
     void set(const CameraSettingsRequest& data);
     void set(const LEDData& data);
     void set(const IRSendData& data);
     void set(const UltraSoundSendData& data);
     void set(const SoundData& data);
     void set(const MotorJointData& data);

  private:
    void get_Devices();

    void getInertialSensorData(double* data);

    void copyImage(Image& image, const unsigned char *original_image);

  //  REPRESENTATION_PROVIDER(GPSDataProvider, Cognition, GPSData);
  };
}

#endif	/* _WEBOTSCONTROLLER_H */

