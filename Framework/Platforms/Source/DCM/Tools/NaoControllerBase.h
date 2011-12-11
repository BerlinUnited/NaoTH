/**
 * @file NaoControllerBase.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief Interface for the real robot for both cognition and motion
 *
 */

#ifndef _NAO_CONTROLLER_BASE_H_
#define _NAO_CONTROLLER_BASE_H_

#include "IPCData.h"
#include "Tools/NaoTime.h"
#include "SharedMemory.h"
#include "PlatformInterface/PlatformInterface.h"
#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Process.h"
#include <Representations/Infrastructure/FrameInfo.h>

namespace naoth
{
template<class PlatformType>
class NaoControllerBase : public PlatformInterface<PlatformType>
{
public:
  NaoControllerBase()
    :
    PlatformInterface<PlatformType>("Nao", 10),
    sensorDataReading(NULL)
  {
    staticMemberPath = Platform::getInstance().theConfigDirectory+"nao.info";

    // init shared memory
    const std::string naoCommandDataPath = "/nao_command_data";
    const std::string naoSensorDataPath = "/nao_sensor_data";
    std::cout << "Opening Shared Memory: "<<naoCommandDataPath<<std::endl;
    naoCommandData.open(naoCommandDataPath);
    std::cout<< "Opening Shared Memory: "<<naoSensorDataPath<<std::endl;
    naoSensorData.open(naoSensorDataPath);
  }

  virtual string getBodyID() const { return theBodyID; }

  virtual string getBodyNickName() const { return theBodyNickName; }
  
  void get(FrameInfo& data) { data = theFrameInfo; }
  
  void get(unsigned int& timestamp) { if (sensorDataReading!=NULL) timestamp = sensorDataReading->timeStamp; }

  void get(SensorJointData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(AccelerometerData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(GyrometerData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(FSRData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(InertialSensorData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(IRReceiveData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(ButtonData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(BatteryData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

  void get(UltraSoundReceiveData& data) { if (sensorDataReading!=NULL) sensorDataReading->get(data); }

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name)
  {
    return new MessageQueue4Process(name);
  }

  void updateFrameInfo()
  {
    theFrameInfo.setTime( NaoTime::getNaoTimeInMilliSeconds() );
  }

protected:
  const NaoSensorData* sensorDataReading;
  
  // DCM --> NaoController
  SharedMemory<NaoSensorData> naoSensorData;

  // NaoController --> DCM
  SharedMemory<NaoCommandData> naoCommandData;
  
  FrameInfo theFrameInfo;

  std::string staticMemberPath;
  std::string theBodyID;
  std::string theBodyNickName;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
