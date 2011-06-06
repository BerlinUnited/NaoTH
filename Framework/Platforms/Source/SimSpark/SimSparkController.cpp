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
//#include "Tools/NaoInfo.h"
#include <Tools/ImageProcessing/ColorModelConversions.h>
#include <Tools/DataConversion.h>

//#include "CommunicationCollectionImpl.h"
//#include "Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/DebugModify.h"

using namespace std;

SimSparkController::SimSparkController()
: PlatformInterface<SimSparkController>("SimSpark", 20),
  theImageData(NULL),
  theImageSize(0),
  isNewImage(false),
  isNewVirtualVision(false)
{
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

  // register output
  registerOutput<const CameraSettingsRequest>(*this);
  registerOutput<const MotorJointData>(*this);


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


  theCameraId = 0;
  theSenseTime = 0;

  if (!g_thread_supported())
    g_thread_init(NULL);
  theCognitionInputMutex = g_mutex_new();
  theCognitionInputCond = g_cond_new();

  maxJointAbsSpeed = Math::fromDegrees(351.77);

  theTeamName = "NaoTH";
}

SimSparkController::~SimSparkController()
{
  g_mutex_free(theCognitionInputMutex);
  g_cond_free(theCognitionInputCond);
  
  if (theImageData != NULL)
    delete [] theImageData;
}

bool SimSparkController::init(const std::string& teamName, unsigned int num, const std::string& server, unsigned int port)
{

  theTeamName = teamName;
  // connect to the simulator
  if(!theSocket.connect(server, port))
  {
    std::cerr << "SimSparkController could not connect" << std::endl;
    return false;
  }

  // send create command to simulator

  theSocket << "(scene rsg/agent/nao/nao.rsg)" << send;
  // wait the response
  updateSensors();
  // initialize the teamname and number
  theSocket << "(init (teamname " << teamName << ")(unum " << num<< "))" << send;
  // wait the response
  while (theGameInfo.thePlayerNum == 0)
  {
    updateSensors();
  }
  // we should get the team index and player number now

  // calculate debug communicaiton port
  unsigned short debugPort = 5401;
  if (theGameInfo.theTeamIndex == SimSparkGameInfo::TI_LEFT )
  {
    debugPort = 5400 + theGameInfo.thePlayerNum;
  } else if (theGameInfo.theTeamIndex == SimSparkGameInfo::TI_RIGHT)
  {
    debugPort = 5500 + theGameInfo.thePlayerNum;
  }

  //Platform::getInstance().init(this, new SimSparkCommunicationCollection(debugPort,theGameInfo, theTeamComm));
  
  Platform::getInstance().init(this);


  /*
  if (theGameInfo.theTeamIndex == SimSparkGameInfo::TI_LEFT ){
    thePlayerInfoInitializer.thePlayerInfo.teamColor = PlayerInfo::blue;
    thePlayerInfoInitializer.thePlayerInfo.teamNumber = 1;
  }
  else if (theGameInfo.theTeamIndex == SimSparkGameInfo::TI_RIGHT ){
    thePlayerInfoInitializer.thePlayerInfo.teamColor = PlayerInfo::red;
    thePlayerInfoInitializer.thePlayerInfo.teamNumber = 2;
  }
  thePlayerInfoInitializer.thePlayerInfo.playerNumber = theGameInfo.thePlayerNum;
  */

  cout << "NaoTH Simpark initialization successful: " << teamName << " " << theGameInfo.thePlayerNum << endl;

  initPosition();
  //DEBUG_REQUEST_REGISTER("SimSparkController:beam", "beam to start pose", false);

  return true;
}

void SimSparkController::initPosition()
{
  ifstream ifile("init_position.txt");
  std::map<int, Vector3<double> > positions;
  while (!ifile.fail() && !ifile.eof() )
  {
    int num;
    Vector3<double> p;
    ifile>>num>>p.x>>p.y>>p.z;
    positions[num] = p;
  }

  if ( positions.find( theGameInfo.thePlayerNum ) != positions.end() )
  {
    startPose =positions[theGameInfo.thePlayerNum];
    beam(startPose);
  }
}

void SimSparkController::main()
{
  cout << "SimSpark Controller runs in single thread" << endl;
  while ( updateSensors() )
  {
    callCognition();
    callMotion();
  }//end while
}//end main

