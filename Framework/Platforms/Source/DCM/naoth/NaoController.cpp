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
: PlatformInterface<NaoController>("Nao", 10),
theSoundHandler(NULL),
theLEDData(NULL),
theIRSendData(NULL),
theUltraSoundSendData(NULL)
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
  registerInput<MotorJointData>(*this);

  // register output
  registerOutput<const CameraSettingsRequest>(*this);
  registerOutput<const LEDData>(*this);
  registerOutput<const IRSendData>(*this);
  registerOutput<const UltraSoundSendData>(*this);
  registerOutput<const SoundPlayData>(*this);
  
  std::cout << "Open Shared Memory"<<endl;
  libNaothData.open("/libnaoth");
  naothData.open("/naoth");
  theLEDData = &(naothData.data().theLEDData);
  theIRSendData = &(naothData.data().theIRSendData);
  theUltraSoundSendData = &(naothData.data().theUltraSoundSendData);
  
  cout<<"Init Platform"<<endl;
  Platform::getInstance().init(this);
  
  std::cout << "Init CameraHandler" << endl;
  theCameraHandler.init("/dev/video");
  
  std::cout << "Init SoundHandler" <<endl;
  theSoundHandler = new SoundControl();
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

string NaoController::getBodyID() const
{
  //libNaothData.lock(); // lock shared memory before reading
  string bodyID(libNaothData.data().bodyID);
  //libNaothData.unlock();
  return bodyID;
}

string NaoController::getBodyNickName() const
{
  //libNaothData.lock(); // lock shared memory before reading
  string nickName(libNaothData.data().nickName);
  //libNaothData.unlock();
  return nickName;
}

void NaoController::get(unsigned int& timestamp)
{
  libNaothData.lock(); // lock shared memory before reading
  timestamp = libNaothData.data().timeStamp;
  libNaothData.unlock();
}

void NaoController::get(FrameInfo& data)
{
  data.time = NaoTime::getNaoTimeInMilliSeconds();
  data.frameNumber++;
  data.basicTimeStep = getBasicTimeStep();
}

void NaoController::get(SensorJointData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(AccelerometerData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(Image& data)
{
  theCameraHandler.get(data);
}

void NaoController::get(GyrometerData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(FSRData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(InertialSensorData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(IRReceiveData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(ButtonData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(BatteryData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(UltraSoundReceiveData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::get(MotorJointData& data)
{
  libNaothData.lock(); // lock shared memory before reading
  libNaothData.data().get(data);
  libNaothData.unlock();
}

void NaoController::set(const LEDData& data)
{
  naothData.lock(); // lock shared memory before writing
  *theLEDData = data;
  naothData.unlock();
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
  naothData.lock(); // lock shared memory before writing
  *theIRSendData = data;
  naothData.unlock();
}

void NaoController::set(const UltraSoundSendData& data)
{
  naothData.lock(); // lock shared memory before writing
  *theUltraSoundSendData = data;
  naothData.unlock();
}

void NaoController::set(const SoundPlayData& data)
{
  theSoundHandler->setSoundData(data);
}
