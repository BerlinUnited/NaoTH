/**
 * @file BoosterController.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @brief Interface for the real robot for both cognition and motion
 *
 */

#ifndef BOOSTER_CONTROLLER_H
#define BOOSTER_CONTROLLER_H

#include <string>
#include <fstream>
#include <iostream>

//
#include "PlatformInterface/PlatformInterface.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Process.h"
//#include "Tools/Debug/Stopwatch.h"

#include "Tools/Communication/Network/SPLGameController.h"
#include "Tools/Communication/Network/BroadCaster.h"
#include "Tools/Communication/Network/UDPReceiver.h"
#include "Tools/Communication/Network/UDPSender.h"

#include "DebugCommunication/DebugServer.h"
//#include "CPUTemperatureReader.h"

// representations
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/BatteryData.h>

#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/FSRData.h>

#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/TeamMessageDebug.h"
#include "Representations/Infrastructure/RemoteMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/AudioData.h"
#include <Representations/Infrastructure/AudioControl.h>
#include "Representations/Infrastructure/RobotInfo.h"

#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/CpuData.h>

#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/UltraSoundData.h>


// local tools
//#include "Tools/SharedMemoryIO.h"

#include "Tools/NaoTime.h"
#include "Tools/FileUtils.h"

#include "ImageBridge.h"

namespace naoth
{

class BoosterController : public PlatformInterface
{
public:
  BoosterController();
  virtual ~BoosterController();

  virtual std::string getBodyId() const         { return robotInfo.bodyId;        } // body serial number: AL...XXXX
  virtual std::string getBodyNickName() const   { return robotInfo.bodyNickName;  } // NaoXXXX
  virtual std::string getHeadId() const         { return robotInfo.headId;        } // SN of the head or mac of the eth0
  virtual std::string getRobotName() const      { return robotInfo.robotName;     } // name of the robot, e.g., nao12
  virtual std::string getPlatformName() const   { return robotInfo.platform;      } // e.g., 
  virtual unsigned int getBasicTimeStep() const { return robotInfo.basicTimeStep; }
  
  // camera stuff
  void get(Image& data) { 
    //imageBridge.readImage(data);
  }
  void get(ImageTop& data) { 
    imageBridge.readImage(data);
  }
  
  void get(CurrentCameraSettings& data) { 
    //theBottomCameraHandler.getCameraSettings(data);
  }
  void get(CurrentCameraSettingsTop& data) { 
    //theTopCameraHandler.getCameraSettings(data);
  }
  
  void set(const CameraSettingsRequest &request) {
    // FIXME: CameraSettings are assembled and copied in every frame 
    //CameraSettings settings = request.getCameraSettings();
    //theBottomCameraHandler.setAllCameraParams(settings);
  }

  void set(const CameraSettingsRequestTop &request) {
    // FIXME: CameraSettings are assembled and copied in every frame
    //CameraSettings settings = request.getCameraSettings();
    //theTopCameraHandler.setAllCameraParams(settings);
  }

