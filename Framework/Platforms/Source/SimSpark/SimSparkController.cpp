/**
 * @file SimSparkController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @brief Interface for the SimSpark simulator
 *
 */

#include "SimSparkController.h"
#include <iostream>
#include <fstream>
#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
#include <Tools/ImageProcessing/ColorModelConversions.h>
#include <Tools/StringTools.h>
//#include <Tools/Debug/DebugRequest.h>
#include <DebugCommunication/DebugCommandManager.h>

#include <Tools/ThreadUtil.h>
#include <Tools/NaoTime.h>
#include <Tools/NaoInfo.h>
#include <Tools/Math/Common.h>

#include <Tools/Communication/ASCIIEncoder.h>

using namespace std;

SimSparkController::SimSparkController(const std::string& name)
  : 
  thePlatformName(name),
  theTeamMessageReceiveBuffer(NULL),
  theImageData(NULL),
  theImageSize(0),
  isNewImage(false),
  isNewVirtualVision(false),
  theCameraId(0),
  theSenseTime(0),
  theStepTime(0),
  theSyncMode(false),
  ignoreOpponentMsg(false),
  exiting(false)
{
  // register input
  registerInput<AccelerometerData>(*this);
  registerInput<FrameInfo>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<Image>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<BatteryData>(*this);
  registerInput<VirtualVision>(*this);
  registerInput<VirtualVisionTop>(*this);
  registerInput<TeamMessageDataIn>(*this);
  registerInput<GameData>(*this);
  registerInput<GPSData>(*this);
  registerInput<UltraSoundReceiveData>(*this);

  // register output
  registerOutput<const MotorJointData>(*this);
  registerOutput<const TeamMessageDataOut>(*this);

  // debug
  registerInput<DebugMessageInCognition>(*this);
  registerInput<DebugMessageInMotion>(*this);
  registerOutput<DebugMessageOut>(*this);

  // init the name -- id maps
  theJointSensorNameMap.clear();
  theJointSensorNameMap["hj1"] = JointData::HeadYaw;
  theJointSensorNameMap["hj2"] = JointData::HeadPitch;
  theJointSensorNameMap["laj1"] = JointData::LShoulderPitch;
  theJointSensorNameMap["laj2"] = JointData::LShoulderRoll;
  theJointSensorNameMap["laj3"] = JointData::LElbowYaw;
  theJointSensorNameMap["laj4"] = JointData::LElbowRoll;
  theJointSensorNameMap["llj1"] = JointData::LHipYawPitch;
  theJointSensorNameMap["llj2"] = JointData::LHipRoll;
  theJointSensorNameMap["llj3"] = JointData::LHipPitch;
  theJointSensorNameMap["llj4"] = JointData::LKneePitch;
  theJointSensorNameMap["llj5"] = JointData::LAnklePitch;
  theJointSensorNameMap["llj6"] = JointData::LAnkleRoll;
  theJointSensorNameMap["raj1"] = JointData::RShoulderPitch;
  theJointSensorNameMap["raj2"] = JointData::RShoulderRoll;
  theJointSensorNameMap["raj3"] = JointData::RElbowYaw;
  theJointSensorNameMap["raj4"] = JointData::RElbowRoll;
  theJointSensorNameMap["rlj1"] = JointData::RHipYawPitch;
  theJointSensorNameMap["rlj2"] = JointData::RHipRoll;
  theJointSensorNameMap["rlj3"] = JointData::RHipPitch;
  theJointSensorNameMap["rlj4"] = JointData::RKneePitch;
  theJointSensorNameMap["rlj5"] = JointData::RAnklePitch;
  theJointSensorNameMap["rlj6"] = JointData::RAnkleRoll;

  // motor names
  theJointMotorNameMap.clear();
  theJointMotorNameMap[JointData::HeadYaw] = "he1";
  theJointMotorNameMap[JointData::HeadPitch] = "he2";
  theJointMotorNameMap[JointData::LShoulderPitch] = "lae1";
  theJointMotorNameMap[JointData::LShoulderRoll] = "lae2";
  theJointMotorNameMap[JointData::LElbowYaw] = "lae3";
  theJointMotorNameMap[JointData::LElbowRoll] = "lae4";
  theJointMotorNameMap[JointData::LHipYawPitch] = "lle1";
  theJointMotorNameMap[JointData::LHipRoll] = "lle2";
  theJointMotorNameMap[JointData::LHipPitch] = "lle3";
  theJointMotorNameMap[JointData::LKneePitch] = "lle4";
  theJointMotorNameMap[JointData::LAnklePitch] = "lle5";
  theJointMotorNameMap[JointData::LAnkleRoll] = "lle6";
  theJointMotorNameMap[JointData::RShoulderPitch] = "rae1";
  theJointMotorNameMap[JointData::RShoulderRoll] = "rae2";
  theJointMotorNameMap[JointData::RElbowYaw] = "rae3";
  theJointMotorNameMap[JointData::RElbowRoll] = "rae4";
  theJointMotorNameMap[JointData::RHipYawPitch] = "rle1";
  theJointMotorNameMap[JointData::RHipRoll] = "rle2";
  theJointMotorNameMap[JointData::RHipPitch] = "rle3";
  theJointMotorNameMap[JointData::RKneePitch] = "rle4";
  theJointMotorNameMap[JointData::RAnklePitch] = "rle5";
  theJointMotorNameMap[JointData::RAnkleRoll] = "rle6";

  maxJointAbsSpeed = Math::fromDegrees(351.77);

  GError *err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);
  if (err)
  {
    socket = NULL;
    std::cout << "[WARN] Could not create a socket. This is a fatal error and communication is available. Error message:"
              << std::endl << err->message << std::endl;
    g_error_free (err);
  }

  // HACK: fixed size is bad
  theTeamMessageReceiveBuffer = new char[MAX_TEAM_MESSAGE_SIZE];
}

SimSparkController::~SimSparkController()
{
  if (socket != NULL) {
    g_socket_close(socket, NULL);
  }

  if (theImageData != NULL) {
    delete [] theImageData;
  }

  if(theTeamMessageReceiveBuffer != NULL) {
    delete [] theTeamMessageReceiveBuffer;
  }
}

string SimSparkController::getBodyNickName() const
{
  return theGameInfo.teamName + StringTools::toStr(theGameInfo.playerNumber);
}

string SimSparkController::getHeadNickName() const
{
  return theGameInfo.teamName + StringTools::toStr(theGameInfo.playerNumber);
}

string SimSparkController::getBodyID() const
{
  return StringTools::toStr(theGameInfo.playerNumber);
}

