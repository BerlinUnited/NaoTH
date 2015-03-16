/**
 * @file SimSparkController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @breief Interface for the SimSpark simulator
 *
 */

#include "SimSparkController.h"
#include <iostream>
#include <fstream>
#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
#include <Tools/ImageProcessing/ColorModelConversions.h>
#include <Tools/DataConversion.h>
//#include <Tools/Debug/DebugRequest.h>
#include <DebugCommunication/DebugCommandManager.h>
#include <Tools/NaoTime.h>
#include <Tools/NaoInfo.h>

using namespace std;

SimSparkController::SimSparkController(const std::string& name)
: PlatformInterface(name, 20),
  theImageData(NULL),
  theImageSize(0),
  isNewImage(false),
  isNewVirtualVision(false),
  theCameraId(0),
  theSenseTime(0),
  theStepTime(0),
  theSyncMode(false),
  exiting(false)
{
  theGameData.gameState = GameData::unknown;

  // register input
  registerInput<AccelerometerData>(*this);
  registerInput<FrameInfo>(*this);
  registerInput<SensorJointData>(*this);
  registerInput<Image>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<CurrentCameraSettings>(*this);
  registerInput<BatteryData>(*this);
  registerInput<VirtualVision>(*this);
  registerInput<VirtualVisionTop>(*this);
  registerInput<TeamMessageDataIn>(*this);
  registerInput<GameData>(*this);
  registerInput<GPSData>(*this);
  registerInput<BatteryData>(*this);

  // register output
  registerOutput<const CameraSettingsRequest>(*this);
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

  if (!g_thread_supported()) {
    g_thread_init(NULL);
  }

  theCognitionInputMutex = g_mutex_new();
  theCognitionOutputMutex = g_mutex_new();
  theCognitionInputCond = g_cond_new();

  theTimeMutex = g_mutex_new();
  theTimeCond = g_cond_new();
  theSensorDataMutex = g_mutex_new();
  theSensorDataCond = g_cond_new();
  theActDataMutex = g_mutex_new();

  maxJointAbsSpeed = Math::fromDegrees(351.77);

  GError *err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);
  if (err)
  {
    socket = NULL;
    g_warning("Could not create a socket. This is a fatal error and communication is available. Error message:\n%s", err->message);
    g_error_free (err);
  }
}

SimSparkController::~SimSparkController()
{
  g_mutex_free(theCognitionInputMutex);
  g_mutex_free(theCognitionOutputMutex);
  g_cond_free(theCognitionInputCond);

  g_mutex_free(theTimeMutex);
  g_cond_free(theTimeCond);
  g_mutex_free(theSensorDataMutex);
  g_cond_free(theSensorDataCond);
  g_mutex_free(theActDataMutex);

  if (socket != NULL) {
    g_socket_close(socket, NULL);
  }

  if (theImageData != NULL) {
    delete [] theImageData;
  }
}

string SimSparkController::getBodyNickName() const
{
  return theGameData.teamName + DataConversion::toStr(theGameData.playerNumber);
}

string SimSparkController::getHeadNickName() const
{
  return theGameData.teamName + DataConversion::toStr(theGameData.playerNumber);
}

string SimSparkController::getBodyID() const
{
  return DataConversion::toStr(theGameData.playerNumber);
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
    g_warning("Could not connect:\n%s", error->message);
    g_error_free(error);
  }

  if (conn_error) {
    g_warning("Could not connect:\n%s", conn_error->message);
    g_error_free(conn_error);
  }

  return false;
}//end connect

