/**
 * @file NaoController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the real robot for cognition
 *
 */

#include "NaoController.h"
#include <PlatformInterface/Platform.h>
#include <Tools/Debug/Stopwatch.h>

#define TEAMCOMM_MAX_MSG_SIZE 4096

using namespace naoth;

NaoController::NaoController()
:theSoundHandler(NULL),
theBroadCaster(NULL),
theBroadCastListener(NULL)
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
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  string interfaceName = "wlan0";
  if(config.hasKey("teamcomm", "interface"))
  {
    interfaceName = config.getString("teamcomm", "interface");
  }

  unsigned int port = 10700;
  if(config.hasKey("teamcomm", "port"))
  {
    port = config.getInt("teamcomm", "port");
  }

  theBroadCaster = new BroadCaster(interfaceName, port);
  theBroadCastListener = new BroadCastListener(port, TEAMCOMM_MAX_MSG_SIZE);

  std::cout<< "Init SPLGameController"<<endl;
  theGameController = new SPLGameController();

  theDebugServer.start(5401, true);
}

NaoController::~NaoController()
{
  if (theSoundHandler!=NULL)
  {
    delete theSoundHandler;
  }

  if ( theBroadCaster != NULL)
  {
    delete theBroadCaster;
  }

  if ( theBroadCastListener != NULL )
  {
    delete theBroadCastListener;
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
  STOPWATCH_START("getCognitionInput");
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
  STOPWATCH_STOP("getCognitionInput");
}
  
void NaoController::setCognitionOutput()
{
  STOPWATCH_START("setCognitionOutput");
  NaoControllerBase<NaoController>::setCognitionOutput();
  
  naothData.swapWriting();
  naothDataWriting = naothData.writing();
  STOPWATCH_STOP("setCognitionOutput");
}

void NaoController::get(TeamMessageDataIn& data)
{
  theBroadCastListener->receive(data.data);
}

void NaoController::set(const TeamMessageDataOut& data)
{
  if ( data.data.size() <= TEAMCOMM_MAX_MSG_SIZE )
  {
    theBroadCaster->send(data.data);
  }
  else
  {
    cerr<<"NaoController: TeamMessageDataOut is too big "<<data.data.size()<<endl;
  }
}

void NaoController::get(GameData& data)
{
  if ( theGameController->get(data, theFrameInfo.getTime()) )
  {
    data.valid = true;
    data.frameNumber = theFrameInfo.getFrameNumber();
  }
  else
  {
    data.valid = false;
  }
}

void NaoController::set(const GameReturnData& data)
{
  theGameController->setReturnData(data);
}