bool SimSparkController::connect(const std::string& host, int port)
{
  if(socket == NULL) {
    return false;
  }

  cout << "[SimSparkController] connecting to " << host << ":" << port << endl;

  gboolean conn = false;
  GCancellable* cancellable = NULL;
  GSocketAddress* sockaddr = NULL;
  GError* conn_error = NULL;
  GError* error = NULL;

  GSocketConnectable* addr = g_network_address_new(host.c_str(),static_cast<guint16> (port));
  GSocketAddressEnumerator* enumerator = g_socket_connectable_enumerate(addr);
  g_object_unref(addr);

  /**
  * Try each sockaddr until we succeed. Record the first
  * connection error, but not any further ones (since they'll probably
  * be basically the same as the first).
  */
  while (!conn && (sockaddr = g_socket_address_enumerator_next(enumerator, cancellable, &error)))
  {
    conn = g_socket_connect(socket, sockaddr, NULL, conn_error ? NULL : &conn_error);
    g_object_unref(sockaddr);
  }
  g_object_unref(enumerator);

  if (conn)
  {
    /**
    * We couldn't connect to the first address, but we succeeded
    * in connecting to a later address.
    */
    if (conn_error) {
      g_error_free (conn_error);
    }
    return true;
  }

  if(error) {
    std::cout  << "[WARN] Could not connect:" << std::endl << error->message << std::endl;
    g_error_free(error);
  }

  if (conn_error) {
    std::cout << "[WARN] Could not connect:" << std::endl << conn_error->message << std::endl;
    g_error_free(conn_error);
  }

  return false;
}//end connect

bool SimSparkController::init(const std::string& modelPath, const std::string& teamName, unsigned int teamNumber, unsigned int playerNumber, const std::string& server, unsigned int port, bool sync)
{
  Platform::getInstance().init(this);

  Configuration& config = Platform::getInstance().theConfiguration;

  // set the team number first
  if (teamNumber != 0) {
    theGameInfo.teamNumber = teamNumber;
  } else if (config.hasKey("player", "TeamNumber")) {
    theGameInfo.teamNumber = config.getInt("player", "TeamNumber");
  } else {
    theGameInfo.teamNumber = 4;
  }

  // use the player number if one is given, otherwise use the number from the simulation
  if(playerNumber != 0) {
      theGameInfo.playerNumber = playerNumber;
  } else if(config.hasKey("player", "PlayerNumber") && config.getInt("player", "PlayerNumber") != 0) {
      theGameInfo.playerNumber = config.getInt("player", "PlayerNumber");
  } else {
      theGameInfo.playerNumber = 0;
  }

  // use the team name if one is given, otherwise generate a name
  if (!teamName.empty()) {
      theGameInfo.teamName = teamName;
  } else if (config.hasKey("player", "TeamName")) {
      theGameInfo.teamName = config.getString("player", "TeamName");
  } else {
      theGameInfo.teamName = "NaoTH-" + std::to_string(theGameInfo.teamNumber);
  }

  theSync = sync?"(syn)":"";
  theSyncMode = sync;

  // connect to the simulator
  if(!connect(server, port)) {
    std::cerr << "SimSparkController could not connect" << std::endl;
    return false;
  }
  theSocket.init(socket);

  // send create command to simulator
  //"rsg/agent/naov4/nao.rsg";
  theSocket << "(scene " << modelPath << ")" << theSync << send;

  // wait the response
  getSensorData(theSensorData);
  updateSensors(theSensorData);

  // initialize the teamname and number
  theSocket << "(init (teamname " << theGameInfo.teamName << ")(unum " << theGameInfo.playerNumber << "))" << theSync << send;

  // this is to detect whether the game data has been updated
  theGameInfo.playerNumber = 0;
  // wait the response
  while (theGameInfo.playerNumber == 0)
  {
    getSensorData(theSensorData);
    updateSensors(theSensorData);
    theSocket << theSync << send;
  }
  // we should get the team index and player number now

  cout << "NaoTH Simpark initialization successful: " << theGameInfo.teamName << " " << theGameInfo.playerNumber << endl;

  //DEBUG_REQUEST_REGISTER("SimSparkController:beam", "beam to start pose", false);
  //REGISTER_DEBUG_COMMAND("beam", "beam to given pose", this);

  config.setInt("player", "TeamNumber", theGameInfo.teamNumber);
  config.setInt("player", "PlayerNumber", theGameInfo.playerNumber);
  config.setString("player", "TeamName", theGameInfo.teamName);

#ifdef DEBUG
  // calculate debug communicaiton port
  unsigned short debugPort = static_cast<short unsigned int> (5000 + (theGameInfo.teamNumber*100) + theGameInfo.playerNumber);
  theDebugServer.start(debugPort);
#endif

  // for the 'hear' percept
  if (config.hasKey("player", "ignoreOpponentMsg")) {
      ignoreOpponentMsg = config.getBool("player", "ignoreOpponentMsg");
  }

  theLastSenseTime = NaoTime::getNaoTimeInMilliSeconds();
  theLastActTime = theLastSenseTime;
  calculateNextActTime();

  return true;
}

void SimSparkController::main()
{
  if ( theSyncMode ) {
    singleThreadMain();
  } else {
    multiThreadsMain();
  }
}

void SimSparkController::singleThreadMain()
{
  cout << "SimSpark Controller runs in single thread" << endl;
  while ( getSensorData(theSensorData) )
  {
    updateSensors(theSensorData);
    if ( isNewImage || isNewVirtualVision )
    {
      callCognition();
    }
    PlatformInterface::runMotion();
    act();
  }//end while
}//end main

void SimSparkController::executeDebugCommand(const std::string &command, const std::map<std::string, std::string> &arguments, std::ostream &outstream)
{
  if(command == "beam")
  {
    Vector3d pose;
    if(arguments.find("x") != arguments.end() && arguments.find("y") != arguments.end())
    {
      pose.x = atof(arguments.find("x")->second.c_str());
      pose.y = atof(arguments.find("y")->second.c_str());
      if(arguments.find("r") != arguments.end())
      {
        pose.z = atof(arguments.find("r")->second.c_str());
      }
      else
      {
        pose.z = 0;
      }
      beam(pose);
      outstream << "beamed to (" << pose.x << "," << pose.y << "," << pose.z << ")" << std::endl;
    }
    else
    {
      outstream << "required parameter x or y missing (r is optional)" << std::endl;
    }

  }
}

void SimSparkController::motionLoop()
{
  while ( !exiting )
  {
    std::string data;
    {
      std::unique_lock<std::mutex> lock(theSensorDataMutex);
      theSensorDataCond.wait(lock);
      data = theSensorData;
      theSensorData = "";
    }
    updateSensors(data);
    PlatformInterface::runMotion();
  }
}//end motionLoop

void SimSparkController::cognitionLoop()
{
  while (!exiting)
  {
    callCognition();
  }
}//end cognitionLoop

void SimSparkController::callCognition()
{
  if(cognitionRegistered())
  {
    getCognitionInput();
    if ( !exiting )
    {
      PlatformInterface::callCognition();
      setCognitionOutput();
    }
  }
}//end callCognition

