/**
 * @file SimSparkController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the SimSpark simulator
 *
 */

#ifndef _SIMSPARKCONTROLLER_H
#define _SIMSPARKCONTROLLER_H


#include <map>

#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/GPSData.h>
#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/GameData.h>


#include "SimSparkGameInfo.h"
#include "MessagesSPL/SPLStandardMessage.h"

#include <Tools/Communication/SocketStream/SocketStream.h>

#include "PlatformInterface/PlatformInterface.h"
#include <DebugCommunication/DebugCommandExecutor.h>
#include "DebugCommunication/DebugServer.h"

#include "sfsexp/SexpParser.h"
#include <Extern/libb64/decode.h>
#include <Extern/libb64/encode.h>
#include <set>
#include <mutex>
#include <condition_variable>

using namespace naoth;

// TODO: make this better
#define MAX_TEAM_MESSAGE_SIZE 1024

class SimSparkController : public PlatformInterface, DebugCommandExecutor
{
private:
  std::string thePlatformName;

  GSocket* socket;
  PrefixedSocketStream theSocket;

  // sensor data
  std::map<std::string, JointData::JointID> theJointSensorNameMap;
  std::map<JointData::JointID, std::string> theJointMotorNameMap;

  char* theTeamMessageReceiveBuffer;

  char* theImageData;
  unsigned int theImageSize;
  bool isNewImage;
  bool isNewVirtualVision;
  VirtualVision theVirtualVision;
  VirtualVisionTop theVirtualVisionTop;
  base64::Decoder theBase64Decoder;
  base64::Encoder theBase64Encoder;

  SensorJointData theLastSensorJointData;
  GyrometerData theGyroData;
  AccelerometerData theAccelerometerData;
  FSRData theFSRData;
  FrameInfo theFrameInfo;
  BatteryData theBatteryData;
  GPSData theGPSData;
  UltraSoundReceiveData theUSData;

  int theCameraId;
  CameraInfo theCameraInfo;

  double theSenseTime;
  double theStepTime; // the time of last step in seconds
  
  SimSparkGameInfo theGameInfo;
  InertialSensorData theInertialSensorData;
  SensorJointData theSensorJointData;
  TeamMessageDataOut theTeamMessageDataOut; // message to other robots
  TeamMessageDataIn theTeamMessageDataIn; // message from other robots
  double theIMU[2];

  std::list<MotorJointData> theMotorJointData;
  std::string theSync;
  bool theSyncMode;

  // set of unknown messages to be ignored
  std::set<std::string> ignore;
  bool ignoreOpponentMsg;

public:
  SimSparkController(const std::string& name);

  virtual ~SimSparkController();

  virtual std::string getBodyID() const;
  virtual std::string getBodyNickName() const;
  virtual std::string getHeadNickName() const;
  virtual std::string getRobotName() const { return getBodyNickName(); }
  virtual std::string getPlatformName() const { return thePlatformName; }
  virtual unsigned int getBasicTimeStep() const { return 20; }

  /////////////////////// init ///////////////////////
  bool init(const std::string& modelPath, const std::string& teamName, unsigned int teamNumber, unsigned int num, const std::string& server, unsigned int port, bool sync);

  void main();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);

public:
  void get(FrameInfo& data);

  void get(SensorJointData& data);

  void get(AccelerometerData& data);

  void get(Image& data);

  void get(GyrometerData& data);

  void get(FSRData& data);

  void get(InertialSensorData& data);

  void get(BatteryData& data);
  void get(GPSData& data);
  void get(UltraSoundReceiveData& data);

  void get(VirtualVision& data);
  void get(VirtualVisionTop& data);

  void get(TeamMessageDataIn& data);

  void get(GameData& data);

  /////////////////////// set ///////////////////////
  void set(const MotorJointData& data);

  void set(const TeamMessageDataOut& data);

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name);
  
  // the main loop in single thread
  void singleThreadMain();

  // the main loop in threads
  void multiThreadsMain();

  void updateInertialSensor();

private:
  bool connect(const std::string& host, int port);

  bool getSensorData(std::string& data);

  bool updateSensors(std::string& msg);

  int parseString(char* data, std::string& value);
  int parseInt(char* data, int& value);
  int parseImage(char* data);
  bool parsePoint3D(const sexp_t* sexp, Vector3d& result) const;
  bool parseTeamInfo(const sexp_t* team, std::vector<naoth::GameData::RobotInfo>& players);

  bool updateImage(const sexp_t* sexp);
  bool updateHingeJoint(const sexp_t* sexp);
  bool updateGyro(const sexp_t* sexp);
  bool updateAccelerometer(const sexp_t* sexp);
  bool updateBattery(const sexp_t* sexp);
  bool updateSonar(const sexp_t* sexp);
  bool updateGPS(const sexp_t* sexp);
  bool updateGameInfo(const sexp_t* sexp);
  bool updateFSR(const sexp_t* sexp);
  bool updateSee(VirtualVision& virtualVision, const sexp_t* sexp);
  bool updateIMU(const sexp_t* sexp);

  Vector3d decomposeForce(double f, double fx, double fy, const Vector3d& c0, const Vector3d& c1, const Vector3d& c2);

  void calFSRForce(double f, double x, double y, 
              const Vector3d* positions,
              std::vector<double>& values,
              FSRData::SensorID id0, FSRData::SensorID id1, FSRData::SensorID id2);

  void say();

  bool hear(const sexp_t* sexp);

  void beam(const Vector3<double>& p);

  void autoBeam();

  void jointControl();

  void initPosition();

public:
  void motionLoop();

  void cognitionLoop();

  void senseLoop();

  void actLoop();

  virtual void getMotionInput();

  virtual void setMotionOutput();

  virtual void getCognitionInput();

  virtual void setCognitionOutput();

  virtual void callCognition();

private:
  // members for threads
  std::mutex  theCognitionInputMutex;
  std::mutex  theCognitionOutputMutex;
  std::condition_variable theCognitionInputCond;
  double maxJointAbsSpeed;
  bool exiting;

  //
  unsigned int theLastActTime;
  unsigned int theLastSenseTime;
  unsigned int theNextActTime;
  void calculateNextActTime();
  std::condition_variable theTimeCond;
  std::mutex theTimeMutex;

  std::string theSensorData;
  std::mutex theSensorDataMutex;
  std::condition_variable theSensorDataCond;

  std::mutex  theActDataMutex;
  std::stringstream theActData;
  void act();

private:
  DebugServer theDebugServer;

public:
  void get(DebugMessageInCognition& data) {
    theDebugServer.getDebugMessageInCognition(data);
  }
  void get(DebugMessageInMotion& data) {
    theDebugServer.getDebugMessageInMotion(data);
  }

  void set(const DebugMessageOut& data)
  {
    if(data.answers.size() > 0)
      theDebugServer.setDebugMessageOut(data);
  }
};

#endif  /* _SIMSPARKCONTROLLER_H */

