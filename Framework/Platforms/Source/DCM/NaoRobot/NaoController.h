/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @brief Interface for the real robot for both cognition and motion
 *
 */

#ifndef NAO_CONTROLLER_H
#define NAO_CONTROLLER_H

#include <string>
#include <fstream>
#include <iostream>

//
#include "PlatformInterface/PlatformInterface.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
//#include "Tools/Debug/Stopwatch.h"

//
#include "V4lCameraHandler.h"
#include "CameraSettingsV5Manager.h"
#include "CameraSettingsV6Manager.h"


#include "SoundControl.h"
#include "CPUTemperatureReader.h"
#include "DebugCommunication/DebugServer.h"
#include "AudioRecorder.h"

#include "Tools/Communication/Network/SPLGameController.h"
#include "Tools/Communication/Network/BroadCaster.h"
#include "Tools/Communication/Network/UDPReceiver.h"
#include "Tools/Communication/Network/UDPSender.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/TeamMessageDebug.h"
#include "Representations/Infrastructure/RemoteMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/AudioData.h"

// local tools
#include "Tools/DCMData.h"
#include "Tools/NaoTime.h"
#include "Tools/SharedMemoryIO.h"
#include "Tools/FileUtils.h"

namespace naoth
{

class NaoController : public PlatformInterface
{
public:
  NaoController(bool nao6);
  virtual ~NaoController();

  // platform info
  const bool nao6;
  virtual std::string getBodyID() const         { return theBodyID;             } // body serial number: AL...XXXX
  virtual std::string getBodyNickName() const   { return theBodyNickName;       } // NaoXXXX
  virtual std::string getHeadNickName() const   { return theHeadNickName;       } // mac of the eth0
  virtual std::string getRobotName() const      { return theRobotName;          } // e.g., nao12
  virtual std::string getPlatformName() const   { return nao6 ? "Nao6" : "Nao"; }
  virtual unsigned int getBasicTimeStep() const { return nao6 ? 12 : 10;        }
  
  // camera stuff
  void get(Image& data) { 
    theBottomCameraHandler.get(data); 
  } // blocking
  void get(ImageTop& data) { 
    theTopCameraHandler.get(data); 
  } // non blocking
  
  void get(CurrentCameraSettings& data) { 
    theBottomCameraHandler.getCameraSettings(data);
  }
  void get(CurrentCameraSettingsTop& data) { 
    theTopCameraHandler.getCameraSettings(data);
  }
  
  void set(const CameraSettingsRequest &request) {
    // FIXME: CameraSettings are assembled and copied in every frame 
    CameraSettings settings = request.getCameraSettings();
    theBottomCameraHandler.setAllCameraParams(settings);
  }

  void set(const CameraSettingsRequestTop &request) {
    // FIXME: CameraSettings are assembled and copied in every frame
    CameraSettings settings = request.getCameraSettings();
    theTopCameraHandler.setAllCameraParams(settings);
  }

  // sound
  void set(const SoundPlayData& data) {
    theSoundHandler.setSoundData(data);
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { theTeamCommListener->receive(data.data); }
  void set(const TeamMessageDataOut& data) { theTeamCommSender->send(data.data); }

  void get(RemoteMessageDataIn& data) { theRemoteCommandListener->receive(data.data); }

  // gamecontroller stuff
  void get(GameData& data){ theGameController->get(data); }
  void set(const GameReturnData& data) { theGameController->set(data); }

  // team debug stuff
  void set(const TeamMessageDebug& data)
  {
      if (theTeamCommDebugger->getIp() != data.host || theTeamCommDebugger->getPort() != data.port)
      {
        std::cout << "[TeamCommDebugger] " << "Change debug teamcom to " << data.host << ":" << data.port << std::endl;
        if (theTeamCommDebugger != nullptr) { delete theTeamCommDebugger; }
        theTeamCommDebugger = new UDPSender(data.host, data.port, "TeamCommDebugger");
      }

      theTeamCommDebugger->send(data.data);
  }

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

private:
  void readNaoInfo();

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
  
  SoundControl theSoundHandler;
  AudioRecorder theAudioRecorder;
  
  CPUTemperatureReader theCPUTemperatureReader;
  
  // communication
  UDPSender* theTeamCommDebugger;
  BroadCaster* theTeamCommSender;
  UDPReceiver* theTeamCommListener;
  UDPReceiver* theRemoteCommandListener;
  SPLGameController* theGameController;
  DebugServer* theDebugServer;

};

} // end namespace naoth

#endif // NAO_CONTROLLER_H
