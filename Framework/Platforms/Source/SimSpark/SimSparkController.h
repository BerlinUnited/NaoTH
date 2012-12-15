/**
 * @file SimSparkController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the SimSpark simulator
 *
 */

#ifndef _SIMSPARKCONTROLLER_H
#define  _SIMSPARKCONTROLLER_H


#include <glib.h>
#include <map>

#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/GameData.h>

#include "SimSparkGameInfo.h"


#include <Tools/Communication/SocketStream/SocketStream.h>
#include "TeamCommEncoder.h"

#include "PlatformInterface/PlatformInterface.h"
#include <DebugCommunication/DebugCommandExecutor.h>
#include "DebugCommunication/DebugServer.h"

#include "sfsexp/SexpParser.h"
#include "libb64/decode.h"

using namespace naoth;

class SimSparkController : public PlatformInterface<SimSparkController>, DebugCommandExecutor
{
private:
  GSocket* socket;
  PrefixedSocketStream theSocket;

  // sensor data
  std::map<std::string, JointData::JointID> theJointSensorNameMap;
  std::map<JointData::JointID, std::string> theJointMotorNameMap;

  char* theImageData;
  unsigned int theImageSize;
  bool isNewImage;
  bool isNewVirtualVision;
  VirtualVision theVirtualVision;
  base64::Decoder theBase64Decoder;

  SensorJointData theLastSensorJointData;
  GyrometerData theGyroData;
  AccelerometerData theAccelerometerData;
  FSRData theFSRData;
  FrameInfo theFrameInfo;

  int theCameraId;
  CameraInfo theCameraInfo;

  double theSenseTime;
  double theStepTime; // the time of last step in seconds
  
  InertialSensorData theInertialSensorData;
  GameData theGameData;
  SensorJointData theSensorJointData;
  TeamMessageDataOut theTeamMessageDataOut; // message to other robots
  TeamMessageDataIn theTeamMessageDataIn; // message from other robots
  double theIMU[2];

  std::list<MotorJointData> theMotorJointData;
  std::string theSync;
  bool theSyncMode;

  TeamCommEncoder theTeamCommEncoder;

public:
  SimSparkController(const std::string& name);

  virtual ~SimSparkController();

  virtual std::string getBodyID() const;

  virtual std::string getBodyNickName() const;

  virtual std::string getHeadNickName() const;

  /////////////////////// init ///////////////////////
  bool init(const std::string& teamName, unsigned int num, const std::string& server, unsigned int port, bool sync);

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

  void get(CurrentCameraSettings& data);

  void get(BatteryData& data) {data.charge = 1.0;}

  void get(VirtualVision& data);

  void get(TeamMessageDataIn& data);

  void get(GameData& data);

  /////////////////////// set ///////////////////////
  void set(const MotorJointData& data);

  void set(const TeamMessageDataOut& data);

  void set(const CameraSettingsRequest& data);

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
  int paseImage(char* data);


  bool updateImage(const sexp_t* sexp);

  bool updateHingeJoint(const sexp_t* sexp);

  bool updateGyro(const sexp_t* sexp);

  bool updateAccelerometer(const sexp_t* sexp);

  bool updateGameInfo(const sexp_t* sexp);

  bool updateFSR(const sexp_t* sexp);

  bool updateSee(const std::string& preName, const sexp_t* sexp);

  bool updateIMU(const sexp_t* sexp);

  Vector3d decomposeForce(double f, double fx, double fy, const Vector3d& c0, const Vector3d& c1, const Vector3d& c2);

  void calFSRForce(double f, double x, double y, FSRData::FSRID id0, FSRData::FSRID id1, FSRData::FSRID id2);

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
  GMutex*  theCognitionInputMutex;
  GMutex*  theCognitionOutputMutex;
  GCond* theCognitionInputCond;
  double maxJointAbsSpeed;
  bool exiting;

  //
  unsigned int theLastActTime;
  unsigned int theLastSenseTime;
  unsigned int theNextActTime;
  void calculateNextActTime();
  GCond* theTimeCond;
  GMutex* theTimeMutex;

  std::string theSensorData;
  GMutex* theSensorDataMutex;
  GCond* theSensorDataCond;

  GMutex*  theActDataMutex;
  std::stringstream theActData;
  void act();

private:
  DebugServer theDebugServer;
public:
  void get(DebugMessageIn& data)
  {
    theDebugServer.getDebugMessageIn(data);
  }

  void set(const DebugMessageOut& data)
  {
    if(data.answers.size() > 0)
      theDebugServer.setDebugMessageOut(data);
  }
};

#endif  /* _SIMSPARKCONTROLLER_H */

