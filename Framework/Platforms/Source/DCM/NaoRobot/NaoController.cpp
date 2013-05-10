/**
 * @file NaoController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief Interface for the real robot for both cognition and motion
 *
 */

#include "NaoController.h"

#include <algorithm>

using namespace std;
using namespace naoth;

NaoController::NaoController()
    :
    PlatformInterface("Nao", 10),
    theSoundHandler(NULL),
    theBroadCaster(NULL),
    theBroadCastListener(NULL),
    theDebugServer(NULL),
    theRCTCBroadCaster(NULL),
    theRCTCBroadCastListener(NULL)
{
  // init shared memory
  // sensor data
  const std::string naoSensorDataPath = "/nao_sensor_data";
  // command data
  const std::string naoCommandMotorJointDataPath = "/nao_command.MotorJointData";
  const std::string naoCommandUltraSoundSendDataPath = "/nao_command.UltraSoundSendData";
  const std::string naoCommandIRSendDataPath = "/nao_command.IRSendData";
  const std::string naoCommandLEDDataPath = "/nao_command.LEDData";

  naoSensorData.open(naoSensorDataPath);

  naoCommandMotorJointData.open(naoCommandMotorJointDataPath);
  naoCommandUltraSoundSendData.open(naoCommandUltraSoundSendDataPath);
  naoCommandIRSendData.open(naoCommandIRSendDataPath);
  naoCommandLEDData.open(naoCommandLEDDataPath);
  // end init shared memory
  


  // read the theBodyID and the theBodyNickName from file "nao.info"
  const std::string naoInfoPath = Platform::getInstance().theConfigDirectory + "nao.info";
  ifstream is(naoInfoPath.c_str());
  if(!is.good())
  {
    // exit the program
    THROW("is.good() failed. No Configs found. Do you call ../bin/naoth from ~/naoqi?");
  }
  else
  {
    is >> theBodyID >> theBodyNickName;
    cout << "bodyID: " << theBodyID << endl;
    cout << "bodyNickName: " << theBodyNickName << endl;
  }

  // read the mac address of the LAN adaptor
  ifstream isEthMac("/sys/class/net/eth0/address");    
  isEthMac >> theHeadNickName;
  std::replace(theHeadNickName.begin(), theHeadNickName.end(), ':', '_');    
  cout << "headNickName: " << theHeadNickName << endl;



  /*  REGISTER IO  */
  // camera
  registerInput<Image>(*this);
  registerInput<Image2>(*this);
  registerInput<CurrentCameraSettings>(*this);
  registerOutput<const CameraSettingsRequest>(*this);
    
  // sound
  registerOutput<const SoundPlayData>(*this);

  // gamecontroller
  registerInput<GameData>(*this);
  registerOutput<const GameReturnData>(*this);
    
  // teamcomm
  registerInput<TeamMessageDataIn>(*this);
  registerOutput<const TeamMessageDataOut>(*this);

  // rctc teamcomm
  registerInput<RCTCTeamMessageDataIn>(*this);
  registerOutput<const RCTCTeamMessageDataOut>(*this);

  // debug comm
  registerInput<DebugMessageIn>(*this);
  registerOutput<const DebugMessageOut>(*this);

  // time
  registerInput<FrameInfo>(*this);

  // register sensor input
  registerInput<AccelerometerData>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<IRReceiveData>(*this);
  registerInput<ButtonData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<UltraSoundReceiveData>(*this);

  // register command output
  registerOutput<const MotorJointData>(*this);
  registerOutput<const LEDData>(*this);
  registerOutput<const IRSendData>(*this);
  registerOutput<const UltraSoundSendData>(*this);


  /*  INIT DEVICES  */
  std::cout << "Init Platform" << endl;
  Platform::getInstance().init(this);

  std::cout << "Init SoundHandler" <<endl;
  //theSoundPlayer.play("penalized");
  theSoundHandler = new SoundControl();

  // create the teamcomm
  std::cout << "Init TeamComm" << endl;
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  string interfaceName = "wlan0";
  if(config.hasKey("teamcomm", "interface"))
  {
    interfaceName = config.getString("teamcomm", "interface");
  }
  int teamcomm_port = 10700; // default port
  config.get("teamcomm", "port", teamcomm_port);
  theBroadCaster = new BroadCaster(interfaceName, teamcomm_port);
  theBroadCastListener = new BroadCastListener(teamcomm_port, TEAMCOMM_MAX_MSG_SIZE);


  // create RCTC connections
  int rctc_port = 22022; // default port
  config.get("teamcomm", "rctc_port", rctc_port);
  theRCTCBroadCaster = new BroadCaster(interfaceName, rctc_port);
  theRCTCBroadCastListener = new BroadCastListener(rctc_port, rctc::PACKET_SIZE);


  // start the debug server at the default debug port
  std::cout << "Init DebugServer" << endl;
  int debug_port = 5401; // default port
  config.get("network", "debug_port", debug_port);
  theDebugServer = new DebugServer();
  theDebugServer->start(debug_port, true);


  std::cout<< "Init SPLGameController"<<endl;
  theGameController = new SPLGameController();
}

