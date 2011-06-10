/**
 * @file NaoMotionController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot
 *
 */

#include "NaoMotionController.h"
#include "Tools/NaoTime.h"
#include <PlatformInterface/Platform.h>

using namespace naoth;

NaoMotionController::NaoMotionController()
: PlatformInterface<NaoMotionController>("Nao", 10),
sensorsValue(NULL),
theMotorJointData(NULL),
theLEDData(NULL),
theIRSendData(NULL),
theUltraSoundSendData(NULL)
{
  // register input
  registerInput<AccelerometerData>(*this);
  registerInput<FrameInfo>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<IRReceiveData>(*this);
  registerInput<ButtonData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<UltraSoundReceiveData>(*this);

  // register output
  registerOutput<const LEDData>(*this);
  registerOutput<const IRSendData>(*this);
  registerOutput<const UltraSoundSendData>(*this);
  registerOutput<const MotorJointData>(*this);
}

NaoMotionController::~NaoMotionController()
{
}

string NaoMotionController::getHardwareIdentity() const
{
  return Platform::getMACaddress("eth0");
}

void NaoMotionController::init(ALPtr<ALBroker> pB)
{
  // init shared memory
  std::cout << "Open Shared Memory" << endl;
  libNaothData.open("/libnaoth");
  sensorsValue = libNaothData.data().sensorsValue;
  theMotorJointData = &(libNaothData.data().theMotorJointData);
  
  naothData.open("/naoth");
  theLEDData = &(naothData.data().theLEDData);
  theIRSendData = &(naothData.data().theIRSendData);
  theUltraSoundSendData = &(naothData.data().theUltraSoundSendData);
  
  std::cout << "Init Platform" << endl;
  Platform::getInstance().init(this);
  
  std::cout << "Init DCMHandler" << endl;
  theDCMHandler.init(pB);
  
  // save the body ID
  string bodyID = theDCMHandler.getBodyID();
  int lenBodyID = min(bodyID.length(), sizeof(libNaothData.data().bodyID) - 1);
  memcpy(libNaothData.data().bodyID, bodyID.c_str(), lenBodyID);
  libNaothData.data().bodyID[lenBodyID] = '\0';
  std::cout << "get bodyID"<< bodyID << endl;
  
  // save the nick name
  string nickName = theDCMHandler.getBodyNickName();
  int lenNickName = min(nickName.length(), sizeof(libNaothData.data().nickName) - 1);
  memcpy(libNaothData.data().nickName, nickName.c_str(), lenNickName);
  libNaothData.data().nickName[lenNickName] = '\0';
  std::cout << "get nickName"<< nickName << endl;
}

void NaoMotionController::get(unsigned int& timestamp)
{
  timestamp = libNaothData.data().timeStamp;
}

void NaoMotionController::get(FrameInfo& data)
{
  data.time = NaoTime::getNaoTimeInMilliSeconds();
  data.frameNumber++;
  data.basicTimeStep = getBasicTimeStep();
}

void NaoMotionController::set(const MotorJointData& data)
{
  libNaothData.lock(); // lock shared memory before writing
  *theMotorJointData = data;
  libNaothData.unlock();
}

void NaoMotionController::set(const LEDData& data)
{
  naothData.lock(); // lock shared memory before writing
  *theLEDData = data;
  naothData.unlock();
}

void NaoMotionController::set(const IRSendData& data)
{
  naothData.lock(); // lock shared memory before writing
  *theIRSendData = data;
  naothData.unlock();
}

void NaoMotionController::set(const UltraSoundSendData& data)
{
  naothData.lock(); // lock shared memory before writing
  *theUltraSoundSendData = data;
  naothData.unlock();
}

void NaoMotionController::updateSensorData()
{
  libNaothData.lock(); // lock shared memory before writing
  theDCMHandler.readSensorData(libNaothData.data().timeStamp, sensorsValue);
  libNaothData.unlock();
}

void NaoMotionController::setActuatorData()
{
  theDCMHandler.setAllMotorData(*theMotorJointData);
  naothData.lock(); // lock shared memory before reading
  theDCMHandler.setLED(*theLEDData);
  theDCMHandler.setIRSend(*theIRSendData);
  theDCMHandler.setUltraSoundSend(*theUltraSoundSendData);
  naothData.unlock();
}
