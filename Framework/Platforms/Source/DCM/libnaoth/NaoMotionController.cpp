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
#include <unistd.h>

using namespace naoth;

NaoMotionController::NaoMotionController()
  : lastTimeChestButtonNotPressed(0), shutdownRequested(false), shutdownBlinker(true)
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
  registerOutput<const MotorJointData>(*this);

  theDebugServer.start(6401, true);
}

NaoMotionController::~NaoMotionController()
{
}

void NaoMotionController::init(ALPtr<ALBroker> pB)
{ 
  std::cout << "Init DCMHandler" << endl;
  theDCMHandler.init(pB);
  
  // save the body ID
  theBodyID = theDCMHandler.getBodyID();
  std::cout << "bodyID: "<< theBodyID << endl;
  
  // save the nick name
  theBodyNickName = theDCMHandler.getBodyNickName();
  std::cout << "nickName: "<< theBodyNickName << endl;

  // save the value to file
  ofstream os(staticMemberPath.c_str());
  ASSERT(os.good());
  os<<theBodyID<<"\n"<<theBodyNickName<<endl;

  std::cout << "Init Platform" << endl;
  Platform::getInstance().init(this);
}

void NaoMotionController::set(const MotorJointData& data)
{
  libNaothData.writing()->theMotorJointData = data;
  theDCMHandler.setAllMotorData(data);
}

void NaoMotionController::getMotionInput()
{
  updateFrameInfo();
  LibNaothData* libNaothDataWriting = libNaothData.writing();
  theDCMHandler.readSensorData(libNaothDataWriting->timeStamp, libNaothDataWriting->sensorsValue);
  libNaothDataReading = libNaothDataWriting;
  libNaothData.swapWriting();
  NaoControllerBase<NaoMotionController>::getMotionInput();

  // check if chest button was pressed as shutdown request
  if(!shutdownRequested)
  {
    if(*(theDCMHandler.sensorPtrs[theButtonDataIndex + ButtonData::Chest]) == 0.0f)
    {
      lastTimeChestButtonNotPressed = theFrameInfo.getTime();
    }
    if((theFrameInfo.getTime() - lastTimeChestButtonNotPressed) > 3000)
    {
      shutdownRequested = true;

      std::cerr << "request shutdown: halt" << std::endl;
      system("shutdown -h now");

    }
  }
}

void NaoMotionController::setMotionOutput()
{
  NaoControllerBase<NaoMotionController>::setMotionOutput();

  if ( naothData.swapReading() )
  {
    const NaothData* naothDataReading = naothData.reading();


    if(!shutdownRequested)
    {
      theDCMHandler.setLED(naothDataReading->lEDData());
    }
    theDCMHandler.setIRSend(naothDataReading->iRSendData());
    theDCMHandler.setUltraSoundSend(naothDataReading->ultraSoundSendData());
  }


  if(shutdownRequested)
  {
    if(theFrameInfo.getFrameNumber() % 10 == 0)
    {
      shutdownBlinker = !shutdownBlinker;
    }

    LEDData overrrideLEDData;
    if(shutdownBlinker)
    {
      overrrideLEDData.theMultiLED[LEDData::ChestButton][LEDData::RED] = 1.0;
      overrrideLEDData.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 0.0;
      overrrideLEDData.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 1.0;
    }
    else
    {
      overrrideLEDData.theMultiLED[LEDData::ChestButton][LEDData::RED] = 0.2;
      overrrideLEDData.theMultiLED[LEDData::ChestButton][LEDData::GREEN] = 0.0;
      overrrideLEDData.theMultiLED[LEDData::ChestButton][LEDData::BLUE] = 0.2;
    }
    theDCMHandler.setLED(overrrideLEDData);
  }

}

void NaoMotionController::set(const LEDData& data)
{
  theDCMHandler.setLED(data);
}