void SimSparkController::senseLoop()
{
  while( true )
  {
    string data;
    if ( !getSensorData(data) )
    {
      break;
    }

    {
      std::unique_lock<std::mutex> lock(theSensorDataMutex);
      if ( !theSensorData.empty() )
      {
        cerr<<"[Warning] the sensor data @ " << theLastSenseTime << " is dropped!"<<endl;
      }
      theSensorData = data;
    }
    theTimeCond.notify_one();
    theSensorDataCond.notify_one();
  }

  exiting = true;
  theSensorDataCond.notify_one(); // tell motion to exit
  theTimeCond.notify_one(); // tell act loop to exit
  theCognitionInputCond.notify_one(); // tell cognition to exit
}

void SimSparkController::actLoop()
{
  while( !exiting )
  {
    { // NOTE: lock object lives only in this code block
      std::unique_lock<std::mutex> lock(theTimeMutex);
      theTimeCond.wait(lock);
      calculateNextActTime();
    }

    unsigned int now = NaoTime::getNaoTimeInMilliSeconds();
    if ( theNextActTime > now )
    {
      ThreadUtil::sleep(theNextActTime - now);
    }

    act();
  }
}

void SimSparkController::act()
{
  // send command
  std::unique_lock<std::mutex> lock(theActDataMutex);
  try{
    theSocket << theActData.str() << theSync << send;
    theActData.str("");
  }
  catch(std::runtime_error& exp)
  {
    cerr<<"can not send data to server, because of "<<exp.what()<<endl;
  }

}

void SimSparkController::multiThreadsMain()
{
  cout << "SimSpark Controller runs in multi-threads" << endl;

  std::thread senseThread = std::thread(&SimSparkController::senseLoop, this);
  std::thread actThread = std::thread(&SimSparkController::actLoop, this);
  std::thread motionThread = std::thread(&SimSparkController::motionLoop, this);

  cognitionLoop();

  if(motionThread.joinable()) {
    motionThread.join();
  }
  if(actThread.joinable()) {
    actThread.join();
  }
  if(senseThread.joinable()) {
    senseThread.join();
  }
}//end multiThreadsMain

void SimSparkController::getMotionInput()
{
  PlatformInterface::getMotionInput();

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    theSensorJointData.stiffness[i] = theLastSensorJointData.stiffness[i];
  }
  theLastSensorJointData = theSensorJointData;
}

void SimSparkController::setMotionOutput()
{
  PlatformInterface::setMotionOutput();

  {
    std::unique_lock<std::mutex> lock(theActDataMutex);
    say();
    autoBeam();
    jointControl();
    theActData << theSync;
  }
}

void SimSparkController::getCognitionInput()
{
  std::unique_lock<std::mutex> lock(theCognitionInputMutex);
  while (!isNewImage && !isNewVirtualVision && !exiting )
  {
    theCognitionInputCond.wait(lock);
  }

  PlatformInterface::getCognitionInput();
  isNewVirtualVision = false;
  isNewImage = false;
}

void SimSparkController::setCognitionOutput()
{
  std::unique_lock<std::mutex> lock(theCognitionOutputMutex);
  PlatformInterface::setCognitionOutput();
}

bool SimSparkController::getSensorData(std::string& data)
{
  try {
    theSocket >> data;
    theLastSenseTime = NaoTime::getNaoTimeInMilliSeconds();
  }
  catch (std::runtime_error& exp)
  {
    cerr<<"can not get data from server, because of "<<exp.what()<<endl;
    return false;
  }
  return true;
}

bool SimSparkController::updateSensors(std::string& msg)
{
  double lastSenseTime = theSenseTime;

  pcont_t* pcont;
  sexp_t* sexp = NULL;

  char* c = const_cast<char*> (msg.c_str());
  pcont = init_continuation(c);
  sexp = iparse_sexp(c, msg.size(), pcont);

  std::unique_lock<std::mutex> lock(theCognitionInputMutex);

  // clear FSR data, since if there is no FSR data, it means no touch
  for (int i = 0; i < FSRData::numOfFSR; i++) {
    theFSRData.dataLeft[i] = 0;
    theFSRData.dataRight[i] = 0;
  }

    while(sexp)
    {
      const sexp_t* t = sexp->list;
      if (SexpParser::isVal(t))
      {
        bool ok = true;
        string name(t->val);
        if ("HJ" == name) { ok = updateHingeJoint(t->next); }
        else if ("FRP" == name) { ok = updateFSR(t->next); }
        else if ("BottomCamera" == name || "See" == name)
        {
          theVirtualVision.clear();
          ok = updateSee(theVirtualVision, t->next);
          if ( ok ) {
            isNewVirtualVision = true;
          }

          //HACK: assume the image is behind of "See"
          int offset = parseImage(pcont->lastPos);
          pcont->lastPos = &(pcont->lastPos[offset]);
          isNewImage = offset > 0;
        }
        else if ("TopCamera" == name)
        {
          theVirtualVisionTop.clear();
          ok = updateSee(theVirtualVisionTop, t->next);
          if ( ok ) {
            isNewVirtualVision = true;
          }
        }
        else if ("time" == name)
        {
          ok = SexpParser::parseGivenValue(t->next, "now", theSenseTime); // time
          theStepTime = theSenseTime - lastSenseTime;
          theFrameInfo.setTime( static_cast<unsigned int>(theSenseTime * 1000.0) );
          if ( static_cast<unsigned int>(theStepTime*100)*10 > getBasicTimeStep() ) {
            cerr<<"warning: the step is "<<theStepTime<<" s"<<endl;
          }
        }
        else if ("GYR" == name) { ok = updateGyro(t->next); } // gyro rate
        else if ("ACC" == name) { ok = updateAccelerometer(t->next); }
        else if ("GS" == name) { ok = updateGameInfo(t->next); } // game state
        else if ("hear" == name) { ok = hear(t->next); } // hear
        else if ("IMU" == name) { ok = updateIMU(t->next); } // interial sensor data
        else if ("IMG" == name)
        {
          // HACK: image parsing is very slow in Windows
          // thus we parse the image separately (cf. the case "See")
          //ok = updateImage(t->next); // image from camera
          //if (ok) isNewImage = true;
        }
        else if ("GPS" == name) { ok = updateGPS(t->next); }
        else if ("BAT" == name) { ok = updateBattery(t->next); }
        else if ("US" == name) { ok = updateSonar(t->next); }
        else
        {
          if( ignore.find(name) == ignore.end() ) // new unknown message
          {
            cerr << "[SimSparkController] Perception unknow name: " << name << endl;
            ignore.insert(name);
          }
        }

        if (!ok) {
          cerr << "[SimSparkController] Perception update failed: " << name << endl;
          return false;
        }
      }
      destroy_sexp(sexp);
      sexp = iparse_sexp(c, msg.size(), pcont);
    }//end while

    updateInertialSensor();

    if ( isNewImage || isNewVirtualVision ){
      theCognitionInputCond.notify_one();
    }

  destroy_sexp(sexp);
  destroy_continuation(pcont);

  return true;
}//end updateSensors


int SimSparkController::parseString(char* data, std::string& value)
{
  int c = 0;
  std::stringstream ss;
  while(data[c] != ' ' && data[c] != ')')
  {
    ss << data[c++];
  }

  value = ss.str();

  // eat ' '
  if(data[c] == ' ') c++;

  return c;
}//end parseString


