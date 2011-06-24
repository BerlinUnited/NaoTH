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
#include "MacAddr.h"
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
  NaoControllerBase():PlatformInterface<PlatformType>("Nao", 10),libNaothDataReading(NULL)
  {
    staticMemberPath = Platform::getInstance().theConfigDirectory+"nao.info";

    // init shared memory
    const std::string libnaothpath = "/libnaoth";
    const std::string naothpath = "/naoth";
    std::cout << "Opening Shared Memory: "<<libnaothpath<<std::endl;
    libNaothData.open(libnaothpath);
    std::cout<< "Opening Shared Memory: "<<naothpath<<std::endl;
    naothData.open(naothpath);
  }

  virtual string getBodyID() const { return theBodyID; }

  virtual string getBodyNickName() const { return theBodyNickName; }
  
  void get(FrameInfo& data) { data = theFrameInfo; }
  
  void get(unsigned int& timestamp) { if (libNaothDataReading!=NULL) timestamp = libNaothDataReading->timeStamp; }

  void get(SensorJointData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(AccelerometerData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(GyrometerData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(FSRData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(InertialSensorData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(IRReceiveData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(ButtonData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(BatteryData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

  void get(UltraSoundReceiveData& data) { if (libNaothDataReading!=NULL) libNaothDataReading->get(data); }

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name)
  {
    return new MessageQueue4Process(name);
  }

  void updateFrameInfo()
  {
    theFrameInfo.time = NaoTime::getNaoTimeInMilliSeconds();
    theFrameInfo.frameNumber++;
  }

protected:
  const LibNaothData* libNaothDataReading;
  
  SharedMemory<LibNaothData> libNaothData;
  SharedMemory<NaothData> naothData;
  FrameInfo theFrameInfo;

  std::string staticMemberPath;
  std::string theBodyID;
  std::string theBodyNickName;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
