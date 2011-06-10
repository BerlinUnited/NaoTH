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
  virtual void get(FrameInfo& data);
  
  virtual void get(Image& data);
  
  virtual void get(CurrentCameraSettings& data);

  virtual void get(unsigned int& timestamp) { timestamp = libNaothDataReading->timeStamp; }

  virtual void get(SensorJointData& data) { libNaothDataReading->get(data); }

  virtual void get(AccelerometerData& data) { libNaothDataReading->get(data); }

  virtual void get(GyrometerData& data) { libNaothDataReading->get(data); }

  virtual void get(FSRData& data) { libNaothDataReading->get(data); }

  virtual void get(InertialSensorData& data) { libNaothDataReading->get(data); }

  virtual void get(IRReceiveData& data) { libNaothDataReading->get(data); }

  virtual void get(ButtonData& data) { libNaothDataReading->get(data); }

  virtual void get(BatteryData& data) { libNaothDataReading->get(data); }

  virtual void get(UltraSoundReceiveData& data) { libNaothDataReading->get(data); }
  
  virtual void get(MotorJointData& data) { libNaothDataReading->get(data); }

  /////////////////////// set ///////////////////////
  virtual void set(const CameraSettingsRequest& data);

  virtual void set(const LEDData& data) { naothDataWriting->set(data); }

  virtual void set(const IRSendData& data) { naothDataWriting->set(data); }

  virtual void set(const UltraSoundSendData& data) { naothDataWriting->set(data); }

  virtual void set(const SoundPlayData& data);

private:
  V4lCameraHandler theCameraHandler;
  SoundControl *theSoundHandler;
  
  SharedMemory<LibNaothData> libNaothData;
  const LibNaothData* libNaothDataReading;
  
  SharedMemory<NaothData> naothData;
  NaothData* naothDataWriting;
};

} // end namespace naoth
#endif	/* _NAOCONTROLLER_H */