int SimSparkController::parseInt(char* data, int& value)
{
  std::string tmp;
  int c = parseString(data, tmp);
  StringTools::strTo(tmp, value);
  return c;
}


int SimSparkController::parseImage(char* data)
{
  //(IMG (s 320 240) (

  int c = 0;

  // check and eat (
  if(data[c++] != '(') return 0;

  // get the name: IMG
  std::string name;
  c += parseString(&data[c], name);

  if(name != "IMG") return 0;

  // s
  if(data[c++] != '(') return 0;
  std::string sizeName;
  c += parseString(&data[c], sizeName);
  if(sizeName != "s") return 0;


  // size x
  int x = 0;
  c += parseInt(&data[c], x);

  // size y
  int y = 0;
  c += parseInt(&data[c], y);

  if(data[c++] != ')') return 0;
  if(data[c++] != ' ') return 0;
  if(data[c++] != '(') return 0;

  // d
  std::string dataName;
  c += parseString(&data[c], dataName);
  if(dataName != "d") return 0;

  // parse the image data
  int image_start = c;

  // image length
  unsigned int len = x*y*3;

  // read the image data until the next ')'
  //while(data[c++] != ')');

  // HACK: base64 encoded data is has 1/3 more chars
  c += x*y*4;
  if(data[c++] != ')') return 0; // check integrity

  int image_length = c - image_start - 1;

  // check the buffer size
  if (len != theImageSize || theImageData == NULL)
  {
    cout << "recreate image" << endl;
    if (theImageData != NULL)
      delete [] theImageData;

    theImageSize = len;
    theImageData = new char[theImageSize];
  }//end if

  int dl = theBase64Decoder.decode(&data[image_start], image_length, theImageData);
  ASSERT(dl == (int) theImageSize);

  // eat the last )
  c++;

  return c;
}//end paseImage


bool SimSparkController::updateImage(const sexp_t* sexp)
{
  // size of image
  int size[2];
  if (!SexpParser::parseGivenArrayValue(sexp, "s", 2, size))
  {
    cerr << "can not get the image size!\n";
    return false;
  }

  unsigned int len = size[0] * size[1]*3;

  // check the buffer size
  if (len != theImageSize || theImageData == NULL)
  {
    cout << "recreate image" << endl;
    if (theImageData != NULL)
      delete [] theImageData;

    theImageSize = len;
    theImageData = new char[theImageSize];
  }//end if

  // decode the image
  sexp = sexp->next;

  if (SexpParser::isList(sexp))
  {
    const sexp_t* dsexp = sexp->list;
    if (SexpParser::isVal(dsexp) && string(dsexp->val) == "d")
    {
      dsexp = dsexp->next;
      int dl = theBase64Decoder.decode(dsexp->val,static_cast<int> (dsexp->val_used), theImageData);
      ASSERT(dl == (int) theImageSize);
      return true;
    }
  }//end if

  return false;
}//end updateImage

bool SimSparkController::updateHingeJoint(const sexp_t* sexp)
{
  // get the name
  std::string name;
  if (!SexpParser::parseGivenValue(sexp, "n", name))
  {
    cerr << "can not get the HJ name!\n";
    return false;
  }

  // get the id
  JointData::JointID jid = theJointSensorNameMap[name];
  if (JointData::numOfJoint <= jid)
  {
    cerr << "unknown the HJ name!" << name << '\n';
    return false;
  }

  // set the joint
  sexp = sexp->next;
  double ax;
  if (!SexpParser::parseGivenValue(sexp, "ax", ax))
  {
    cerr << "can not get the data of joint " << name << '\n';
    return false;
  }

  ax *= (Math::pi / 180);
  // due to the different coordination
  if (JointData::HeadPitch == jid
    || JointData::LShoulderPitch == jid
    || JointData::RShoulderPitch == jid
    || JointData::LHipPitch == jid
    || JointData::RHipPitch == jid
    || JointData::LKneePitch == jid
    || JointData::RKneePitch == jid
    || JointData::LAnklePitch == jid
    || JointData::RAnklePitch == jid)
  {
    ax *= -1;
  }

  theSensorJointData.dp[jid] = Math::clamp(Math::normalize(ax - theSensorJointData.position[jid]) / theStepTime,
    -maxJointAbsSpeed, maxJointAbsSpeed);
  theSensorJointData.position[jid] = ax;

  // set the joint temperature
  sexp = sexp->next;
  double temp;
  if (SexpParser::parseGivenValue(sexp, "tp", temp))
  {
    theSensorJointData.temperature[jid] = temp;
  }

  return true;
}

bool SimSparkController::updateGyro(const sexp_t* sexp)
{
  // get the name
  std::string name;
  if (!SexpParser::parseGivenValue(sexp, "n", name))
  {
    cerr << "can not get the Gyro name!\n";
    return false;
  }

  if ("torso" != name)
  {
    cerr << "can not handle gyro : " << name << endl;
    return false;
  }

  double data[3];
  if (!SexpParser::parseGivenArrayValue(sexp->next, "rt", 3, data))
  {
    cerr << "can not get the Gyro data!\n";
    return false;
  }

  theGyroData.data[0] = Math::fromDegrees(data[1]);
  theGyroData.data[1] = -Math::fromDegrees(data[0]);
  theGyroData.data[2] = Math::fromDegrees(data[2]);

  return true;
}

// message format: (ACC (n <name>) (a <x> <y> <z>))
bool SimSparkController::updateAccelerometer(const sexp_t* sexp)
{
  // get the name
  std::string name;
  if (!SexpParser::parseGivenValue(sexp, "n", name))
  {
    cerr << "can not get the Accelerometer name!\n";
    return false;
  }

  if ("torso" != name)
  {
    cerr << "can not handle Accelerometer : " << name << endl;
    return false;
  }

  Vector3d acc;
  if (!SexpParser::parseGivenArrayValue(sexp->next, "a", 3, &(acc.x)))
  {
    cerr << "can not get the Accelerometer data!\n";
    return false;
  }

  swap(acc[0], acc[1]);
  acc[1] = -acc[1];
  double k = 0.1;
  theAccelerometerData.data = theAccelerometerData.data*(1-k) + acc*k;

  return true;
}//end updateAccelerometer

// Example message: (BAT 1.00)
bool SimSparkController::updateBattery(const sexp_t* sexp)
{
  if (!SexpParser::parseValue(sexp, theBatteryData.charge))
  {
    cerr << "SimSparkGameInfo::update failed battery value\n";
  return false;
  }

  return true;
}//end updateBattery

