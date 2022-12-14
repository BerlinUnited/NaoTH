/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @brief Interface for the real robot for both cognition and motion
 *
 */

#ifndef _NaoController_H_
#define _NaoController_H_

#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

//
#include "PlatformInterface/PlatformInterface.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
//#include "Tools/Debug/Stopwatch.h"

//
#include "V4lCameraHandler.h"
#include "CameraSettingsV5Manager.h"
#include "CameraSettingsV6Manager.h"


#include "SoundControl.h"
#include "SPLGameController.h"
#include "CPUTemperatureReader.h"
#include "DebugCommunication/DebugServer.h"
#include "AudioRecorder.h"

#include "Tools/Communication/Network/BroadCaster.h"
#include "Tools/Communication/Network/UDPReceiver.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/RemoteMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/AudioData.h"

// local tools
#include "Tools/DCMData.h"
#include "Tools/NaoTime.h"
#include "Tools/SharedMemoryIO.h"

namespace naoth
{

class NaoController : public PlatformInterface
{
public:
  NaoController(bool nao6);
  virtual ~NaoController();

  // platform info
  const bool nao6;
  virtual std::string getBodyID() const { return theBodyID; }
  virtual std::string getBodyNickName() const { return theBodyNickName; }
  virtual std::string getHeadNickName() const { return theHeadNickName; }
  virtual std::string getRobotName() const { return theRobotName; }
  virtual std::string getPlatformName() const { return nao6 ? "Nao6" : "Nao"; }
  virtual unsigned int getBasicTimeStep() const { return nao6 ? 12 : 10; }
  
  // camera stuff
  void get(Image& data){ 
    theBottomCameraHandler.get(data); 
  } // blocking
  void get(ImageTop& data){ 
    theTopCameraHandler.get(data); 
  } // non blocking
  
  void get(CurrentCameraSettings& data) { 
    theBottomCameraHandler.getCameraSettings(data);
  }
  void get(CurrentCameraSettingsTop& data) { 
    theTopCameraHandler.getCameraSettings(data);
  }
  
  void set(const CameraSettingsRequest& data);
  void set(const CameraSettingsRequestTop& data);

  // sound
  void set(const SoundPlayData& data)
  {
    theSoundHandler->setSoundData(data);
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { theTeamCommListener->receive(data.data); }
  void set(const TeamMessageDataOut& data) { theTeamCommSender->send(data.data); }

  void get(RemoteMessageDataIn& data) { theRemoteCommandListener->receive(data.data); }

  // gamecontroller stuff
  void get(GameData& data){ theGameController->get(data); }
  void set(const GameReturnData& data) { theGameController->set(data); }

  // debug comm
  void get(DebugMessageInCognition& data) { theDebugServer->getDebugMessageInCognition(data); }
  void get(DebugMessageInMotion& data) { theDebugServer->getDebugMessageInMotion(data); }
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
  void get(ButtonData& data) { naoSensorData.get(data); }
  void get(BatteryData& data) { naoSensorData.get(data); }
  void get(UltraSoundReceiveData& data) { naoSensorData.get(data); }
  
  void get(AudioData& data) { theAudioRecorder.get(data); }
  void get(CpuData& data) { theCPUTemperatureReader.get(data); }

  // write directly to the shared memory
  // ACHTUNG: each set calls swapWriting()
  void set(const MotorJointData& data) { naoCommandMotorJointData.set(data); }
  void set(const LEDData& data) { naoCommandLEDData.set(data); }
  void set(const UltraSoundSendData& data) { naoCommandUltraSoundSendData.set(data); }

  void set(const AudioControl& data) { theAudioRecorder.set(data); }

  virtual void getMotionInput()
  {
    //STOPWATCH_START("getMotionInput");
    // try to get some data from the DCM
    if ( !naoSensorData.swapReading() )
    {
      std::cerr << "[NaoController] didn't get new sensor data" << std::endl;
    }
    PlatformInterface::getMotionInput();
    //STOPWATCH_STOP("getMotionInput");
  }


  virtual void setMotionOutput()
  {
    //STOPWATCH_START("setMotionOutput");
    PlatformInterface::setMotionOutput();
    //STOPWATCH_STOP("setMotionOutput");
  }


  virtual void getCognitionInput()
  {
    //STOPWATCH_START("getCognitionInput");
    PlatformInterface::getCognitionInput();
    //STOPWATCH_STOP("getCognitionInput");
  }


  virtual void setCognitionOutput()
  {
    //STOPWATCH_START("setCognitionOutput");
    PlatformInterface::setCognitionOutput();
    //STOPWATCH_STOP("setCognitionOutput");
  }


protected:
  virtual MessageQueue* createMessageQueue(const std::string& /*name*/)
  {
    return new MessageQueue4Threads();
  }

protected:
  std::string theBodyID;
  std::string theBodyNickName;
  std::string theHeadNickName;
  std::string theRobotName;

  bool lolaAvailable;

  // -- begin -- shared memory access --
  // DCM --> NaoController
  SharedMemoryReader<DCMSensorData> naoSensorData;

  // NaoController --> DCM
  SharedMemoryWriter<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemoryWriter<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemoryWriter<Accessor<LEDData> > naoCommandLEDData;
  // -- end -- shared memory access --

  //
  
  V4lCameraHandler theBottomCameraHandler;
  V4lCameraHandler theTopCameraHandler;
  
  SoundControl *theSoundHandler;
  BroadCaster* theTeamCommSender;
  UDPReceiver* theTeamCommListener;
  UDPReceiver* theRemoteCommandListener;
  SPLGameController* theGameController;
  DebugServer* theDebugServer;
  CPUTemperatureReader theCPUTemperatureReader;
  AudioRecorder theAudioRecorder;

private:
  static bool fileExists (const std::string& filename);
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
