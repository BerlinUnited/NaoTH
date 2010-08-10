/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "PlatformInterface.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Tools/NaoTime.h"

PlatformInterface::PlatformInterface(const std::string& name, unsigned int basicTimeStep)
:platformName(name),
  theBasicTimeStep(basicTimeStep),
  lastUltraSoundSendTime(0),
  lastUltraSoundReceiveTime(1)
{
  theCognition.theSensorJointData = NULL;
  theCognition.theFrameInfo = NULL;
  theCognition.theAccelerometerData = NULL;
  theCognition.theImage = NULL;
  theCognition.theFSRData = NULL;
  theCognition.theGyrometerData = NULL;
  theCognition.theInertialSensorData = NULL;
  theCognition.theBumperData = NULL;
  theCognition.theIRReceiveData = NULL;
  theCognition.theCurrentCameraSettings = NULL;
  theCognition.theButtonData = NULL;
  theCognition.theBatteryData = NULL;
  theCognition.theUltraSoundReceiveData = NULL;

  theCognition.theCameraSettingsRequest = NULL;
  theCognition.theLEDData = NULL;
  theCognition.theIRSendData = NULL;
  theCognition.theUltraSoundSendData = NULL;
  theCognition.theSoundData = NULL;

  theMotion.theFrameInfo = NULL;
  theMotion.theSensorJointData = NULL;
  theMotion.theAccelerometerData = NULL;
  theMotion.theFSRData = NULL;
  theMotion.theInertialSensorData = NULL;
  theMotion.theBumperData = NULL;
  theMotion.theGyrometerData = NULL;

  theMotion.theMotorJointData = NULL;

  cout<<"NaoTH "<<platformName<<" starting..."<<endl;
}

PlatformInterface::~PlatformInterface()
{

}

void PlatformInterface::run()
{
  
}