// Example message: (US Left (0.52, 5.00, 5.00, 5.00, 5.00, 5.00, 5.00, 5.00, 5.00, 5.00))
bool SimSparkController::updateSonar(const sexp_t* sexp)
{
    string name;
    SexpParser::parseValue(sexp, name);

    // decide which 'side' should be updated
    double *data = nullptr;
    if (name == "Left") {
        data = theUSData.dataLeft;
    } else if (name == "Right") {
        data = theUSData.dataRight;
    } else {
        cerr << "Unknown sonar sensor!\n";
        return false;
    }

    // read sonar echoes
    sexp = sexp->next;
    if (SexpParser::isList(sexp))
    {
        unsigned int i = 0;
        double echo = 0;
        const sexp_t* echoes = sexp->list;
        while (echoes && i < UltraSoundReceiveData::numOfUSEcho) {
            if (SexpParser::parseValue(echoes, echo)) {
                data[i++] = echo;
            } else {
                cerr << "Unable to parse sonar echo!\n";
            }
            echoes = echoes->next;
        }

        // fill remaining echoes
        for (; i < UltraSoundReceiveData::numOfUSEcho; ++i) {
            data[i] = UltraSoundReceiveData::INVALID;
        }

        // HACK: introduce some "noise" on the last echo, so that the
        //       UltraSoundObstacleLocator recognizes the new data
        data[UltraSoundReceiveData::numOfUSEcho - 1] +=
                NaoTime::getNaoTimeInMilliSeconds() % 2 == 0 ? 0.01 : -0.01;

        // take the smallest value; the first echoes are the smallest
        theUSData.rawdata = std::min(theUSData.dataLeft[0], theUSData.dataRight[0]);
    } else {
        cerr << "Missing sonar echoes!\n";
        return false;
    }

    return true;
}//end updateSonar

// Example message:
// (GPS (n torso) (tf 0.00 1.00 0.00 -2.00 -1.00 0.00 0.00 2.10 0.00 -0.00 1.00 0.40 0.00 -0.00 0.00 1.00))
bool SimSparkController::updateGPS(const sexp_t* sexp)
{
  std::string name;
  if (!SexpParser::parseGivenValue(sexp, "n", name))
  {
    cerr << "can not get the GPS name!\n";
    return false;
  }

  // treat the position of the torso
  if ("torso" == name)
  {
    double tf[16];
      if (!SexpParser::parseGivenArrayValue(sexp->next, "tf", 16, &(tf[0])))
      {
        cerr << "can not get the GPS data!\n";
        return false;
      }
    int i = 0;
    Pose3D& p(theGPSData.data);

    p.rotation.c[0][0] = tf[i++]; p.rotation.c[1][0] = tf[i++]; p.rotation.c[2][0] = tf[i++]; p.translation[0] = tf[i++];
    p.rotation.c[0][1] = tf[i++]; p.rotation.c[1][1] = tf[i++]; p.rotation.c[2][1] = tf[i++]; p.translation[1] = tf[i++];
    p.rotation.c[0][2] = tf[i++]; p.rotation.c[1][2] = tf[i++]; p.rotation.c[2][2] = tf[i++]; p.translation[2] = tf[i++];
    assert(tf[i++] == 0.0);       assert(tf[i++] == 0.0);       assert(tf[i++] == 0.0);       assert(tf[i++] == 1.0);

    p.translation *= 1000.0; // convert from m to mm

    // rotate the coordinate system if the own goal is right
    if(!theGameInfo.playLeftSide) {
      p.rotation.rotateZ(Math::pi);
      p.translation.x *= -1;
      p.translation.y *= -1;
    }
  }

  return true;
}//end updateGPS

// Example message: "(GS (t 0.00) (pm BeforeKickOff))"
bool SimSparkController::updateGameInfo(const sexp_t* sexp)
{
  bool ok = true;
  string name;
  while (sexp)
  {
    const sexp_t* t = sexp->list;
    if (SexpParser::parseValue(t, name))
    {
      if ("t" == name) // time
      {
        double gameTime = 0;
        if (!SexpParser::parseValue(t->next, gameTime))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get time value\n";
        }
        else
        {
          ASSERT(gameTime >= 0);
          theGameInfo.gameTime = static_cast<unsigned int>(gameTime);
        }
      } else if ("pm" == name) // play mode
      {
        std::string pm;
        if (!SexpParser::parseValue(t->next, pm))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get play mode value\n";
        }

        // try SPL state first
        GameData::GameState state = GameData::gameStateFromString(pm);
        if (state == GameData::unknown_game_state)
        {
          // try SimSpark play mode
          SimSparkGameInfo::PlayMode playMode = SimSparkGameInfo::getPlayModeByName(pm);
          if(playMode != SimSparkGameInfo::numOfPlayMode) {
            state = SimSparkGameInfo::covertToGameState(playMode);
          }
        }
        else
        {
          theGameInfo.gameState = state;
        }
      } else if ("ti" == name) // team info
      {
          // this should be a list of infos of the left team
          t = t->next;
          if(!SexpParser::isList(t) || !parseTeamInfo(t->list, theGameInfo.playLeftSide?theGameInfo.ownPlayers:theGameInfo.oppPlayers)) {
              ok = false;
              cerr << "SimSparkGameInfo::update failed get left team info\n";
          }
          // this should be a list of infos of the right team
          t = t->next;
          if(!SexpParser::isList(t) || !parseTeamInfo(t->list, theGameInfo.playLeftSide?theGameInfo.oppPlayers:theGameInfo.ownPlayers)) {
              ok = false;
              cerr << "SimSparkGameInfo::update failed get right team info\n";
          }
      } else if ("unum" == name) // unum
      {
        if (!SexpParser::parseValue(t->next, theGameInfo.playerNumber))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get unum value\n";
        }
      } else if ("team" == name) // side
      {
        string side;
        if (!SexpParser::parseValue(t->next, side))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get team index value\n";
        }
        theGameInfo.updateBySideName(side);
      }
      else if ("sl" == name)
      {
        if (!SexpParser::parseValue(t->next, theGameInfo.score.first))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed score left\n";
        }
      }
      else if ("sr" == name)
      {
        if (!SexpParser::parseValue(t->next, theGameInfo.score.second))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed score right\n";
        }
      }
      else if ("k" == name)
      {
        string kickoffSide;
        if (!SexpParser::parseValue(t->next, kickoffSide))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed kickoffSide\n";
        }
        theGameInfo.kickoff = (kickoffSide == STR_TI_LEFT && theGameInfo.playLeftSide)
                           || (kickoffSide == STR_TI_RIGHT && !theGameInfo.playLeftSide);
      }
      else
      {
        ok = false;
        cerr << "SimSparkGameInfo::update unknown name: " << name << '\n';
      }
    } else
    {
      ok = false;
      cerr << "SimSparkGameInfo::update can not get the name!\n";
    }
    sexp = sexp->next;
  }

  if ( ok ) {
    theGameInfo.valid = true;
  }
  return ok;
}//end updateGameInfo

