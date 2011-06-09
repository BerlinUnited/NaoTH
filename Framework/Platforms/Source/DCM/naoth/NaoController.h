/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot
 *
 */

#ifndef _NAOCONTROLLER_H
#define	_NAOCONTROLLER_H

#include "SoundControl.h"
#include "PlatformInterface/PlatformInterface.h"
#include "V4lCameraHandler.h"
#include "Tools/IPCData.h"
#include "Tools/SharedMemory.h"

#include <Representations/Infrastructure/FrameInfo.h>

namespace naoth
{
class NaoController : public PlatformInterface<NaoController>
{
public:
  NaoController();
  virtual ~NaoController();

  virtual string getHardwareIdentity() const;

  virtual string getBodyID() const;

  virtual string getBodyNickName() const;

public:
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
  
  virtual void get(MotorJointData& data);

  /////////////////////// set ///////////////////////
  virtual void set(const CameraSettingsRequest& data);

  virtual void set(const LEDData& data);

  virtual void set(const IRSendData& data);

  virtual void set(const UltraSoundSendData& data);

  virtual void set(const SoundPlayData& data);

private:
  V4lCameraHandler theCameraHandler;
  SoundControl *theSoundHandler;
  
  SharedMemory<LibNaothData> libNaothData;
  SharedMemory<NaothData> naothData;
  LEDData* theLEDData;
  IRSendData* theIRSendData;
  UltraSoundSendData* theUltraSoundSendData;
};

} // end namespace naoth
#endif	/* _NAOCONTROLLER_H */