NaoController::~NaoController()
{
  delete theSoundHandler;
  delete theBroadCaster;
  delete theBroadCastListener;
  delete theGameController;
  delete theDebugServer;
  delete theRCTCBroadCaster;
  delete theRCTCBroadCastListener;
}

void NaoController::set(const CameraSettingsRequest& data)
{
  bool somethingChanged = false;
  if(theBottomCameraHandler.isRunning())
  {
    CurrentCameraSettings current;
    theBottomCameraHandler.getCameraSettings(current, data.queryCameraSettings);

    if(data.queryCameraSettings)
    {
      somethingChanged = false;
    }
    else
    {
      for(int i=0; i < CameraSettings::numOfCameraSetting; i++)
      {
        if(current.data[i] != data.data[i])
        {
          std::cout << "CameraParameter " <<
                        CameraSettings::getCameraSettingsName((CameraSettings::CameraSettingID) i)
                    << " was requested to change from " << current.data[i]
                    << " to " << data.data[i] << std::endl;
          somethingChanged = true;
//           break;
        }
      }
    }
  }
  else
  {
    somethingChanged = true;
  }

  if(somethingChanged)
  {
    std::cout << "Init CameraHandler and settting camera settings (bottom)" << endl;
    theBottomCameraHandler.init(data, "/dev/video1", CameraInfo::Bottom, true);
    std::cout << "Init CameraHandler and settting camera settings (top)" << endl;
    theTopCameraHandler.init(data, "/dev/video0", CameraInfo::Top, false);
  }
}//end set CameraSettingsRequest


void NaoController::get(RCTCTeamMessageDataIn& data) 
{ 
  data.data.clear();
  std::vector<std::string> msg_vector;
  theRCTCBroadCastListener->receive(msg_vector);
  for(unsigned int i = 0; i < msg_vector.size(); i++)
  {
    const char* bin_msg = msg_vector[i].c_str();
    rctc::Message msg;
    if(rctc::binaryToMessage((const uint8_t*)bin_msg, msg))
    {
      data.data.push_back(msg);
    }
  }
}//end get RCTCTeamMessageDataIn


void NaoController::set(const RCTCTeamMessageDataOut& data)
{
  if(data.valid)
  {
    uint8_t bin_msg[rctc::PACKET_SIZE];
    rctc::messageToBinary(data.data, bin_msg);
    std::string msg((char*)bin_msg, rctc::PACKET_SIZE);
//      std::cout << "sending RCTC " <<theRCTCBroadCaster->broadcastAddress << " (bcast adress) " << std::endl;
    theRCTCBroadCaster->send(msg);
  }
}//end set RCTCTeamMessageDataOut
