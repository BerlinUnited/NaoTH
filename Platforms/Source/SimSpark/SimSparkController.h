/**
 * @file SimSparkController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the SimSpark simulator
 *
 */

#ifndef _SIMSPARKCONTROLLER_H
#define	_SIMSPARKCONTROLLER_H


//#include <pthread.h>
#include <map>

#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/BumperData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/VirtualVision.h>

#include <Representations/Infrastructure/SimSparkGameInfo.h>


#include "Communication/SocketStream.h"
//#include "SimSparkTeamCommunicator.h"

#include "PlatformInterface/Platform.h"
#include "PlatformInterface/PlatformInterface.h"

#include "sfsexp/SexpParser.h"
#include "libb64/decode.h"

using namespace naoth;

class SimSparkController : public PlatformInterface<SimSparkController>
{
private:
  PrefixedSocketStream theSocket;

  // sensor data
  std::map<string, JointData::JointID> theJointSensorNameMap;
  map<JointData::JointID, string> theJointMotorNameMap;

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

  int theCameraId;
  CameraInfo theCameraInfo;

  double theSenseTime;
  double theStepTime; // the time of last step in seconds

  //SimSparkTeamCommunicator theTeamComm;

  virtual SimSparkController& getPlatform(){return *this;}
  
  InertialSensorData theInertialSensorData;
  SimSparkGameInfo theGameInfo;
  SensorJointData theSensorJointData;
  double theIMU[2];
  Vector3<double> startPose;

  list<MotorJointData> theMotorJointData;
  string theTeamName;
  
public:
  SimSparkController();

  virtual ~SimSparkController();

  virtual string getHardwareIdentity() const { return "simspark"; }

  virtual string getBodyID() { return "naoth-simspark"; }

  /* return the team name
   *  it is useful for distinguishing players from different teams in VirtualVision
   */
  virtual string getBodyNickName() {return theTeamName; }

  /////////////////////// init ///////////////////////
  bool init(const std::string& teamName, unsigned int num, const std::string& server, unsigned int port);

  // the main loop in single thread
  void main();

  // the main loop in three threads
  void multiThreadsMain();

public:
  void get(FrameInfo& data);

  void get(SensorJointData& data);

  void get(AccelerometerData& data);

  void get(Image& data);

  void get(GyrometerData& data);

  void get(FSRData& data);

  void get(InertialSensorData& data);

  void get(BumperData& data);

  void get(IRReceiveData& data);

  void get(CurrentCameraSettings& data);

  void get(ButtonData& data);

  void get(BatteryData& data) {data.charge = 1.0;}

  void get(UltraSoundReceiveData& data) { data.data = 0.0; };

  void get(VirtualVision& data);

  void get(SimSparkGameInfo& data);

  /////////////////////// set ///////////////////////
  void set(const MotorJointData& data);

  void set(const CameraSettingsRequest& data);

  void set(const LEDData& data);

  void set(const IRSendData& data);

  void set(const UltraSoundSendData& data);

  void set(const SoundData& data);

protected:
  virtual void updateInertialSensor();

private:
  bool updateSensors();

  bool updateImage(const sexp_t* sexp);

  bool updateHingeJoint(const sexp_t* sexp);

  bool updateGyro(const sexp_t* sexp);

  bool updateAccelerometer(const sexp_t* sexp);

  bool updateGameInfo(const sexp_t* sexp);

  bool updateFSR(const sexp_t* sexp);

  bool updateSee(const string& preName, const sexp_t* sexp);

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

  virtual void getMotionInput();

  virtual void setMotionOutput();

  virtual void getCognitionInput();

private:
  // members for threads
//  pthread_mutex_t  theCognitionInputMutex;
//  pthread_cond_t theCognitionInputCond;
  double maxJointAbsSpeed;

  //REPRESENTATION_PROVIDER(PlayerInfoInitializer, Cognition, PlayerInfo);
};

#endif	/* _SIMSPARKCONTROLLER_H */

