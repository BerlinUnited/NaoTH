/**
 * @file BoosterController.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief Interface for the real robot for both cognition and motion
 *
 */

#include "BoosterController.h"

#include "PlatformInterface/Platform.h"
#include <algorithm>

//#include "Tools/NaoInfoTools.h"

using namespace std;
using namespace naoth;

#define TEAMCOMM_MAX_MSG_SIZE 4096

BoosterController::BoosterController()
{
  // init shared memory
  // sensor data
  //naoSensorData.open("/nao_sensor_data");
  
  // command data
  //naoCommandMotorJointData.open("/nao_command.MotorJointData");
  //naoCommandUltraSoundSendData.open("/nao_command.UltraSoundSendData");
  //naoCommandLEDData.open("/nao_command.LEDData");
  // end init shared memory

  //std::cout << "[BoosterController] Read robotInfo from /tmp/nao.info:" << std::endl;
  //readNaoInfo();
  //robotInfo.print(std::cout);
  
  robotInfo.bodyId        = "boo41";
  robotInfo.bodyNickName  = "boo41";
  robotInfo.headId        = "boo41";
  robotInfo.robotName     = "boo41";
  robotInfo.platform      = "Booster-K1";
  robotInfo.basicTimeStep = 20;

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
  // register team debug
  registerOutput<const TeamMessageDebug>(*this);
  
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
  std::cout << "[BoosterController] " << "Init Platform" << endl;
  Platform::getInstance().init(this);

  // shortcut for configuration
  const naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;

  /* 
  * ------------
  *   NETWORK
  * ------------
  */

  // create the teamcomm
  /*
  std::cout << "[BoosterController] " << "Init TeamComm" << endl;
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
  */

  // start the debug server at the default debug port
  std::cout << "[NaoController] " << "Init DebugServer" << endl;
  int debug_port = 5401; // default port
  config.get("network", "debug_port", debug_port);
  theDebugServer = new DebugServer();
  theDebugServer->start(static_cast<unsigned short>(debug_port));
  
  /*
  string debugIp = "127.0.0.1";
  unsigned int debugPort = 10704;
  if(config.hasKey("TeamCommDebugger", "host"))
  {
    debugIp = config.getString("TeamCommDebugger", "host");
  }
  if(config.hasKey("TeamCommDebugger", "port") && config.getInt("TeamCommDebugger", "port") > 0)
  {
    debugPort = (unsigned int) config.getInt("TeamCommDebugger", "port");
  }
  theTeamCommDebugger = new UDPSender(debugIp, debugPort, "TeamCommDebugger");

  std::cout << "[BoosterController] " << "Init SPLGameController"<<endl;
  theGameController = new SPLGameController();
  */


  /* 
  * ------------
  *   Image
  * ------------
  */

  // activate the image bridge
  imageBridge.connectSocket();
  if(imageBridge.hasError()) {
    exit(-1);
  }
}

BoosterController::~BoosterController()
{
  std::cout << "[BoosterController] destruct" << std::endl;

  delete theTeamCommDebugger;
  delete theTeamCommSender;
  delete theTeamCommListener;
  delete theRemoteCommandListener;
  delete theGameController;
  delete theDebugServer;
}

/*
void BoosterController::readNaoInfo() 
{
  const std::string naoInfoPath = "/tmp/nao.info";
  std::cout << "[NaoController] wait for /tmp/nao.info (must be created by lola_adaptor) ..." << std::endl;
  while(!FileUtils::fileExists(naoInfoPath)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  
  ifstream is(naoInfoPath);
  if(!is.good())
  {
    // exit the program
    THROW(
      "is.good() failed. File '/tmp/nao.info' found."
      "Hint: 'nao.info' is generated by LolaAdaptor, you have to run it at least once."
    );
  }
  else
  {
    is >> robotInfo.bodyId 
       >> robotInfo.bodyNickName
       >> robotInfo.headId
       >> robotInfo.robotName
       >> robotInfo.platform;
  }
  
  // TODO: this needs a better place
  robotInfo.basicTimeStep = 12; // for Nao6
}
*/