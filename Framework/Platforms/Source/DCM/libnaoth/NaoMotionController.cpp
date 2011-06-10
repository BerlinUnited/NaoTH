/**
 * @file NaoMotionController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot for motion
 *
 */

#include "NaoMotionController.h"
#include <PlatformInterface/Platform.h>

using namespace naoth;

NaoMotionController::NaoMotionController()
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
  //registerOutput<const LEDData>(*this);
  //registerOutput<const IRSendData>(*this);
  //registerOutput<const UltraSoundSendData>(*this);
  registerOutput<const MotorJointData>(*this);
}

NaoMotionController::~NaoMotionController()
{
}

void NaoMotionController::init(ALPtr<ALBroker> pB)
{
  std::cout << "Init Platform" << endl;
  Platform::getInstance().init(this);
  
  std::cout << "Init DCMHandler" << endl;
  theDCMHandler.init(pB);
  
  // save the body ID
  string bodyID = theDCMHandler.getBodyID();
  std::cout << "get bodyID"<< bodyID << endl;
  
  // save the nick name
  string nickName = theDCMHandler.getBodyNickName();
  std::cout << "get nickName"<< nickName << endl;
}

void NaoMotionController::set(const MotorJointData& data)
{
  libNaothData.writing()->theMotorJointData = data;
  theDCMHandler.setAllMotorData(data);
}

void NaoMotionController::getMotionInput()
{
  libNaothData.swapWriting();
  LibNaothData* libNaothDataWriting = libNaothData.writing();
  theDCMHandler.readSensorData(libNaothDataWriting->timeStamp, libNaothDataWriting->sensorsValue);
  libNaothDataReading = libNaothDataWriting;
  NaoControllerBase<NaoMotionController>::getMotionInput();
}

void NaoMotionController::setMotionOutput()
{
  NaoControllerBase<NaoMotionController>::setMotionOutput();
  
  if ( naothData.swapReading() )
  {
    const NaothData* naothDataReading = naothData.reading();
    theDCMHandler.setLED(naothDataReading->lEDData());
    theDCMHandler.setIRSend(naothDataReading->iRSendData());
    theDCMHandler.setUltraSoundSend(naothDataReading->ultraSoundSendData());
  }
}