void SimSparkController::motionLoop()
{
  while ( updateSensors() )
  {    
    callMotion();
  }
}//end motionLoop

void SimSparkController::cognitionLoop()
{
  while (true)
  {
    callCognition();
  }
}//end cognitionLoop


void* motionLoopWrap(void* c)
{
  SimSparkController* ctr = static_cast<SimSparkController*> (c);
  ctr->motionLoop();
  return NULL;
}//end motionLoopWrap

void SimSparkController::multiThreadsMain()
{
  cout << "SimSpark Controller runs in multi-threads" << endl;

  callMotion();

  GThread* motionThread = g_thread_create(motionLoopWrap, this, true, NULL);
  ASSERT(motionThread != NULL);

  cognitionLoop();
}//end multiThreadsMain

void SimSparkController::getMotionInput()
{
  PlatformInterface<SimSparkController>::getMotionInput();

  for (int i = 0; i < JointData::numOfJoint; i++)
  {
    theSensorJointData.stiffness[i] = theLastSensorJointData.stiffness[i];
  }
  theLastSensorJointData = theSensorJointData;
}

void SimSparkController::setMotionOutput()
{
  PlatformInterface<SimSparkController>::setMotionOutput();
  say();
  autoBeam();
  jointControl();
  theSocket << send;
}

void SimSparkController::getCognitionInput()
{
  g_mutex_lock(theCognitionInputMutex);
  while (!isNewImage)
  {
    g_cond_wait(theCognitionInputCond, theCognitionInputMutex);
  }
  PlatformInterface<SimSparkController>::getCognitionInput();
  g_mutex_unlock(theCognitionInputMutex);
}

bool SimSparkController::updateSensors()
{
  double lastSenseTime = theSenseTime;
  string msg;
  theSocket >> msg;

  //cout << "Sensor data: " << msg << endl;

  pcont_t* pcont;
  sexp_t* sexp = NULL;

  char* c = const_cast<char*> (msg.c_str());
  pcont = init_continuation(c);
  sexp = iparse_sexp(c, msg.size(), pcont);

  g_mutex_lock(theCognitionInputMutex);

  // clear FSR data, since if there is no FSR data, it means no toch
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
      if ("HJ" == name) ok = updateHingeJoint(t->next); // hinge joint
      else if ("FRP" == name)
      {
        ok = updateFSR(t->next); // force sensor
      } else if ("See" == name)
      {// See
        theVirtualVision.clear();
        ok = updateSee("", t->next);
        if ( ok ) isNewVirtualVision = true;
        
        //HACK: assume the image is behind of "See"
        int offset = paseImage(pcont->lastPos);
        pcont->lastPos = &(pcont->lastPos[offset]);
        isNewImage = offset > 0;
      } else if ("time" == name)
      {
        ok = SexpParser::parseGivenValue(t->next, "now", theSenseTime); // time
        theStepTime = theSenseTime - lastSenseTime;
        if ( static_cast<unsigned int>(theStepTime*100)*10 > getBasicTimeStep() )
          cerr<<"warning: the step is "<<theStepTime<<" s"<<endl;
      } else if ("GYR" == name) ok = updateGyro(t->next); // gyro rate
      else if ("ACC" == name) ok = updateAccelerometer(t->next);
      else if ("GS" == name) ok = updateGameInfo(t->next); // game state
      else if ("hear" == name)  ok = hear(t->next);// hear
      else if ("IMU" == name) ok = updateIMU(t->next); // interial sensor data
      else if ("IMG" == name)
      {
        // HACK: image parsing is very slow in Windows
        // thus we parse the image separately (cf. the case "See")
        //ok = updateImage(t->next); // image from camera
        //if (ok) isNewImage = true;
      }
      else cerr << " Perception unknow name: " << string(t->val) << endl;
      if (!ok)
      {
        cerr << " Perception update failed: " << string(t->val) << endl;
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
  }//end while

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
}//end parseString


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
      int dl = theBase64Decoder.decode(dsexp->val, dsexp->val_used, theImageData);
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

  double acc[3];
  if (!SexpParser::parseGivenArrayValue(sexp->next, "a", 3, acc))
  {
    cerr << "can not get the Accelerometer data!\n";
    return false;
  }

  swap(acc[0], acc[1]);
  acc[1] = -acc[1];
  double k = 0.1;
  for (int i = 0; i < AccelerometerData::numOfAccelerometer; i++)
  {
    theAccelerometerData.data[i] = theAccelerometerData.data[i]*(1-k) + k*acc[i];
  }

  return true;
}//end updateAccelerometer


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
        if (!SexpParser::parseValue(t->next, theGameInfo.theGameTime))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get time value\n";
        }
      } else if ("pm" == name) // play mode
      {
        std::string pm;
        if (!SexpParser::parseValue(t->next, pm))
        {
          ok = false;
          cerr << "SimSparkGameInfo::update failed get play mode value\n";
        }
        theGameInfo.thePlayMode = SimSparkGameInfo::getPlayModeByName(pm);
      } else if ("unum" == name) // unum
      {
        if (!SexpParser::parseValue(t->next, theGameInfo.thePlayerNum))
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
        theGameInfo.theTeamIndex = SimSparkGameInfo::getTeamIndexByName(team);
      } else
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
  Vector3d F = decomposeForce(f, x, y, FSRData::offset[id0], FSRData::offset[id1], FSRData::offset[id2]);
  theFSRData.data[id0] += F.x;
  theFSRData.data[id1] += F.y;
  theFSRData.data[id2] += F.z;
}

