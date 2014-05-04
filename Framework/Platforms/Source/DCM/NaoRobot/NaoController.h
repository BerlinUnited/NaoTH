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
  
  // camera stuff
  void get(Image& data){ theBottomCameraHandler.get(data); }
  void get(ImageTop& data){ theTopCameraHandler.get(data); }
  void get(CurrentCameraSettings& data) { theBottomCameraHandler.getCameraSettings(data); }
  void get(CurrentCameraSettingsTop& data) { theTopCameraHandler.getCameraSettings(data); }
  void set(const CameraSettingsRequest& data);
  void set(const CameraSettingsRequestTop& data);

  // sound
  void set(const SoundPlayData& data) 
  { 
    //if(data.soundFile.size() > 0) theSoundPlayer.play(data.soundFile); 
    theSoundHandler->setSoundData(data);
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { theBroadCastListener->receive(data.data); }
  void set(const TeamMessageDataOut& data) { theBroadCaster->send(data.data); }

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
      std::cerr << "[NaoController] didn't get new sensor data" << std::endl;
    }
    PlatformInterface::getMotionInput();
    STOPWATCH_STOP("getMotionInput");
  }


  virtual void setMotionOutput()
  {
    STOPWATCH_START("setMotionOutput");
    PlatformInterface::setMotionOutput();
    STOPWATCH_STOP("setMotionOutput");
  }


  virtual void getCognitionInput()
  {
    STOPWATCH_START("getCognitionInput");
    PlatformInterface::getCognitionInput();
    STOPWATCH_STOP("getCognitionInput");
  }
  

  void setCognitionOutput()
  {
    STOPWATCH_START("setCognitionOutput");
    PlatformInterface::setCognitionOutput();
    STOPWATCH_STOP("setCognitionOutput");
  }


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
  V4lCameraHandler theTopCameraHandler;
  //SoundPlayer theSoundPlayer;
  SoundControl *theSoundHandler;
  BroadCaster* theBroadCaster;
  BroadCastListener* theBroadCastListener;
  SPLGameController* theGameController;
  DebugServer* theDebugServer;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