bool SimSparkController::parseTeamInfo(const sexp_t* team, std::vector<naoth::GameData::RobotInfo> &players)
{
    // iterate through player info of the team
    while(team) {
        // the player info is a list!
        if(SexpParser::isList(team)) {
            const sexp_t* player = team->list;
            int playerNumber;
            if(!SexpParser::parseValue(player, playerNumber)) {
                cerr << "SimSparkGameInfo::could not retrieve player number!\n";
                return false;
            }
            string penalty;
            if(!SexpParser::parseValue(player->next, penalty)) {
                cerr << "SimSparkGameInfo::could not retrieve player penalty!\n";
                return false;
            }
            double penaltyTime;
            if(!SexpParser::parseValue(player->next->next, penaltyTime)) {
                cerr << "SimSparkGameInfo::could not retrieve penaltyTime!!\n";
                return false;
            }
            // make sure, we got a valid player number
            if(playerNumber > 0 && static_cast<size_t>(playerNumber) <= players.size()) {
                players.at(static_cast<size_t>(playerNumber-1)).penalty = GameData::penaltyFromString(penalty);
                players.at(static_cast<size_t>(playerNumber-1)).secsTillUnpenalised = static_cast<int>(ceil(penaltyTime));
            } else {
                cerr << "SimSparkGameInfo::got an invalid player number: "<< playerNumber <<"\n";
                return false;
            }
        }
        team = team->next;
    }
    return true;
}

bool SimSparkController::updateFSR(const sexp_t* sexp)
{
  // get the name
  std::string name;
  if (!SexpParser::parseGivenValue(sexp, "n", name))
  {
    cerr << "can not get the ForceResistancePerceptor name!\n";
    return false;
  }

  double C[3], F[3];
  sexp = sexp->next;
  if (!SexpParser::parseGivenArrayValue(sexp, "c", 3, C))
  {
    cerr << "can not get the ForceResistancePerceptor c!\n";
    return false;
  }

  sexp = sexp->next;
  if (!SexpParser::parseGivenArrayValue(sexp, "f", 3, F))
  {
    cerr << "can not get the ForceResistancePerceptor f!\n";
    return false;
  }

  const FSRData::SensorID id0 = FSRData::RearLeft;
  const FSRData::SensorID id1 = FSRData::RearRight;
  const FSRData::SensorID id2 = FSRData::FrontLeft;
  const FSRData::SensorID id3 = FSRData::FrontRight;

  double f = F[2] / 4;
  double fx = f * ( C[1]*1000 + 30);
  double fy = f * (-C[0]*1000);

  if ("lf" == name)
  {
    const Vector3d* positions = NaoInfo::FSRPositionsLeft;
    std::vector<double>& values = theFSRData.dataLeft;

    calFSRForce(f, fx, fy, positions, values, id0, id1, id2);
    calFSRForce(f, fx, fy, positions, values, id1, id2, id3);
    calFSRForce(f, fx, fy, positions, values, id2, id3, id0);
    calFSRForce(f, fx, fy, positions, values, id3, id0, id1);
  }
  else if ("rf" == name)
  {
    const Vector3d* positions = NaoInfo::FSRPositionsRight;
    std::vector<double>& values = theFSRData.dataRight;

    calFSRForce(f, fx, fy, positions, values, id0, id1, id2);
    calFSRForce(f, fx, fy, positions, values, id1, id2, id3);
    calFSRForce(f, fx, fy, positions, values, id2, id3, id0);
    calFSRForce(f, fx, fy, positions, values, id3, id0, id1);
  }
  else
  {
    cerr << "unknow ForceResistancePerceptor name: " << name << endl;
    return false;
  }

  return true;
}

Vector3d SimSparkController::decomposeForce(double f, double fx, double fy, const Vector3d& c0, const Vector3d& c1, const Vector3d& c2)
{
  Matrix3x3<double> A(Vector3d(1, c0.x, c0.y), Vector3d(1, c1.x, c1.y), Vector3d(1, c2.x, c2.y));
  return A.invert() * Vector3d(f, fx, fy);
}

void SimSparkController::calFSRForce(
  double f, double x, double y,
  const Vector3d* positions,
  std::vector<double>& values,
  FSRData::SensorID id0, FSRData::SensorID id1, FSRData::SensorID id2)
{
  Vector3d F = decomposeForce(f, x, y, positions[id0], positions[id1], positions[id2]);
  values[id0] += F.x;
  values[id1] += F.y;
  values[id2] += F.z;
}

bool SimSparkController::updateSee(VirtualVision& virtualVision, const sexp_t* sexp)
{
  bool ok = true;

  std::string name;
  while (sexp)
  {
    name = "";
    const sexp_t* t = sexp->list;
    if (SEXP_VALUE == t->ty)
    {
      SexpParser::parseValue(t, name);
      if ("P" == name) // parse a player
      {
        string teamName;
        t = t->next;
        if (!SexpParser::parseGivenValue(t, "team", teamName)) {
          cerr << "[SimSparkController] Vision can not get the Player's team" << endl;
        }

        string id;
        t = t->next;
        if (!SexpParser::parseGivenValue(t, "id", id)) {
          cerr << "[SimSparkController] Vision can not get Player's id" << endl;
        }

        // parse the players points
        t = t->next;
        while(t) {
          SexpParser::parseValue(t->list, name);
          // NOTE: if the parsePoint3D the map virtualVision contains an zero vector entry with the key name
          if(!parsePoint3D(t->list->next, virtualVision.data["P "+teamName+" "+id+" "+name])) {
            cerr << "[SimSparkController] Vision can not parse the point " << name << " of the player " << teamName << ":"<< id << endl;
          }
          t = t->next;
        }
      }
      else if ("L" == name) // parse a line
      {
        double p0[3], p1[3];
        if (  SexpParser::parseGivenArrayValue(t->next, "pol", 3, p0) &&
              SexpParser::parseGivenArrayValue(t->next->next, "pol", 3, p1))
        {
          VirtualVision::Line l;
          l.p0 = Vector3d(p0[0]*1000, Math::fromDegrees(p0[1]), Math::fromDegrees(p0[2]));
          l.p1 = Vector3d(p1[0]*1000, Math::fromDegrees(p1[1]), Math::fromDegrees(p1[2]));
          virtualVision.lines.push_back(l);
        } else {
          cerr << "[SimSparkController] Vision can not process line! " << endl;
        }
      }
      else if("G1L" == name || "G2L" == name || "G1R" == name || "G2R" == name) // goal points
      {
        ok = parsePoint3D(t->next, virtualVision.data[name]);
      }
      else if("F1L" == name || "F2L" == name || "F1R" == name || "F2R" == name) // flags
      {
        ok = parsePoint3D(t->next, virtualVision.data[name]);
      }
      else if("B" == name) // ball
      {
        ok = parsePoint3D(t->next, virtualVision.data[name]);
      }
      else // parse other points
      {
        if( ignore.find(name) == ignore.end() ) // new unknown message
        {
          cerr << "[SimSparkController] see: unknown object " << name << endl;
          ignore.insert(name);
        }
      }
    }
    sexp = sexp->next;
  }

  return ok;
}

bool SimSparkController::parsePoint3D(const sexp_t* sexp, Vector3d& result) const
{
  static double buffer[3];
  if (SexpParser::parseGivenArrayValue(sexp, "pol", 3, buffer))
  {
    result = Vector3d(buffer[0]*1000, Math::fromDegrees(buffer[1]), Math::fromDegrees(buffer[2]));
    return true;
  }
  else if (SexpParser::parseArrayValue(sexp, 3, buffer))
  {
    result = Vector3d(buffer[0], buffer[1], buffer[2])*1000;
    return true;
  }

  return false;
}

