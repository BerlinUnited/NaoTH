/**
 * @file NaoController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief Interface for the real robot for both cognition and motion
 *
 */

#include "NaoController.h"

#include "PlatformInterface/Platform.h"
#include <algorithm>

using namespace std;
using namespace naoth;

#define TEAMCOMM_MAX_MSG_SIZE 4096

NaoController::NaoController()
    :
    lolaAvailable(false),
    theSoundHandler(NULL),
    theTeamCommSender(NULL),
    theTeamCommListener(NULL),
    theRemoteCommandListener(NULL),
    theDebugServer(NULL)
{
  if(fileExists("/usr/bin/lola") || fileExists("/opt/aldebaran/bin/lola"))
  {
    lolaAvailable = true;
  }

  // init shared memory
  // sensor data
  const std::string naoSensorDataPath = "/nao_sensor_data";
  // command data
  const std::string naoCommandMotorJointDataPath = "/nao_command.MotorJointData";
  const std::string naoCommandUltraSoundSendDataPath = "/nao_command.UltraSoundSendData";
  const std::string naoCommandLEDDataPath = "/nao_command.LEDData";

  naoSensorData.open(naoSensorDataPath);

  naoCommandMotorJointData.open(naoCommandMotorJointDataPath);
  naoCommandUltraSoundSendData.open(naoCommandUltraSoundSendDataPath);
  naoCommandLEDData.open(naoCommandLEDDataPath);
  // end init shared memory

  char hostname[128];
  hostname[127] = '\0';
  gethostname(hostname, 127);
  theRobotName = string(hostname);
  cout << "[NaoController] " << "RobotName: " << theRobotName << endl;
  //theWhistleDetector.setRobotName(theRobotName);

  // read the theBodyID and the theBodyNickName from file "nao.info"
  const std::string naoInfoPath = Platform::getInstance().theConfigDirectory + "nao.info";
  ifstream is(naoInfoPath.c_str());
  if(!is.good())
  {
    // exit the program
    THROW(
		"is.good() failed. File 'Config/nao.info' found."
		"Hint: 'nao.info' is generated by NaoSMAL, you have to run it at least once."
		"Do you call bin/naoth from /home/nao?"
		);
  }
  else
  {
    is >> theBodyID >> theBodyNickName;
    cout << "[NaoController] " << "bodyID: " << theBodyID << endl;
    cout << "[NaoController] " << "bodyNickName: " << theBodyNickName << endl;
  }

  // read the mac address of the LAN adaptor
  ifstream isEthMac("/sys/class/net/eth0/address");
  isEthMac >> theHeadNickName;
  std::replace(theHeadNickName.begin(), theHeadNickName.end(), ':', '_');
  cout << "[NaoController] " << "headNickName: " << theHeadNickName << endl;


  /*  REGISTER IO  */
  // camera
  registerInput<Image>(*this);
  registerInput<ImageTop>(*this);
  registerInput<CurrentCameraSettings>(*this);
  registerInput<CurrentCameraSettingsTop>(*this);
  registerOutput<const CameraSettingsRequest>(*this);
  registerOutput<const CameraSettingsRequestTop>(*this);

  // sound
  registerOutput<const SoundPlayData>(*this);

  // gamecontroller
  registerInput<GameData>(*this);
  registerOutput<const GameReturnData>(*this);

  // teamcomm
  registerInput<TeamMessageDataIn>(*this);
  registerOutput<const TeamMessageDataOut>(*this);

  registerInput<RemoteMessageDataIn>(*this);

  // debug comm
  registerInput<DebugMessageInCognition>(*this);
  registerInput<DebugMessageInMotion>(*this);
  registerOutput<const DebugMessageOut>(*this);

  // time
  registerInput<FrameInfo>(*this);

  // register sensor input
  registerInput<AccelerometerData>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<ButtonData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<UltraSoundReceiveData>(*this);
  registerInput<AudioData>(*this);
  registerInput<CpuData>(*this);

  // register command output
  registerOutput<const MotorJointData>(*this);
  registerOutput<const LEDData>(*this);
  registerOutput<const UltraSoundSendData>(*this);
  registerOutput<const AudioControl>(*this);

  /*  INIT DEVICES  */
  std::cout << "[NaoController] " << "Init Platform" << endl;
  Platform::getInstance().init(this);

  std::cout << "[NaoController] " << "Init SoundHandler" <<endl;
  theSoundHandler = new SoundControl();

  // create the teamcomm
  std::cout << "[NaoController] " << "Init TeamComm" << endl;
  const naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  string interfaceName = "wlan0";
  if(config.hasKey("teamcomm", "interface"))
  {
    interfaceName = config.getString("teamcomm", "interface");
  }
  int teamcomm_port = 10700; // default port
  config.get("teamcomm", "port", teamcomm_port);
  theTeamCommSender = new BroadCaster(interfaceName, teamcomm_port);
  theTeamCommListener = new UDPReceiver(teamcomm_port, TEAMCOMM_MAX_MSG_SIZE);

  theRemoteCommandListener = new UDPReceiver(10401, 4096);

  // start the debug server at the default debug port
  std::cout << "[NaoController] " << "Init DebugServer" << endl;
  int debug_port = 5401; // default port
  config.get("network", "debug_port", debug_port);
  theDebugServer = new DebugServer();
  theDebugServer->start(static_cast<unsigned short>(debug_port));


  std::cout << "[NaoController] " << "Init SPLGameController"<<endl;
  theGameController = new SPLGameController();

  // HACK: we are in NAO V6
  if(lolaAvailable)
  {
    std::cout << "[NaoController] " << "Init CameraHandler V6 (bottom)" << std::endl;
    theBottomCameraHandler.init("/dev/video1", CameraInfo::Bottom, true);
    std::cout << "[NaoController] " << "Init CameraHandler V6 (top)" << std::endl;
    theTopCameraHandler.init("/dev/video0", CameraInfo::Top, true);
  } else {
    std::cout << "[NaoController] " << "Init CameraHandler V5 (bottom)" << std::endl;
    theBottomCameraHandler.init("/dev/video1", CameraInfo::Bottom, false);
    std::cout << "[NaoController] " << "Init CameraHandler V5 (top)" << std::endl;
    theTopCameraHandler.init("/dev/video0", CameraInfo::Top, false);
  }
}

NaoController::~NaoController()
{
  std::cout << "[NaoController] destruct" << std::endl;
  delete theSoundHandler;
  delete theTeamCommSender;
  delete theTeamCommListener;
  delete theGameController;
  delete theDebugServer;
}

void NaoController::set(const CameraSettingsRequest &request)
{
  CameraSettings settings = request.getCameraSettings();
  theBottomCameraHandler.setAllCameraParams(settings);
}

void NaoController::set(const CameraSettingsRequestTop &request)
{
  CameraSettings settings = request.getCameraSettings();
  theTopCameraHandler.setAllCameraParams(settings);
}
