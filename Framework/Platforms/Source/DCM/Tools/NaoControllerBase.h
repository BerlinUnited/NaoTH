/**
 * @file NaoControllerBase.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief Interface for the real robot
 *
 */

#ifndef _NAO_CONTROLLER_BASE_H_
#define _NAO_CONTROLLER_BASE_H_

#include "IPCData.h"
#include "MacAddr.h"
#include "Tools/NaoTime.h"
#include "PlatformInterface/PlatformInterface.h"
#include <Representations/Infrastructure/FrameInfo.h>

namespace naoth
{
template<class PlatformType>
class NaoControllerBase : public PlatformInterface<PlatformType>
{
public:
  NaoControllerBase():PlatformInterface<PlatformType>("Nao", 10),libNaothDataReading(NULL)
  {
  }

  virtual string getHardwareIdentity() const { return getMACaddress("eth0");}

  virtual string getBodyID() const { return libNaothDataReading->getBodyID(); }

  virtual string getBodyNickName() const { return libNaothDataReading->getBodyID(); }
  
  void get(FrameInfo& data)
  {
    data.time = NaoTime::getNaoTimeInMilliSeconds();
    data.frameNumber++;
    data.basicTimeStep = this->getBasicTimeStep();
  }
  
  void get(unsigned int& timestamp) { timestamp = libNaothDataReading->timeStamp; }

  void get(SensorJointData& data) { libNaothDataReading->get(data); }

  void get(AccelerometerData& data) { libNaothDataReading->get(data); }

  void get(GyrometerData& data) { libNaothDataReading->get(data); }

  void get(FSRData& data) { libNaothDataReading->get(data); }

  void get(InertialSensorData& data) { libNaothDataReading->get(data); }

  void get(IRReceiveData& data) { libNaothDataReading->get(data); }

  void get(ButtonData& data) { libNaothDataReading->get(data); }

  void get(BatteryData& data) { libNaothDataReading->get(data); }

  void get(UltraSoundReceiveData& data) { libNaothDataReading->get(data); }

protected:
  const LibNaothData* libNaothDataReading;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_