void SimSparkController::get(FrameInfo& data)
{
  data.setTime(theFrameInfo.getTime());
  data.setFrameNumber(data.getFrameNumber() + 1);
}

void SimSparkController::get(SensorJointData& data)
{
  data = theSensorJointData;
  // hip joints should be the same!
  double hipYawPitch = (data.position[JointData::LHipYawPitch] + data.position[JointData::RHipYawPitch] ) * 0.5;
  data.position[JointData::LHipYawPitch] = hipYawPitch;
  data.position[JointData::RHipYawPitch] = hipYawPitch;
  hipYawPitch = (data.dp[JointData::LHipYawPitch] + data.dp[JointData::RHipYawPitch] ) * 0.5;
  data.dp[JointData::LHipYawPitch] = hipYawPitch;
  data.dp[JointData::RHipYawPitch] = hipYawPitch;
  hipYawPitch = (data.ddp[JointData::LHipYawPitch] + data.ddp[JointData::RHipYawPitch] ) * 0.5;
  data.ddp[JointData::LHipYawPitch] = hipYawPitch;
  data.ddp[JointData::RHipYawPitch] = hipYawPitch;
}

void SimSparkController::get(AccelerometerData& data)
{
  data = theAccelerometerData;
}

void SimSparkController::get(BatteryData& data)
{
  data = theBatteryData;
}

void SimSparkController::get(GPSData& data)
{
  data = theGPSData;
}

void SimSparkController::get(UltraSoundReceiveData& data)
{
  data = theUSData;
}

void SimSparkController::get(Image& data)
{
  if ( isNewImage )
  {
    //ACHTUNG: this is set by the module CameraInfoSetter
    //data.setCameraInfo(Platform::getInstance().theCameraInfo);

    unsigned int width = data.width();
    unsigned int height = data.height();
    unsigned int resolution = width*height;

    ASSERT(resolution * 3 == theImageSize);

    for (unsigned int x = 0; x < width; x++)
    {
      for (unsigned int y = 0; y < height; y++)
      {
        int idx = (resolution - width * (y + 1) + x)*3;
        Pixel p;
        p.y = theImageData[idx];
        p.u = theImageData[idx + 1];
        p.v = theImageData[idx + 2];

        naoth::ColorModelConversions::fromRGBToYCbCr(
          p.y, p.u, p.v,
          p.y, p.u, p.v);

        data.set(x, y, p);
      }
    }
  }
}//end get

void SimSparkController::get(GyrometerData& data)
{
  data = theGyroData;
}

void SimSparkController::get(FSRData& data)
{
  data = theFSRData;
}

void SimSparkController::get(InertialSensorData& data)
{
  data = theInertialSensorData;
}

void SimSparkController::get(VirtualVision& data)
{
  data = theVirtualVision;
}

void SimSparkController::get(VirtualVisionTop& data)
{
  data = theVirtualVisionTop;
}

void SimSparkController::get(GameData& data)
{
  data.valid = theGameInfo.valid;

  if ( theGameInfo.valid )
  {
    data.playersPerTeam = theGameInfo.playersPerTeam;
    data.gameState = theGameInfo.gameState;
    data.secsRemaining = theGameInfo.getRemainingTimeInHalf();

    data.kickingTeam = theGameInfo.kickoff ? theGameInfo.teamNumber : 0;
    data.firstHalf = theGameInfo.firstHalf();

    data.newPlayerNumber = theGameInfo.playerNumber;

    data.ownTeam.teamNumber = theGameInfo.teamNumber;
    data.ownTeam.teamColor = theGameInfo.getOwnTeamColor();
    data.ownTeam.players.resize(theGameInfo.playersPerTeam);
    data.ownTeam.score = theGameInfo.getOwnScore();

    for (size_t i=0; i<data.ownTeam.players.size(); ++i) {
        if(theGameInfo.ownPlayers.size() > i) {
            data.ownTeam.players[i].penalty = theGameInfo.ownPlayers[i].penalty;
            data.ownTeam.players[i].secsTillUnpenalised = theGameInfo.ownPlayers[i].secsTillUnpenalised;
        }
    }

    data.oppTeam.teamColor = theGameInfo.getOppTeamColor();
    data.oppTeam.players.resize(theGameInfo.playersPerTeam);
    data.oppTeam.score = theGameInfo.getOppScore();
    for (size_t i=0; i<data.oppTeam.players.size(); ++i) {
        if(theGameInfo.oppPlayers.size() > i) {
            data.oppTeam.players[i].penalty = theGameInfo.oppPlayers[i].penalty;
            data.oppTeam.players[i].secsTillUnpenalised = theGameInfo.oppPlayers[i].secsTillUnpenalised;
        }
    }

    theGameInfo.valid = false;
  }
}

void SimSparkController::updateInertialSensor()
{
  // calculate inertial sensor data by gyrometer
  const Vector3d& gyrometer = theGyroData.data;
  static double oldGyroX = gyrometer.x;
  static double oldGyroY = gyrometer.y;
  theInertialSensorData.data.x += ((gyrometer.x+oldGyroX) * 0.5 * theStepTime);
  theInertialSensorData.data.y += ((gyrometer.y+oldGyroY) * 0.5 * theStepTime);
  oldGyroX = gyrometer.x;
  oldGyroY = gyrometer.y;

  // calculate intertial sensor data by accelerometer
  const Vector3d& acc = theAccelerometerData.data;
  double len = acc.abs();

  if (len > 2 && len < 30)
  {
    double x = asin(acc.y / len);
    double cx = cos(x);
    double y = -atan2(acc.x / Math::sgn(cx), acc.z / Math::sgn(cx));

    double k = 0.04;
//    MODIFY("updateInertialSensor.k", k);
    theInertialSensorData.data.x = (1 - k) * theInertialSensorData.data.x + k * x;
    theInertialSensorData.data.y = (1 - k) * theInertialSensorData.data.y + k * y;
  }
}

void SimSparkController::set(const MotorJointData& data)
{
  theMotorJointData.push_back(data);
}

