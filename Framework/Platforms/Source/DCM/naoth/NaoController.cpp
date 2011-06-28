/**
 * @file NaoController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot for cognition
 *
 */

#include "NaoController.h"
#include <PlatformInterface/Platform.h>

using namespace naoth;

NaoController::NaoController()
:theSoundHandler(NULL),
theTeamComm(NULL)
{
  // read the value from file
  ifstream is(staticMemberPath.c_str());
  ASSERT(is.good());
  is>>theBodyID>>theBodyNickName;
  cout<<"bodyID: "<<theBodyID<<endl;
  cout<<"bodyNickName: "<<theBodyNickName<<endl;

  naothDataWriting = naothData.writing();
  
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
  registerInput<GameData>(*this);
  registerInput<TeamMessageDataIn>(*this);

  // register output
  registerOutput<const CameraSettingsRequest>(*this);
  registerOutput<const LEDData>(*this);
  registerOutput<const IRSendData>(*this);
  registerOutput<const UltraSoundSendData>(*this);
  registerOutput<const SoundPlayData>(*this);
  registerOutput<const TeamMessageDataOut>(*this);
  registerOutput<const GameReturnData>(*this);
  
  cout<<"Init Platform"<<endl;
  Platform::getInstance().init(this);
  
  std::cout << "Init CameraHandler" << endl;
  theCameraHandler.init("/dev/video");
  
  std::cout << "Init SoundHandler" <<endl;
  theSoundHandler = new SoundControl();

  std::cout<< "Init TeamComm"<<endl;
  theTeamComm = new TeamCommunicator();

  std::cout<< "Init SPLGameController"<<endl;
  theGameController = new SPLGameController();
}

NaoController::~NaoController()
{
  if (theSoundHandler!=NULL)
  {
    delete theSoundHandler;
  }

  if ( theTeamComm != NULL)
  {
    delete theTeamComm;
  }

  if ( theGameController != NULL )
  {
    delete theGameController;
  }
}

void NaoController::get(Image& data)
{
  theCameraHandler.get(data);
}

void NaoController::set(const CameraSettingsRequest& data)
{
  theCameraHandler.setCameraSettings(data, data.queryCameraSettings);
}

void NaoController::get(CurrentCameraSettings& data)
{
  theCameraHandler.getCameraSettings(data);
}

void NaoController::set(const SoundPlayData& data)
{
  theSoundHandler->setSoundData(data);
}

void NaoController::getCognitionInput()
{
  if ( libNaothData.swapReading() )
  {
    libNaothDataReading = libNaothData.reading();
  }
  else
  {
    // didn't get new sensor data
    libNaothDataReading = NULL;
  }

  updateFrameInfo();
  NaoControllerBase<NaoController>::getCognitionInput();
}
  
void NaoController::setCognitionOutput()
{
  NaoControllerBase<NaoController>::setCognitionOutput();
  
  naothData.swapWriting();
  naothDataWriting = naothData.writing();
}

void NaoController::get(TeamMessageDataIn& data)
{
  theTeamComm->receive(data.data);
}

void NaoController::set(const TeamMessageDataOut& data)
{
  theTeamComm->send(data.data);
}

void NaoController::get(GameData& data)
{
  if ( theGameController->get(data, theFrameInfo.getTime()) )
  {
    data.frameNumber = theFrameInfo.getFrameNumber();
  }
}

void NaoController::set(const GameReturnData& data)
{
  theGameController->setReturnData(data);
}
