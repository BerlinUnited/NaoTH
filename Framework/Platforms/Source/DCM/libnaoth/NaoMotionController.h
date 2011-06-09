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

  virtual string getBodyID();

  virtual string getBodyNickName();

  /////////////////////// init ///////////////////////
  void init(ALPtr<ALBroker> pB);

  /////////////////////// run ///////////////////////
  void updateSensorData();
  void setActuatorData();

public:
  virtual void get(unsigned int& timestamp);

  virtual void get(FrameInfo& data);

  virtual void get(SensorJointData& data);

  virtual void get(AccelerometerData& data);

  virtual void get(GyrometerData& data);

  virtual void get(FSRData& data);

  virtual void get(InertialSensorData& data);

  virtual void get(IRReceiveData& data);

  virtual void get(ButtonData& data);

  virtual void get(BatteryData& data);

  virtual void get(UltraSoundReceiveData& data);

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& data);

  virtual void set(const LEDData& data);

  virtual void set(const IRSendData& data);

  virtual void set(const UltraSoundSendData& data);

private:
  DCMHandler theDCMHandler;
};

} // end namespace naoth
#endif	/* _NAO_MOTION_CONTROLLER_H */