  // sound
  void set(const SoundPlayData& data) {
    //theSoundHandler.setSoundData(data);
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { 
    if(theTeamCommListener != nullptr) { 
      theTeamCommListener->receive(data.data); 
    } 
  }
  void set(const TeamMessageDataOut& data) { 
    if(theTeamCommSender != nullptr) { 
      theTeamCommSender->send(data.data); 
    }
  }

  void get(RemoteMessageDataIn& data) { 
    if (theRemoteCommandListener != nullptr) {
      theRemoteCommandListener->receive(data.data); 
    }
  }

  // gamecontroller stuff
  void get(GameData& data){ 
    if(theGameController != nullptr) {
      theGameController->get(data); 
    }
  }
  void set(const GameReturnData& data) { 
    if(theGameController != nullptr) {
      theGameController->set(data); 
    }
  }

  // team debug stuff
  void set(const TeamMessageDebug& data)
  {
    if(theTeamCommDebugger == nullptr) {
      return;
    }

    // make sure we got something (eg. if the module is deactivated, the parameter are empty!)
    if (!data.host.empty() && data.port > 0 && (theTeamCommDebugger->getIp() != data.host || theTeamCommDebugger->getPort() != data.port))
    {
      std::cout << "[TeamCommDebugger] " << "Change debug teamcom to " << data.host << ":" << data.port << std::endl;
      if (theTeamCommDebugger != nullptr) { delete theTeamCommDebugger; }
      theTeamCommDebugger = new UDPSender(data.host, data.port, "TeamCommDebugger");
    }

    theTeamCommDebugger->send(data.data);
  }

  // debug comm
  void get(DebugMessageInCognition& data) { if(theDebugServer != nullptr) { theDebugServer->getDebugMessageInCognition(data); } }
  void get(DebugMessageInMotion& data) { if(theDebugServer != nullptr) { theDebugServer->getDebugMessageInMotion(data); } }
  void set(const DebugMessageOut& data) { if(theDebugServer != nullptr) { theDebugServer->setDebugMessageOut(data); } }

  // time
  void get(FrameInfo& data)
  {
    //TODO: use naoSensorData.data().timeStamp
    data.setTime(NaoTime::getNaoTimeInMilliSeconds());
    data.setFrameNumber(data.getFrameNumber()+1);

    std::cout << data << std::endl;
  }

  // read directly from the shared memory
  void get(SensorJointData& data) {       /*naoSensorData.get(data);*/ }
  void get(AccelerometerData& data) {     /*naoSensorData.get(data);*/ }
  void get(GyrometerData& data) {         /*naoSensorData.get(data);*/ }
  void get(FSRData& data) {               /*naoSensorData.get(data);*/ }
  void get(InertialSensorData& data) {    /*naoSensorData.get(data);*/ }
  void get(ButtonData& data) {            /*naoSensorData.get(data);*/ }
  void get(BatteryData& data) {           /*naoSensorData.get(data);*/ }
  void get(UltraSoundReceiveData& data) { /*naoSensorData.get(data);*/ }
  
  void get(AudioData& data) { 
    //theAudioRecorder.get(data); 
  }
  void get(CpuData& data) { 
    //theCPUTemperatureReader.get(data); 
  }

  // write directly to the shared memory
  // ACHTUNG: each set calls swapWriting()
  void set(const MotorJointData& data) { /*naoCommandMotorJointData.set(data);*/ }
  void set(const LEDData& data) { /*naoCommandLEDData.set(data);*/ }
  void set(const UltraSoundSendData& data) { /*naoCommandUltraSoundSendData.set(data);*/ }

  void set(const AudioControl& data) { /*theAudioRecorder.set(data);*/ }

  /*
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
  */


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
  //void readNaoInfo();


protected:
  virtual MessageQueue* createMessageQueue(const std::string& name)
  {
    return new MessageQueue4Threads();
    //return new MessageQueue4Process(name);
  }

protected:
  RobotInfo robotInfo;

  // -- begin -- shared memory access --
  // LoLa --> NaoController
  //SharedMemoryReader<DCMSensorData> naoSensorData;

  // NaoController --> LoLa
  //SharedMemoryWriter<Accessor<MotorJointData> > naoCommandMotorJointData;
  //SharedMemoryWriter<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  //SharedMemoryWriter<Accessor<LEDData> > naoCommandLEDData;
  // -- end -- shared memory access --

  //
  
  //V4lCameraHandler theBottomCameraHandler;
  //V4lCameraHandler theTopCameraHandler;
  
  //SoundControl theSoundHandler;
  //AudioRecorder theAudioRecorder;
  
  //CPUTemperatureReader theCPUTemperatureReader;
  
  // communication
  UDPSender* theTeamCommDebugger;
  BroadCaster* theTeamCommSender;
  UDPReceiver* theTeamCommListener;
  UDPReceiver* theRemoteCommandListener;
  SPLGameController* theGameController;
  DebugServer* theDebugServer;

  ImageBridge imageBridge;
};

} // end namespace naoth

#endif // BOOSTER_CONTROLLER_H
