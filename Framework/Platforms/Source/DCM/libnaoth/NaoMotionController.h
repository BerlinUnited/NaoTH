/**
 * @file NaoMotionController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot
 *
 */

#ifndef _NAO_MOTION_CONTROLLER_H
#define	_NAO_MOTION_CONTROLLER_H

#include "DCMHandler.h"
#include "PlatformInterface/PlatformInterface.h"

#include <Representations/Infrastructure/FrameInfo.h>
#include <Tools/DataStructures/Singleton.h>

namespace naoth
{
class NaoMotionController : public PlatformInterface<NaoMotionController>, public Singleton<NaoMotionController>
{
protected:
  friend class Singleton<NaoMotionController>;
  NaoMotionController();
  virtual ~NaoMotionController();
  
public:

  virtual string getHardwareIdentity() const;

  virtual string getBodyID() const { return libNaothDataReading->getBodyID(); }

  virtual string getBodyNickName() const { return libNaothDataReading->getBodyID(); }

  /////////////////////// init ///////////////////////
  void init(ALPtr<ALBroker> pB);

  /////////////////////// run ///////////////////////
  void updateSensorData();
  void setActuatorData();

public:
  

  virtual void get(FrameInfo& data);
  
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

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& data);

private:
  DCMHandler theDCMHandler;
  
  SharedMemory<LibNaothData> libNaothData;
  const LibNaothData* libNaothDataReading;
  
  SharedMemory<NaothData> naothData;
};

} // end namespace naoth
#endif	/* _NAO_MOTION_CONTROLLER_H */

