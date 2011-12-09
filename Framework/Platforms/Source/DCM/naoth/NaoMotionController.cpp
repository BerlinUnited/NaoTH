/**
 * @file NaoMotionController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot for motion
 *
 */

#include "NaoMotionController.h"
#include <PlatformInterface/Platform.h>
#include "Tools/IPCData.h"

using namespace naoth;

NaoMotionController::NaoMotionController()
{
  // read the value from file
  ifstream is(staticMemberPath.c_str());
  ASSERT(is.good());
  is>>theBodyID>>theBodyNickName;
  cout<<"bodyID: "<<theBodyID<<endl;
  cout<<"bodyNickName: "<<theBodyNickName<<endl;

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
  registerOutput<const MotorJointData>(*this);

  std::cout << "Init Platform" << endl;
  Platform::getInstance().init(this);


  theDebugServer.start(6401, true);
}

NaoMotionController::~NaoMotionController()
{
}


void NaoMotionController::set(const MotorJointData& data)
{
  libNaothData.writing()->theMotorJointData = data;
  //theDCMHandler.setAllMotorData(data);
}

void NaoMotionController::getMotionInput()
{
  updateFrameInfo();
  //LibNaothData* libNaothDataWriting = libNaothData.writing();
  //theDCMHandler.readSensorData(libNaothDataWriting->timeStamp, libNaothDataWriting->sensorsValue);
  //libNaothDataReading = libNaothDataWriting;
  //libNaothData.swapWriting();
  NaoControllerBase<NaoMotionController>::getMotionInput();
}

void NaoMotionController::setMotionOutput()
{
  NaoControllerBase<NaoMotionController>::setMotionOutput();

  if ( naothData.swapReading() )
  {
    const NaothData* naothDataReading = naothData.reading();


    //theDCMHandler.setLED(naothDataReading->lEDData());
    //theDCMHandler.setIRSend(naothDataReading->iRSendData());
    //theDCMHandler.setUltraSoundSend(naothDataReading->ultraSoundSendData());
  }

}

void NaoMotionController::set(const LEDData& data)
{
  //theDCMHandler.setLED(data);
}