void SimSparkController::jointControl()
{
  if ( theMotorJointData.size() < 2 ) return;

  MotorJointData data = theMotorJointData.front();
  theMotorJointData.pop_front();
  const MotorJointData& data2 = theMotorJointData.front();

  double d = 1.0 / theStepTime * 0.9;
  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    // normalize the joint angle
    double target = data.position[i];
    double ang = theLastSensorJointData.position[i];// + theLastSensorJointData.dp[i] * theStepTime;
    double v = (target - ang) * d * max(0.0, data.stiffness[i]);
    v = Math::clamp(v, -maxJointAbsSpeed, maxJointAbsSpeed);
    ang += (v * theStepTime);
    target = data2.position[i];
    double v2 = (target - ang) * d * max(0.0, data2.stiffness[i]);
    theLastSensorJointData.stiffness[i] = data2.stiffness[i];
    v2 = Math::clamp(v2, -maxJointAbsSpeed, maxJointAbsSpeed);

    // due to the different coordination
    if (JointData::HeadPitch == i
      || JointData::LShoulderPitch == i
      || JointData::RShoulderPitch == i
      || JointData::LHipPitch == i
      || JointData::RHipPitch == i
      || JointData::LKneePitch == i
      || JointData::RKneePitch == i
      || JointData::LAnklePitch == i
      || JointData::RAnklePitch == i)
    {
      v2 *= -1;
    }

    std::map<JointData::JointID, std::string>::const_iterator iter = theJointMotorNameMap.find((JointData::JointID)i);
    if(iter != theJointMotorNameMap.end()) {
      theActData << '(' << (iter->second) << ' ' << v2 << ')';
    }
  }
}


void SimSparkController::say()
{
  if ( theGameInfo.playersPerTeam == 0 ) {
    return;
  }

  // make sure all robot have chance to say something
  if ( ( static_cast<int>(floor(theSenseTime*1000/getBasicTimeStep()/2)) % theGameInfo.playersPerTeam) +1 != theGameInfo.playerNumber ) {
    return;
  }

  {
    std::unique_lock<std::mutex> lock(theCognitionOutputMutex, std::try_to_lock);
    if (lock.owns_lock())
    {
      if (!theTeamMessageDataOut.data.empty()) {
        theActData << "(say " << theTeamMessageDataOut.data << ")";
      }
    }
  }
}

bool SimSparkController::hear(const sexp_t* sexp)
{
    size_t minMsgSize = (sizeof(SPLStandardMessage) - SPL_STANDARD_MESSAGE_DATA_SIZE);
    std::vector<const sexp_t*> data;

    // the actual message is last, collect all preceding values
    while(sexp->val_used < minMsgSize) {
        data.push_back(sexp);
        sexp = sexp->next;
    }

    size_t idx = 0;
    // new simspark version (0.6.8+) has an additional "team" field!
    string team = theGameInfo.teamName;
    if(data.size() >= 3) {
        SexpParser::parseValue(data[idx++], team);
    }

    // ignore opponent messages
    if(ignoreOpponentMsg && team.compare(theGameInfo.teamName) != 0) { return true; }

    double direction;
    if(!SexpParser::parseValue(data[idx++], direction)) {
      std::cerr << "[SimSparkController Hear] can not get direction" << std::endl;
      return false;
    }

    /*
    // NOTE: should be handled by the TeamCommReceiver!
    if ("self" == direction) {
        // this message come from myself, just omit it
        //return true;
    }
    */

    double time;
    if(!SexpParser::parseValue(data[idx++], time)) {
        std::cerr << "[SimSparkController Hear] can not get time" << std::endl;
        return false;
    }

    string msg;
    if (SexpParser::parseValue(sexp, msg) && !msg.empty() && msg != ""){
        theTeamMessageDataIn.data.push_back(msg);
    } else {
        std::cerr << "[SimSparkController Hear] can not parse message" << std::endl;
        return false;
    }

    //std::cout << "hear message : " << team << "/" << theGameInfo.teamName << " " << time << ' ' << ' ' << direction << ' ' << msg << std::endl;

    return true;
}

void SimSparkController::beam(const Vector3<double>& p)
{
    theActData << "(beam "<<p.x<<" "<<p.y<<" "<<p.z<<")";
}

void SimSparkController::autoBeam()
{

  bool beamRequest = false;
  //DEBUG_REQUEST("SimSparkController:beam", beamRequest = true;);

  if (beamRequest
    || theGameInfo.gameState == GameData::initial
  //  || theGameData.playMode == GameData::goal_own
  //  || theGameData.playMode == GameData::goal_opp
  //  || theGameData.playMode == GameData::before_kick_off
  )
  {
    if ( beamRequest ) // || theFrameInfo.getTime() - theGameInfo.timeWhenPlayModeChanged < 1000 )
    {
      const Configuration& cfg = Platform::getInstance().theConfiguration;
      string group = "PoseBeforeKickOff";
      if ( !cfg.hasGroup(group) )
      {
        cerr<<"SimSparkController: can not beam, because there is no configuration"<<endl;
        return;
      }

      stringstream key;
      key << "Player" << theGameInfo.playerNumber << ".Pose.";
      string keyx = key.str()+"x";
      string keyy = key.str()+"y";
      string keyr = key.str()+"rot";
      if ( ! (cfg.hasKey(group, keyx) && cfg.hasKey(group, keyy) && cfg.hasKey(group, keyr)) )
      {
        cerr<<"SimSparkController: can not beam, because configuration for Player "<<theGameInfo.playerNumber
            <<" is misssing"<<endl;
        return;
      }
      Vector3d pose;
      pose.x = cfg.getDouble(group, keyx);
      pose.y = cfg.getDouble(group, keyy);
      pose.z = cfg.getDouble(group, keyr);

      beam(pose); // execute 1 second
    }
  }
}

bool SimSparkController::updateIMU(const sexp_t* sexp)
{
  // get the name
  std::string name;
  if (!SexpParser::parseGivenValue(sexp, "n", name))
  {
    cerr << "can not get the IMU name!\n";
    return false;
  }

  if ("torso" != name)
  {
    cerr << "can not handle IMU : " << name << endl;
    return false;
  }

  double imu[9];
  if (!SexpParser::parseGivenArrayValue(sexp->next, "m", 9, imu))
  {
    cerr << "can not get the IMU data!\n";
    return false;
  }

  theIMU[0] = asin(-imu[2]);
  theIMU[1] = -atan2(imu[5], imu[8]);
  return true;
}

MessageQueue* SimSparkController::createMessageQueue(const std::string& /*name*/)
{
  if ( theSyncMode ) {
    return new MessageQueue();
  } else {
    return new MessageQueue4Threads();
  }
}

void SimSparkController::get(TeamMessageDataIn& data)
{
  data.data.clear();
  for(vector<string>::const_iterator iter=theTeamMessageDataIn.data.begin();
      iter!=theTeamMessageDataIn.data.end(); ++iter)
  {
    if ( !(*iter).empty() )
    {
      // TODO: make this faster
      ASSERT(iter->size() < MAX_TEAM_MESSAGE_SIZE);
      int len = theBase64Decoder.decode( iter->c_str(), static_cast<int>(iter->size()), theTeamMessageReceiveBuffer);

      data.data.push_back( std::string(theTeamMessageReceiveBuffer, len) );
    }
  }
  theTeamMessageDataIn.data.clear();
}

void SimSparkController::set(const TeamMessageDataOut& data)
{
  if ( !data.data.empty() )
  {
    theTeamMessageDataOut.data = theBase64Encoder.encode(data.data.c_str(),(int)data.data.size());
  }
}

void SimSparkController::calculateNextActTime()
{
   theNextActTime = theLastSenseTime + 10;
   while(theLastActTime > theNextActTime)
   {
     theNextActTime += getBasicTimeStep();
   }
}