bool SimSparkController::init(const std::string& modelPath, const std::string& teamName, unsigned int playerNumber, const std::string& server, unsigned int port, bool sync)
{
  Platform::getInstance().init(this);
  theGameData.loadFromCfg(Platform::getInstance().theConfiguration);

  theGameData.teamName = teamName;
  theSync = sync?"(syn)":"";
  theSyncMode = sync;
  // connect to the simulator

  if(!connect(server, port))
  {
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
  theSocket << "(init (teamname " << teamName << ")(unum " << playerNumber << "))" << theSync << send;
  
  // wait the response
  while (theGameData.playerNumber == 0)
  {
    getSensorData(theSensorData);
    updateSensors(theSensorData);
    theSocket << theSync << send;
  }
  // we should get the team index and player number now

#ifdef DEBUG
  // calculate debug communicaiton port
  unsigned short debugPort = 5401;
  if (theGameData.teamColor == GameData::blue ) {
    debugPort = static_cast<short unsigned int> (5400 + theGameData.playerNumber);
  } else if (theGameData.teamColor == GameData::red ) {
    debugPort = static_cast<short unsigned int> (5500 + theGameData.playerNumber);
  }

  theDebugServer.start(debugPort);
#endif

  cout << "NaoTH Simpark initialization successful: " << teamName << " " << theGameData.playerNumber << endl;

  //DEBUG_REQUEST_REGISTER("SimSparkController:beam", "beam to start pose", false);
  //REGISTER_DEBUG_COMMAND("beam", "beam to given pose", this);

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
    g_mutex_lock(theSensorDataMutex);
    g_cond_wait(theSensorDataCond, theSensorDataMutex);
    string data = theSensorData;
    theSensorData = "";
    g_mutex_unlock(theSensorDataMutex);

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

void* motionLoopWrap(void* c)
{
  SimSparkController* ctr = static_cast<SimSparkController*> (c);
  ctr->motionLoop();
  return NULL;
}//end motionLoopWrap

void* senseLoopWrap(void* c)
{
  SimSparkController* ctr = static_cast<SimSparkController*> (c);
  ctr->senseLoop();
  return NULL;
}

void* actLoopWrap(void* c)
{
  SimSparkController* ctr = static_cast<SimSparkController*> (c);
  ctr->actLoop();
  return NULL;
}

void SimSparkController::senseLoop()
{
  while( true )
  {
    string data;
    if ( !getSensorData(data) )
    {
      break;
    }

    g_mutex_lock(theSensorDataMutex);
    if ( !theSensorData.empty() )
    {
      cerr<<"[Warning] the sensor data @ " << theLastSenseTime << " is dropped!"<<endl;
    }
    theSensorData = data;
    g_mutex_unlock(theSensorDataMutex);
    g_cond_signal(theTimeCond);
    g_cond_signal(theSensorDataCond);
  }

  exiting = true;
  g_cond_signal(theSensorDataCond); // tell motion to exit
  g_cond_signal(theTimeCond); // tell act loop to exit
  g_cond_signal(theCognitionInputCond); // tell cognition to exit
}

void SimSparkController::actLoop()
{
  while( !exiting ){
    g_mutex_lock(theTimeMutex);
    g_cond_wait(theTimeCond, theTimeMutex);
    calculateNextActTime();
    g_mutex_unlock(theTimeMutex);
    unsigned int now = NaoTime::getNaoTimeInMilliSeconds();
    if ( theNextActTime > now )
    {
      unsigned int t = theNextActTime - now;
#ifdef WIN32
      Sleep(t);
#else
      usleep(t * 1000);
#endif
    }
    act();
  }
}

void SimSparkController::act()
{
  // send command
  g_mutex_lock(theActDataMutex);
  try{
    theSocket << theActData.str() << theSync << send;
    theActData.str("");
  }
  catch(std::runtime_error& exp)
  {
    cerr<<"can not send data to server, because of "<<exp.what()<<endl;
  }

  g_mutex_unlock(theActDataMutex);
}

void SimSparkController::multiThreadsMain()
{
  cout << "SimSpark Controller runs in multi-threads" << endl;

  GThread* senseThread = g_thread_create(senseLoopWrap, this, true, NULL);
  GThread* actThread = g_thread_create(actLoopWrap, this, true, NULL);
  GThread* motionThread = g_thread_create(motionLoopWrap, this, true, NULL);

  ASSERT(senseThread != NULL);
  ASSERT(actThread != NULL);
  ASSERT(motionThread != NULL);

  cognitionLoop();

  g_thread_join(motionThread);
  g_thread_join(actThread);
  g_thread_join(senseThread);
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

  g_mutex_lock(theActDataMutex);
  say();
  autoBeam();
  jointControl();
  theActData << theSync;
  g_mutex_unlock(theActDataMutex);
}

void SimSparkController::getCognitionInput()
{
  g_mutex_lock(theCognitionInputMutex);
  while (!isNewImage && !isNewVirtualVision && !exiting )
  {
    g_cond_wait(theCognitionInputCond, theCognitionInputMutex);
  }

  PlatformInterface::getCognitionInput();
  isNewVirtualVision = false;
  isNewImage = false;
  g_mutex_unlock(theCognitionInputMutex);
}

void SimSparkController::setCognitionOutput()
{
  g_mutex_lock(theCognitionOutputMutex);
  PlatformInterface::setCognitionOutput();
  g_mutex_unlock(theCognitionOutputMutex);
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

  g_mutex_lock(theCognitionInputMutex);

  // clear FSR data, since if there is no FSR data, it means no touch
  for (int i = 0; i < FSRData::numOfFSR; i++)
  {
    theFSRData.data[i] = 0;
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
        int offset = paseImage(pcont->lastPos);
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
    g_cond_signal(theCognitionInputCond);
  }
  g_mutex_unlock(theCognitionInputMutex);

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
  DataConversion::strTo(tmp, value);
  return c;
}


int SimSparkController::paseImage(char* data)
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

  theSensorJointData.dp[jid] = Math::clamp(Math::normalizeAngle(ax - theSensorJointData.position[jid]) / theStepTime,
    -maxJointAbsSpeed, maxJointAbsSpeed);
  theSensorJointData.position[jid] = ax;
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
  if(theGameData.teamColor == GameData::red) {
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
  const unsigned int half_time = 5 * 60 * 1000;
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
          theGameData.gameTime = static_cast<unsigned int>(gameTime)*1000;
          theGameData.firstHalf = (theGameData.gameTime < half_time);
          if ( theGameData.firstHalf )
          {
            theGameData.msecsRemaining = half_time - theGameData.gameTime;
          }
          else
          {
            theGameData.msecsRemaining = half_time*2 - theGameData.gameTime;
          }
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
        if (state == GameData::unknown)
        {
          // try SimSpark play mode
          SimSparkGameInfo::PlayMode splayMode = SimSparkGameInfo::getPlayModeByName(pm);
          GameData::PlayMode playMode = SimSparkGameInfo::covertPlayMode(splayMode, theGameData.teamColor);
          if ( theGameData.playMode != playMode )
          {
            theGameData.playMode = playMode;
            theGameData.timeWhenPlayModeChanged = theFrameInfo.getTime();
          }
        }
        else
        {
          if ( theGameData.gameState != state )
          {
            theGameData.gameState = state;
            theGameData.timeWhenPlayModeChanged = theFrameInfo.getTime();
          }
        }
      } else if ("unum" == name) // unum
      {
        if (!SexpParser::parseValue(t->next, theGameData.playerNumber))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get unum value\n";
        }
      } else if ("team" == name) // side
      {
        string team;
        if (!SexpParser::parseValue(t->next, team))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get team index value\n";
        }
        theGameData.teamColor = SimSparkGameInfo::getTeamColorByName(team);
        theGameData.teamNumber = theGameData.teamColor;
      } 
      else if ("sl" == name)
      {
        int score_left = 0;
        if (!SexpParser::parseValue(t->next, score_left))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed score left\n";
        }
      } 
      else if ("sr" == name)
      {
        int score_right = 0;
        if (!SexpParser::parseValue(t->next, score_right))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed score right\n";
        }
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

  if ( ok )
  {
    theGameData.frameNumber = theFrameInfo.getFrameNumber();
    theGameData.valid = true;
  }
  return ok;
}//end updateGameInfo


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


  FSRData::FSRID id0, id1, id2, id3;
  if ("lf" == name)
  {
    id0 = FSRData::LFsrBL;
    id1 = FSRData::LFsrBR;
    id2 = FSRData::LFsrFL;
    id3 = FSRData::LFsrFR;
  } else if ("rf" == name)
  {
    id0 = FSRData::RFsrBL;
    id1 = FSRData::RFsrBR;
    id2 = FSRData::RFsrFL;
    id3 = FSRData::RFsrFR;
  } else
  {
    cerr << "unknow ForceResistancePerceptor name: " << name << endl;
    return false;
  }

  double f = F[2] / 4;
  double fx = f * (C[1]*1000 + 30);
  double fy = f * (-C[0]*1000);
  calFSRForce(f, fx, fy, id0, id1, id2);
  calFSRForce(f, fx, fy, id1, id2, id3);
  calFSRForce(f, fx, fy, id2, id3, id0);
  calFSRForce(f, fx, fy, id3, id0, id1);

  return true;
}