bool SimSparkController::updateSee(const string& preName, const sexp_t* sexp)
{
  std::string name;
  while (sexp)
  {
    name = "";
    const sexp_t* t = sexp->list;
    if (SEXP_VALUE == t->ty)
    {
      SexpParser::parseValue(t, name);
      if ("P" == name) // a player
      {
        string teamName;
        t = t->next;
        if (!SexpParser::parseGivenValue(t, "team", teamName))
          cerr << "[SimSparkController] Vision can not get the Player's team" << endl;

        string id;
        t = t->next;
        if (!SexpParser::parseGivenValue(t, "id", id))
          cerr << "[SimSparkController] Vision can not get Player's id" << endl;
        if ( !updateSee("P "+teamName+" "+id+" ", t->next) )
          return false;
      }
      else if ("L"==name)
      {
        double p0[3], p1[3];
        if ( SexpParser::parseGivenArrayValue(t->next, "pol", 3, p0) && SexpParser::parseGivenArrayValue(t->next->next, "pol", 3, p1))
        {
          VirtualVision::Line l;
          l.p0 = Vector3d(p0[0]*1000, Math::fromDegrees(p0[1]), Math::fromDegrees(p0[2]));
          l.p1 = Vector3d(p1[0]*1000, Math::fromDegrees(p1[1]), Math::fromDegrees(p1[2]));
          theVirtualVision.lines.push_back(l);
        }
        else
        {
          cerr<<"[SimSparkController] Vision can not process line! " << endl;
        }
      }
      else
      {
        double d[3];
        if (SexpParser::parseGivenArrayValue(t->next, "pol", 3, d))
        {
          theVirtualVision.data[preName + name] = Vector3<double>(d[0]*1000, Math::fromDegrees(d[1]), Math::fromDegrees(d[2]));
        } else if (SexpParser::parseArrayValue(t->next, 3, d))
        {
          theVirtualVision.data[preName + name] = Vector3d(d[0], d[1], d[2])*1000;
        } else
        {
          cerr << "[SimSparkController] Vision can not get Object " << name << endl;
        }
      }
    }
    sexp = sexp->next;
  }

  return true;
}

