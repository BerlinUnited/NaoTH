/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief Interface for the real robot for both cognition and motion
 *
 */

#ifndef _NaoController_H_
#define _NaoController_H_

#include <glib.h>
#include <string>

//
#include "PlatformInterface/PlatformInterface.h"
#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
#include "Tools/Debug/Stopwatch.h"

//
#include "V4lCameraHandler.h"
//#include "SoundPlayer.h"
#include "SoundControl.h"
#include "SPLGameController.h"
#include "DebugCommunication/DebugServer.h"

#include "Tools/Communication/Broadcast/BroadCaster.h"
#include "Tools/Communication/Broadcast/BroadCastListener.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/SoundData.h"

// local tools
#include "Tools/IPCData.h"
#include "Tools/NaoTime.h"
#include "Tools/SharedMemory.h"


namespace naoth
{

// TODO: remove TEAMCOMM_MAX_MSG_SIZE
static const unsigned int TEAMCOMM_MAX_MSG_SIZE = 4096;


// MutexSharedMemoryReaderWithSwap
template<class V>
class SharedMemoryWriter
{
public:
  SharedMemoryWriter() : mutex(NULL)
  {
    mutex = g_mutex_new();
  }

  ~SharedMemoryWriter()
  {
    g_mutex_free(mutex);
    memory.close();
  }

  void open(std::string name)
  {
    // debug
    std::cout << "Opening Shared Memory: " << name << std::endl;
    memory.open(name);
  }

  template<class T>
  void set(const T& data)
  { 
    g_mutex_lock(mutex);
    // copy the data into the shared memory
    memory.writing()->set(data);
    memory.swapWriting();
    g_mutex_unlock(mutex);
  }

private:
 GMutex* mutex;
 SharedMemory<V> memory;
};//end class SharedMemoryWriter


template<class V>
class SharedMemoryReader
{
public:
  SharedMemoryReader() : mutex(NULL)
  {
    mutex = g_mutex_new();
  }

  ~SharedMemoryReader()
  {
    g_mutex_free(mutex);
    memory.close();
  }

  void open(std::string name)
  {
    // debug
    std::cout << "Opening Shared Memory: " << name << std::endl;
    memory.open(name);
  }

  template<class T>
  void get(T& data) const 
  { 
    g_mutex_lock(mutex);
    memory.reading()->get(data);
    g_mutex_unlock(mutex);
  }

  // delegate
  inline bool swapReading(){ return memory.swapReading(); }
  inline const V& data() { return *memory.reading(); }

private:
 GMutex* mutex;
 SharedMemory<V> memory;
};//end class SharedMemoryReader



class NaoController : public PlatformInterface
{
public:
  NaoController();
  ~NaoController();

  virtual string getBodyID() const { return theBodyID; }
  virtual string getBodyNickName() const { return theBodyNickName; }
  virtual string getHeadNickName() const { return theHeadNickName; }
  

  // camera stuff
  void get(Image& data){ theBottomCameraHandler.get(data); }
  void get(SecondaryImage& data){ theBottomCameraHandler.get(data); }
  void get(CurrentCameraSettings& data) { theBottomCameraHandler.getCameraSettings(data); }
  void set(const CameraSettingsRequest& data);