Vector3d SimSparkController::decomposeForce(double f, double fx, double fy, const Vector3d& c0, const Vector3d& c1, const Vector3d& c2)
{
  Matrix3x3<double> A(Vector3d(1, c0.x, c0.y), Vector3d(1, c1.x, c1.y), Vector3d(1, c2.x, c2.y));
  return A.invert() * Vector3d(f, fx, fy);
}

void SimSparkController::calFSRForce(double f, double x, double y, FSRData::FSRID id0, FSRData::FSRID id1, FSRData::FSRID id2)
{
  Vector3d F = decomposeForce(f, x, y, NaoInfo::FSRPositions[id0], NaoInfo::FSRPositions[id1], NaoInfo::FSRPositions[id2]);
  theFSRData.data[id0] += F.x;
  theFSRData.data[id1] += F.y;
  theFSRData.data[id2] += F.z;
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
  for (int i = 0; i < FSRData::numOfFSR; i++)
  {
    theFSRData.force[i] = Math::clamp(theFSRData.data[i], 0.0, 25.0);
  }
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
  if ( theGameData.valid )
  {
    data = theGameData;
    theGameData.valid = false;
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

void SimSparkController::set(const CameraSettingsRequest& /*data*/)
{
  // switch between two cameras is supported currently

  // switch camera
  /*
  TODO: check if it's necessary
  if (theCameraId != data.data[CameraSettings::CameraSelection])
  {
    theCameraId = data.data[CameraSettings::CameraSelection];
    Pose3D p;
    const Pose3D& cameraTrans = Platform::getInstance().theCameraInfo.transformation[theCameraId];

    // due to the different coordination
    p.translation = RotationMatrix::getRotationZ(Math::fromDegrees(90)) * (cameraTrans.translation) * 0.001;
    p.rotation = RotationMatrix::getRotationZ(Math::fromDegrees(90));
    p.rotation *= cameraTrans.rotation;
    p.rotation.rotateZ(Math::fromDegrees(-90));

    theSocket << "(CameraPoseEffector " << p << ")";
  }
  */
}//end set

void SimSparkController::get(CurrentCameraSettings& data)
{
  data.data[CameraSettings::CameraSelection] = theCameraId;
}


void SimSparkController::say()
{
  if ( theGameData.numOfPlayers == 0 )
    return;

  // make sure all robot have chance to say something
  if ( ( static_cast<int>(floor(theSenseTime*1000/getBasicTimeStep()/2)) % theGameData.numOfPlayers) +1 != theGameData.playerNumber )
    return;

  if ( g_mutex_trylock(theCognitionOutputMutex) )
  {
    string& msg = theTeamMessageDataOut.data;
    if (!msg.empty()){
      if (msg.size()<=20)
      {
        if (msg != "")
        {
          theActData << ("(say "+msg+")");
        }
        msg.clear();
      }
      else
      {
        cerr<<"SimSparkController: can not say a message longer than 20 "<<endl;
      }
    }
    g_mutex_unlock(theCognitionOutputMutex);
  }
}

bool SimSparkController::hear(const sexp_t* sexp)
{
  double time;
  if (!SexpParser::parseValue(sexp, time))
  {
    std::cerr << "[SimSparkController Hear] can not get time" << std::endl;
    return false;
  }

  sexp = sexp->next;
  /*
  std::string direction;
  double dir;
  if (!SexpParser::parseValue(sexp, direction))
  {
    std::cerr << "[SimSparkController Hear] can not get direction" << std::endl;
    return false;
  }


  if ("self" == direction)
  {
    // this message come from myself, just omit it
//    return true;
  } else
  {
    if (!SexpParser::parseValue(sexp, dir))
    {
      std::cerr << "[SimSparkController Hear] can not parse the direction" << std::endl;
      return false;
    }
  }*/

  sexp = sexp->next;
  string msg;
  SexpParser::parseValue(sexp, msg);

  if ( !msg.empty() && msg != ""){
    theTeamMessageDataIn.data.push_back(msg);
  }
//  std::cout << "hear message : " << time << ' ' << direction << ' ' << dir << ' ' << msg << std::endl;
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
      || theGameData.gameState == GameData::initial
    || theGameData.playMode == GameData::goal_own
    || theGameData.playMode == GameData::goal_opp
    || theGameData.playMode == GameData::before_kick_off)
  {
    if ( beamRequest || theFrameInfo.getTime() - theGameData.timeWhenPlayModeChanged < 1000 )
    {
      const Configuration& cfg = Platform::getInstance().theConfiguration;
      string group = "PoseBeforeKickOff";
      if ( !cfg.hasGroup(group) )
      {
        cerr<<"SimSparkController: can not beam, because there is no configuration"<<endl;
        return;
      }

      stringstream key;
      key<<"Player"<<theGameData.playerNumber<<".Pose.";
      string keyx = key.str()+"x";
      string keyy = key.str()+"y";
      string keyr = key.str()+"rot";
      if ( ! (cfg.hasKey(group, keyx) && cfg.hasKey(group, keyy) && cfg.hasKey(group, keyr)) )
      {
        cerr<<"SimSparkController: can not beam, because configuration for Player "<<theGameData.playerNumber
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
    string msg = theTeamCommEncoder.decode(*iter);
    if ( !msg.empty() )
    {
      data.data.push_back( msg );
    }
  }
  theTeamMessageDataIn.data.clear();
}

void SimSparkController::set(const TeamMessageDataOut& data)
{
  if ( !data.data.empty() )
  {
    theTeamMessageDataOut.data = theTeamCommEncoder.encode(data.data);
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