void SimSparkController::get(FrameInfo& data)
{
  data.time = static_cast<unsigned int>(theSenseTime * 1000.0);
  data.frameNumber++;
  data.basicTimeStep = getBasicTimeStep();
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

void SimSparkController::get(Image& data)
{
  if (isNewImage)
  {
    data.setCameraInfo(Platform::getInstance().theCameraInfo);

    unsigned int width = data.cameraInfo.resolutionWidth;
    unsigned int height = data.cameraInfo.resolutionHeight;
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

    isNewImage = false;
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
  if (isNewVirtualVision)
  {
    data = theVirtualVision;
    isNewVirtualVision = false;
  }
}

void SimSparkController::get(SimSparkGameInfo& data)
{
  data = theGameInfo;
}

void SimSparkController::updateInertialSensor()
{
  // calculate inertial sensor data by gyrometer
  const double *gyrometer = theGyroData.data;
  static double oldGyroX = gyrometer[0];
  static double oldGyroY = gyrometer[1];
  theInertialSensorData.data[InertialSensorData::X] += ((gyrometer[0]+oldGyroX) * 0.5 * theStepTime);
  theInertialSensorData.data[InertialSensorData::Y] += ((gyrometer[1]+oldGyroY) * 0.5 * theStepTime);
  oldGyroX = gyrometer[0];
  oldGyroY = gyrometer[1];

  // calculate intertial sensor data by accelerometer
  const double *acc = theAccelerometerData.data;
  double len = sqrt(Math::sqr(acc[0]) + Math::sqr(acc[1]) + Math::sqr(acc[2]));

  if (len > 2 && len < 30)
  {
    double x = asin(acc[1] / len);
    double cx = cos(x);
    double y = -atan2(acc[0] / Math::sgn(cx), acc[2] / Math::sgn(cx));

    double k = 0.04;
//    MODIFY("updateInertialSensor.k", k);
    theInertialSensorData.data[InertialSensorData::X] = (1 - k) * theInertialSensorData.data[InertialSensorData::X] + k * x;
    theInertialSensorData.data[InertialSensorData::Y] = (1 - k) * theInertialSensorData.data[InertialSensorData::Y] + k * y;
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
    double v = (target - ang) * d * data.stiffness[i];
    v = Math::clamp(v, -maxJointAbsSpeed, maxJointAbsSpeed);
    ang += (v * theStepTime);
    target = data2.position[i];
    double v2 = (target - ang) * d * data2.stiffness[i];
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
    theSocket << '(' << theJointMotorNameMap[(JointData::JointID)i] << ' '
      << v2
      << ')';
  }
}

void SimSparkController::set(const CameraSettingsRequest& data)
{
  // switch between two cameras is supported currently

  // switch camera
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
}

void SimSparkController::get(CurrentCameraSettings& data)
{
  data.data[CameraSettings::CameraSelection] = theCameraId;
}

void SimSparkController::say()
{/*
  // make sure all robot have chance to say something
  if ( ( static_cast<int>(floor(theSenseTime*1000/theBasicTimeStep/2)) % thePlayerInfoInitializer.thePlayerInfo.numOfPlayers) +1 != thePlayerInfoInitializer.thePlayerInfo.playerNumber )
    return;
  string msg = theTeamComm.peekSayMessage();
  if (!msg.empty()){
    if (msg.size()>20){
      cerr<<"SimSparkController: can not say a message longer than 20 "<<endl;
      return;
    }
    if (msg != "")
    {
//      cout<<"Nr."<<static_cast<int>(thePlayerInfoInitializer.thePlayerInfo.playerNumber)<<" say @ "<<theSenseTime<<endl;
      theSocket << ("(say "+msg+")");
    }
  }
  */
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
  }

  sexp = sexp->next;
  string msg;
  SexpParser::parseValue(sexp, msg);

  if ( !msg.empty() && msg != ""){
//    theTeamComm.addHearMessage(msg);
  }
//  std::cout << "hear message : " << time << ' ' << direction << ' ' << dir << ' ' << msg << std::endl;
  return true;
}

void SimSparkController::beam(const Vector3<double>& p)
{
    theSocket << "(beam "<<p.x<<" "<<p.y<<" "<<p.z<<")" << send;
}

void SimSparkController::autoBeam()
{
  /*
  DEBUG_REQUEST("SimSparkController:beam", beam(););

  static PlayerInfo::PlayMode lastPlayMode = PlayerInfo::numOfPlayMode;
  if (theGameInfo.thePlayMode == PlayerInfo::PM_GOAL_LEFT
    || theGameInfo.thePlayMode == PlayerInfo::PM_GOAL_RIGHT
    || (theGameInfo.thePlayMode == PlayerInfo::PM_BEFORE_KICK_OFF && theGameInfo.theGameTime > 1))
  {
//    if ( lastPlayMode != theGameInfo.thePlayMode ){
//      beam();// execute once
//    }
    if (int(theSenseTime / theStepTime) % 20 == 0)
    {
      beam();
    }
//    MotorJointData initJoint;
//    for (int i = 0; i < JointData::numOfJoint; i++)
//    {
//      initJoint.hardness[i] = theSensorJointData.hardness[i];
//    }
//    set(initJoint);
  }
  lastPlayMode = theGameInfo.thePlayMode;
  */
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