  // sound
  void set(const SoundPlayData& data) 
  { 
    //if(data.soundFile.size() > 0) theSoundPlayer.play(data.soundFile); 
    theSoundHandler->setSoundData(data);
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { theBroadCastListener->receive(data.data); }
  void set(const TeamMessageDataOut& data)
  {
    // TODO: remove TEAMCOMM_MAX_MSG_SIZE
    if ( data.data.size() <= TEAMCOMM_MAX_MSG_SIZE )
      theBroadCaster->send(data.data);
    else
      cerr << "NaoController: TeamMessageDataOut is too big " << data.data.size() << endl;
  }//end set TeamMessageDataOut


  // rctc teamcomm stuff
  void get(RCTCTeamMessageDataIn& data);
  void set(const RCTCTeamMessageDataOut& data);


  // gamecontroller stuff
  void get(GameData& data){ theGameController->get(data, NaoTime::getNaoTimeInMilliSeconds()); }
  void set(const GameReturnData& data) { theGameController->setReturnData(data); }

  // debug comm
  void get(DebugMessageIn& data) { theDebugServer->getDebugMessageIn(data); }
  void set(const DebugMessageOut& data) { theDebugServer->setDebugMessageOut(data); }

  // time
  void get(FrameInfo& data)
  {
    //TODO: use naoSensorData.data().timeStamp
    data.setTime(NaoTime::getNaoTimeInMilliSeconds()); 
    data.setFrameNumber(data.getFrameNumber()+1);
  }


  // read directly from the shared memory
  void get(SensorJointData& data) { naoSensorData.get(data); }
  void get(AccelerometerData& data) { naoSensorData.get(data); }
  void get(GyrometerData& data) { naoSensorData.get(data); }
  void get(FSRData& data) { naoSensorData.get(data); }
  void get(InertialSensorData& data) { naoSensorData.get(data); }
  void get(IRReceiveData& data) { naoSensorData.get(data); }
  void get(ButtonData& data) { naoSensorData.get(data); }
  void get(BatteryData& data) { naoSensorData.get(data); }
  void get(UltraSoundReceiveData& data) { naoSensorData.get(data); }


  // write directly to the shared memory
  // ACHTUNG: each set calls swapWriting()
  void set(const MotorJointData& data) { naoCommandMotorJointData.set(data); }
  void set(const LEDData& data) { naoCommandLEDData.set(data); }
  void set(const IRSendData& data) { naoCommandIRSendData.set(data); }
  void set(const UltraSoundSendData& data) { naoCommandUltraSoundSendData.set(data); }


  virtual void getMotionInput()
  {
    STOPWATCH_START("getMotionInput");
    // try to get some data from the DCM
    if ( !naoSensorData.swapReading() )
    {
      std::cerr << "didn't get new sensor data" << std::endl;
    }
    PlatformInterface::getMotionInput();
    STOPWATCH_STOP("getMotionInput");
  }//end getMotionInput


  virtual void setMotionOutput()
  {
    STOPWATCH_START("setMotionOutput");
    PlatformInterface::setMotionOutput();
    STOPWATCH_STOP("setMotionOutput");
  }//end setMotionOutput


  virtual void getCognitionInput()
  {
    STOPWATCH_START("getCognitionInput");
    PlatformInterface::getCognitionInput();
    STOPWATCH_STOP("getCognitionInput");
  }//end getCognitionInput
  

  void setCognitionOutput()
  {
    STOPWATCH_START("setCognitionOutput");
    PlatformInterface::setCognitionOutput();
    STOPWATCH_STOP("setCognitionOutput");
  }//end setCognitionOutput


protected:
  virtual MessageQueue* createMessageQueue(const std::string& name)
  {
    return new MessageQueue4Threads();
  }

protected:
  std::string theBodyID;
  std::string theBodyNickName;
  std::string theHeadNickName;


  // -- begin -- shared memory access --
  // DCM --> NaoController
  SharedMemoryReader<NaoSensorData> naoSensorData;

  // NaoController --> DCM
  SharedMemoryWriter<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemoryWriter<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemoryWriter<Accessor<IRSendData> > naoCommandIRSendData;
  SharedMemoryWriter<Accessor<LEDData> > naoCommandLEDData;
  // -- end -- shared memory access --
  
  //
  V4lCameraHandler theBottomCameraHandler;
  //SoundPlayer theSoundPlayer;
  SoundControl *theSoundHandler;
  BroadCaster* theBroadCaster;
  BroadCastListener* theBroadCastListener;
  SPLGameController* theGameController;
  DebugServer* theDebugServer;

  // RCTC
  BroadCaster* theRCTCBroadCaster;
  BroadCastListener* theRCTCBroadCastListener;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
