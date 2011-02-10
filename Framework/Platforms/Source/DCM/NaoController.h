/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot
 *
 */

#ifndef _NAOCONTROLLER_H
#define	_NAOCONTROLLER_H

#include "naoth.h"
#include "DCMHandler.h"
#include "SoundControl.h"
#include "PlatformInterface/PlatformInterface.h"
#include "V4lCameraHandler.h"

#include <Representations/Infrastructure/FrameInfo.h>
#include <Tools/DataStructures/Singleton.h>

namespace naoth
{
class NaoController : public PlatformInterface<NaoController>, public Singleton<NaoController>
{
public:
  NaoController();

  virtual ~NaoController();

  virtual string getHardwareIdentity() const;

  virtual string getBodyID();

  virtual string getBodyNickName();

  /////////////////////// init ///////////////////////
  void init(ALPtr<ALBroker> pB);

  /////////////////////// run ///////////////////////
  virtual void run();

  virtual void close();

protected:
  virtual void get(unsigned int& timestamp);

  virtual void get(FrameInfo& data);

  virtual void get(SensorJointData& data);

  virtual void get(AccelerometerData& data);

  virtual void get(Image& data);

  virtual void get(GyrometerData& data);

  virtual void get(FSRData& data);

  virtual void get(InertialSensorData& data);

  virtual void get(IRReceiveData& data);

  virtual void get(CurrentCameraSettings& data);

  virtual void get(ButtonData& data);

  virtual void get(BatteryData& data);

  virtual void get(UltraSoundReceiveData& data);

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& data);

  virtual void set(const CameraSettingsRequest& data);

  virtual void set(const LEDData& data);

  virtual void set(const IRSendData& data);

  virtual void set(const UltraSoundSendData& data);

  virtual void set(const SoundData& data);

private:
  DCMHandler theDCMHandler;
  V4lCameraHandler theCameraHandler;
  SoundControl *theSoundHandler;
};

} // end namespace naoth
#endif	/* _NAOCONTROLLER_H */

