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
  :
  theDebugServer(NULL)
{
  // read the value from file
  ifstream is(staticMemberPath.c_str());
  ASSERT(is.good());
  is>>theBodyID>>theBodyNickName;
  cout<<"bodyID: "<<theBodyID<<endl;
  cout<<"bodyNickName: "<<theBodyNickName<<endl;

  //naothDataWriting = naothData.writing();

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

  // debug
  registerInput<DebugMessageIn>(*this);
  registerOutput<const DebugMessageOut>(*this);

  std::cout << "Init Platform" << endl;
  Platform::getInstance().init(this);
}

NaoMotionController::~NaoMotionController()
{
}


void NaoMotionController::set(const MotorJointData& data)
{
  naoCommandData.writing()->set(data);
  //theDCMHandler.setAllMotorData(data);
}

void NaoMotionController::getMotionInput()
{
  if ( naoSensorData.swapReading() )
  {
    sensorDataReading = naoSensorData.reading();
  }
  else
  {
    // didn't get new sensor data
    sensorDataReading = NULL;
  }

  updateFrameInfo();
  NaoControllerBase<NaoMotionController>::getMotionInput();

  //updateFrameInfo();
  //LibNaothData* libNaothDataWriting = libNaothData.writing();
  //theDCMHandler.readSensorData(libNaothDataWriting->timeStamp, libNaothDataWriting->sensorsValue);
  //libNaothDataReading = libNaothDataWriting;
  //libNaothData.swapWriting();
  //NaoControllerBase<NaoMotionController>::getMotionInput();
}//end getMotionInput

void NaoMotionController::setMotionOutput()
{
  /*
  NaoControllerBase<NaoMotionController>::setMotionOutput();

  if ( naothData.swapReading() )
  {
    const NaothData* naothDataReading = naothData.reading();


    //theDCMHandler.setLED(naothDataReading->lEDData());
    //theDCMHandler.setIRSend(naothDataReading->iRSendData());
    //theDCMHandler.setUltraSoundSend(naothDataReading->ultraSoundSendData());
  }
  */

  NaoControllerBase<NaoMotionController>::setMotionOutput();
  
  naoCommandData.swapWriting();
}//end setMotionOutput

void NaoMotionController::set(const LEDData& data)
{
  //theDCMHandler.setLED(data);
}

