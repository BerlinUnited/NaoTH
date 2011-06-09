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
: PlatformInterface<NaoMotionController>("Nao", 10)
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

string NaoMotionController::getBodyID()
{
  return theDCMHandler.getBodyID();
}

string NaoMotionController::getBodyNickName()
{
  return theDCMHandler.getBodyNickName();
}

void NaoMotionController::init(ALPtr<ALBroker> pB)
{
  std::cout << "Init DCMHandler" << endl;
  theDCMHandler.init(pB);
  Platform::getInstance().init(this);
}

void NaoMotionController::get(unsigned int& timestamp)
{
  timestamp = theDCMHandler.getCurrentTimeStamp();
}

void NaoMotionController::get(FrameInfo& data)
{
  data.time = NaoTime::getNaoTimeInMilliSeconds();
  data.frameNumber++;
  data.basicTimeStep = getBasicTimeStep();
}

void NaoMotionController::get(SensorJointData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(AccelerometerData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(GyrometerData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(FSRData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(InertialSensorData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(IRReceiveData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(ButtonData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(BatteryData& data)
{
  theDCMHandler.get(data);
}

void NaoMotionController::get(UltraSoundReceiveData& data)
{
  theDCMHandler.get(data);
}


void NaoMotionController::set(const MotorJointData& data)
{
  theDCMHandler.set(data);
}

void NaoMotionController::set(const LEDData& data)
{
  theDCMHandler.set(data);
}

void NaoMotionController::set(const IRSendData& data)
{
  theDCMHandler.set(data);
}

void NaoMotionController::set(const UltraSoundSendData& data)
{
  theDCMHandler.set(data);
}

void NaoMotionController::updateSensorData()
{
//  Platform::getInstance()._commCollection.getSerialComm().sendMessage("hui");
  theDCMHandler.readSensorData();
}

void NaoMotionController::setActuatorData()
{
  theDCMHandler.sendActuatorData();
}