#define REG_INPUT(P,T) \
  if (dynamic_cast<T*>(data)!=NULL) \
  { \
    cout<< platformName<<" register "#P" input: "#T<<endl; \
    assert( NULL == the##P.the##T ); \
    the##P.the##T = dynamic_cast<T*>(data); \
    return true; \
  }

bool PlatformInterface::registerCognitionInput(PlatformInterchangeable* data, const std::string& /*name*/)
{
  REG_INPUT(Cognition,SensorJointData);
  REG_INPUT(Cognition,FrameInfo);
  REG_INPUT(Cognition,AccelerometerData);
  REG_INPUT(Cognition,Image);
  REG_INPUT(Cognition,FSRData);
  REG_INPUT(Cognition,GyrometerData);
  REG_INPUT(Cognition,InertialSensorData);
  REG_INPUT(Cognition,BumperData);
  REG_INPUT(Cognition,IRReceiveData);
  REG_INPUT(Cognition,CurrentCameraSettings);
  REG_INPUT(Cognition,ButtonData);
  REG_INPUT(Cognition,BatteryData);
  REG_INPUT(Cognition,UltraSoundReceiveData);

  THROW("PlatformInterface::registerCognitionInput failed!!");
  return false;
}

bool PlatformInterface::registerMotionInput(PlatformInterchangeable* data, const std::string& /*name*/)
{
  REG_INPUT(Motion,FrameInfo);
  REG_INPUT(Motion,SensorJointData);
  REG_INPUT(Motion,FSRData);
  REG_INPUT(Motion,AccelerometerData);
  REG_INPUT(Motion,InertialSensorData);
  REG_INPUT(Motion,BumperData);
  REG_INPUT(Motion,GyrometerData);

  THROW("PlatformInterface::registerMotionInput failed!!");
  return false;
}

void PlatformInterface::getCognitionInput()
{
  ASSERT(NULL!=theCognition.theSensorJointData);
  get(*theCognition.theSensorJointData);

  ASSERT(NULL!=theCognition.theFrameInfo);
  get(*theCognition.theFrameInfo);

  ASSERT(NULL!=theCognition.theAccelerometerData);
  get(*theCognition.theAccelerometerData);

  ASSERT(NULL!=theCognition.theImage);
  get(*theCognition.theImage);

  ASSERT(NULL!=theCognition.theFSRData);
  get(*theCognition.theFSRData);

  ASSERT(NULL!=theCognition.theGyrometerData);
  get(*theCognition.theGyrometerData);

  ASSERT(NULL!=theCognition.theInertialSensorData);
  get(*theCognition.theInertialSensorData);

  ASSERT(NULL!=theCognition.theBumperData);
  get(*theCognition.theBumperData);

  ASSERT(NULL!=theCognition.theIRReceiveData);
  get(*theCognition.theIRReceiveData);

  ASSERT(NULL!=theCognition.theCurrentCameraSettings);
  get(*theCognition.theCurrentCameraSettings);

  ASSERT(NULL!=theCognition.theButtonData);
  get(*theCognition.theButtonData);

  ASSERT(NULL!=theCognition.theBatteryData);
  get(*theCognition.theBatteryData);

  ASSERT(NULL!=theCognition.theUltraSoundReceiveData);
  unsigned int currentTime = NaoTime::getNaoTimeInMilliSeconds();
//  if((currentTime - lastUltraSoundReceiveTime) > theCognition.theUltraSoundReceiveData->ultraSoundTimeStep)
  if(lastUltraSoundSendTime > lastUltraSoundReceiveTime && (currentTime - lastUltraSoundSendTime) > theCognition.theUltraSoundReceiveData->ultraSoundTimeStep)//x ms after send the us wave
  {
    get(*theCognition.theUltraSoundReceiveData);
    lastUltraSoundReceiveTime = currentTime;
  }
}

void PlatformInterface::getMotionInput()
{
  ASSERT(NULL!=theMotion.theFrameInfo);
  get(*theMotion.theFrameInfo);

  ASSERT(NULL!=theMotion.theSensorJointData);
  get(*theMotion.theSensorJointData);

  ASSERT(NULL!=theMotion.theAccelerometerData);
  get(*theMotion.theAccelerometerData);

  ASSERT(NULL!=theMotion.theFSRData);
  get(*theMotion.theFSRData);

  ASSERT(NULL!=theMotion.theInertialSensorData);
  get(*theMotion.theInertialSensorData);

  ASSERT(NULL!=theMotion.theBumperData);
  get(*theMotion.theBumperData);

  ASSERT(NULL!=theMotion.theGyrometerData);
  get(*theMotion.theGyrometerData);
}

void PlatformInterface::setCognitionOutput()
{
  ASSERT(NULL!=theCognition.theCameraSettingsRequest);
  set(*theCognition.theCameraSettingsRequest);

  ASSERT(NULL!=theCognition.theLEDData);
  if (theCognition.theLEDData->change)
  {
    set(*theCognition.theLEDData);
  }

  ASSERT(NULL!=theCognition.theIRSendData);
  if (theCognition.theIRSendData->changed)
  {
    set(*theCognition.theIRSendData);
  }

  ASSERT(NULL!=theCognition.theUltraSoundSendData);
  unsigned int currentTime = NaoTime::getNaoTimeInMilliSeconds();
//  if((currentTime - lastUltraSoundSendTime) > theCognition.theUltraSoundSendData->ultraSoundTimeStep)
  if(lastUltraSoundReceiveTime > lastUltraSoundSendTime && (currentTime - lastUltraSoundReceiveTime) > theCognition.theUltraSoundSendData->ultraSoundTimeStep) // x ms after receiving last wave reflections
  {
    set(*theCognition.theUltraSoundSendData);
    lastUltraSoundSendTime = currentTime;
//    ultraSoundFrameNumber++;
  }

  ASSERT(NULL!=theCognition.theSoundData);
  if (theCognition.theSoundData->soundFile != "")
  {
    set(*theCognition.theSoundData);
  }
}

void PlatformInterface::setMotionOutput()
{
  ASSERT(NULL!=theMotion.theMotorJointData);
  set(*theMotion.theMotorJointData);
}

#define REG_OUTPUT(P,T) \
  if (dynamic_cast<const T*>(data)!=NULL) \
  { \
    cout<<platformName<<" register "#P" output: "#T<<endl; \
    assert(NULL == the##P.the##T); \
    the##P.the##T = dynamic_cast<const T*>(data); \
    return true; \
  }

bool PlatformInterface::registerCognitionOutput(const PlatformInterchangeable* data, const std::string& /*name*/)
{
  
  REG_OUTPUT(Cognition, CameraSettingsRequest);
  REG_OUTPUT(Cognition, LEDData);
  REG_OUTPUT(Cognition, IRSendData);
  REG_OUTPUT(Cognition, UltraSoundSendData);
  REG_OUTPUT(Cognition, SoundData);

  THROW("PlatformInterface::registerCognitionOutput failed!");
  return false;
}

bool PlatformInterface::registerMotionOutput(const PlatformInterchangeable* data, const std::string& /*name*/)
{
  REG_OUTPUT(Motion, MotorJointData);

  THROW("PlatformInterface::registerMotionOutput failed!");
  return false;
}
