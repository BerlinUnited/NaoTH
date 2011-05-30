/**
 * @file NaoController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot
 *
 */

#include "NaoController.h"
#include "Tools/NaoTime.h"
#include <PlatformInterface/Platform.h>

using namespace naoth;

NaoController::NaoController()
: PlatformInterface<NaoController>("Nao", 10)
{
  // register input
  registerInput<AccelerometerData>(*this);
  registerInput<FrameInfo>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<Image>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<IRReceiveData>(*this);
  registerInput<CurrentCameraSettings>(*this);
  registerInput<ButtonData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<UltraSoundReceiveData>(*this);

  // register output
  registerOutput<const CameraSettingsRequest>(*this);
  registerOutput<const LEDData>(*this);
  registerOutput<const IRSendData>(*this);
  registerOutput<const UltraSoundSendData>(*this);
  registerOutput<const SoundData>(*this);
  registerOutput<const MotorJointData>(*this);
}

NaoController::~NaoController()
{
  if (theSoundHandler!=NULL)
  {
    delete theSoundHandler;
  }
}

string NaoController::getHardwareIdentity() const
{
  return Platform::getMACaddress("eth0");
}

string NaoController::getBodyID()
{
  return theDCMHandler.getBodyID();
}

string NaoController::getBodyNickName()
{
  return theDCMHandler.getBodyNickName();
}

void NaoController::init(ALPtr<ALBroker> pB)
{
  std::cout << "Init DCMHandler" << endl;
  theDCMHandler.init(pB);
  std::cout << "Init CameraHandler" << endl;
  theCameraHandler.init("/dev/video");
  //theCameraHandler.init(pB);
  std::cout << "Init SoundHandler" <<endl;
  theSoundHandler = new SoundControl();
  
  Platform::getInstance().init(this);
  
}

void NaoController::get(unsigned int& timestamp)
{
  timestamp = theDCMHandler.getCurrentTimeStamp();
}

void NaoController::get(FrameInfo& data)
{
  data.time = NaoTime::getNaoTimeInMilliSeconds();
  data.frameNumber++;
  data.basicTimeStep = getBasicTimeStep();
}

void NaoController::get(SensorJointData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(AccelerometerData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(Image& data)
{
  theCameraHandler.get(data);
}

void NaoController::get(GyrometerData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(FSRData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(InertialSensorData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(IRReceiveData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(ButtonData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(BatteryData& data)
{
  theDCMHandler.get(data);
}

void NaoController::get(UltraSoundReceiveData& data)
{
  theDCMHandler.get(data);
}


void NaoController::set(const MotorJointData& data)
{
  theDCMHandler.setAllMotorData(data);
}

void NaoController::set(const LEDData& data)
{
  theDCMHandler.setLED(data);
}

void NaoController::set(const CameraSettingsRequest& data)
{
  theCameraHandler.setCameraSettings(data, data.queryCameraSettings);
}

void NaoController::get(CurrentCameraSettings& data)
{
  theCameraHandler.getCameraSettings(data);
}

void NaoController::set(const IRSendData& data)
{
  theDCMHandler.setIRSend(data);
}

void NaoController::set(const UltraSoundSendData& data)
{
  theDCMHandler.setUltraSoundSend(data);
}

void NaoController::set(const SoundData& data)
{
  theSoundHandler->setSoundData(data);
}

void NaoController::run()
{
//  Platform::getInstance()._commCollection.getSerialComm().sendMessage("hui");
  theDCMHandler.getData();
}

void NaoController::close()
{
//  Platform::getInstance()._commCollection.getSerialComm().sendMessage("hui");
}
