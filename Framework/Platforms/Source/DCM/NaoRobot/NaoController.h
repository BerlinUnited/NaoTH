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

#include <string>
#include <fstream>
#include <iostream>

//
#include "PlatformInterface/PlatformInterface.h"
#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
//#include "Tools/Debug/Stopwatch.h"

//
#include "V4lCameraHandler.h"
#include "SoundControl.h"
#include "SPLGameController.h"
#include "DebugCommunication/DebugServer.h"

#include "Tools/Communication/Network/BroadCaster.h"
#include "Tools/Communication/Network/UDPReceiver.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/RemoteMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/WhistlePercept.h"
#include "Representations/Infrastructure/WhistleControl.h"

// local tools
#include "Tools/IPCData.h"
#include "Tools/NaoTime.h"
#include "Tools/SharedMemoryIO.h"

namespace naoth
{

class NaoController : public PlatformInterface
{
public:
  NaoController();
  virtual ~NaoController();

  virtual string getBodyID() const { return theBodyID; }
  virtual string getBodyNickName() const { return theBodyNickName; }
  virtual string getHeadNickName() const { return theHeadNickName; }
  virtual string getRobotName() const { return theRobotName; }

  // camera stuff
  void get(Image& data){ theBottomCameraHandler.get(data); } // blocking
  void get(ImageTop& data){ theTopCameraHandler.get(data); } // non blocking
  void get(CurrentCameraSettings& data) { theBottomCameraHandler.getCameraSettings(data); }
  void get(CurrentCameraSettingsTop& data) { theTopCameraHandler.getCameraSettings(data); }
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
  void get(IRReceiveData& data) { naoSensorData.get(data); }
  void get(ButtonData& data) { naoSensorData.get(data); }
  void get(BatteryData& data) { naoSensorData.get(data); }
  void get(UltraSoundReceiveData& data) { naoSensorData.get(data); }
  void get(WhistlePercept& data) {data.counter = whistleSensorData.data(); }
  void get(CpuData& data) {
      if (temperatureFile.is_open() && temperatureFile.good()) {
          // read temperature
          temperatureFile >> data.temperature;
          data.temperature /= 1000.0;
          // reset stream
          temperatureFile.clear();                 // clear fail and eof bits
          temperatureFile.seekg(0, std::ios::beg); // back to the start!
      } else {
        // Failed to open temperatureFile!
      }
  }

  // write directly to the shared memory
  // ACHTUNG: each set calls swapWriting()
  void set(const MotorJointData& data) { naoCommandMotorJointData.set(data); }
  void set(const LEDData& data) { naoCommandLEDData.set(data); }
  void set(const IRSendData& data) { naoCommandIRSendData.set(data); }
  void set(const UltraSoundSendData& data) { naoCommandUltraSoundSendData.set(data); }
  void set(const WhistleControl& data) { whistleControlData.set(data.onOffSwitch); }


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

  // -- begin -- shared memory access --
  // DCM --> NaoController
  SharedMemoryReader<NaoSensorData> naoSensorData;

  // NaoController --> DCM
  SharedMemoryWriter<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemoryWriter<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemoryWriter<Accessor<IRSendData> > naoCommandIRSendData;
  SharedMemoryWriter<Accessor<LEDData> > naoCommandLEDData;

  // WhistleDetector --> NaoController
  SharedMemoryReader<int> whistleSensorData;
  SharedMemoryWriter<Accessor<int> > whistleControlData;

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
  std::ifstream temperatureFile;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
