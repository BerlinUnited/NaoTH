/**
 * @file SimSparkController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the SimSpark simulator
 *
 */

#ifndef _SIMSPARKCONTROLLER_H
#define	_SIMSPARKCONTROLLER_H


#include <pthread.h>
#include <map>

#include "Interface/Representations/Infrastructure/JointData.h"
#include "Interface/Representations/Infrastructure/AccelerometerData.h"
#include "Interface/Representations/Infrastructure/Image.h"
#include "Interface/Representations/Infrastructure/FrameInfo.h"
#include "Interface/Representations/Infrastructure/GyrometerData.h"
#include "Interface/Representations/Infrastructure/FSRData.h"
#include "Interface/Representations/Infrastructure/InertialSensorData.h"
#include "Interface/Representations/Infrastructure/BumperData.h"
#include "Interface/Representations/Infrastructure/IRData.h"
#include "Interface/Representations/Infrastructure/CameraSettings.h"
#include "Interface/Representations/Infrastructure/LEDData.h"
#include "Interface/Representations/Infrastructure/UltraSoundData.h"
#include "Interface/Representations/Infrastructure/SoundData.h"
#include "Interface/Representations/Infrastructure/ButtonData.h"
#include "Interface/Representations/Infrastructure/BatteryData.h"
#include "Representations/Infrastructure/VirtualVision.h"

/*
#include "Representations/Modeling/PlayerInfo.h"
*/

#include "Tools/Communication/SocketStream.h"
//#include "SimSparkTeamCommunicator.h"
#include "SimSparkGameInfo.h"

#include "Interface/PlatformInterface/Platform.h"
#include "Interface/PlatformInterface/PlatformInterface.h"

#include "sfsexp/SexpParser.h"

using namespace naorunner;

class SimSparkController : public PlatformInterface
{
private:
  PrefixedSocketStream<TCPSocket> theSocket;

  // sensor data
  std::map<string, JointData::JointID> theJointSensorNameMap;
  map<JointData::JointID, string> theJointMotorNameMap;

  std::string theImageData;

  SensorJointData theLastSensorJointData;
  GyrometerData theGyroData;
  AccelerometerData theAccelerometerData;
  FSRData theFSRData;

  int theCameraId;
  CameraInfo theCameraInfo;

  double theSenseTime;
  double theStepTime; // the time of last step in seconds

  //SimSparkTeamCommunicator theTeamComm;

protected:
  bool isNewImage;
  VirtualVision theVirtualVision;
  //REPRESENTATION_PROVIDER(VirtualVisionProvider, Cognition, VirtualVision);
  //InertialSensorData theInertialSensorData;
  SimSparkGameInfo theGameInfo;
  SensorJointData theSensorJointData;
  double theIMU[2];
  //Config startPoseCfg;

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

protected:
  virtual void get(FrameInfo& data);

  virtual void get(SensorJointData& data);

  virtual void get(AccelerometerData& data);

  virtual void get(Image& data);

  virtual void get(GyrometerData& data);

  virtual void get(FSRData& data);

  virtual void get(InertialSensorData& data);

  virtual void get(BumperData& data);

  virtual void get(IRReceiveData& data);

  virtual void get(CurrentCameraSettings& data);

  virtual void get(ButtonData& data);

  virtual void get(BatteryData& data) {data.charge = 1.0;}

  virtual void get(UltraSoundReceiveData& data) { data.data = 0.0; };

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& data);

  virtual void set(const CameraSettingsRequest& data);

  virtual void set(const LEDData& data);

  virtual void set(const IRSendData& data);

  virtual void set(const UltraSoundSendData& data);

  virtual void set(const SoundData& data);

protected:
  virtual void updateInertialSensor();

private:
  bool updateSensors();

  bool updateImage(const std::string& data);

  bool updateHingeJoint(const sexp_t* sexp);

  bool updateGyro(const sexp_t* sexp);

  bool updateAccelerometer(const sexp_t* sexp);

  bool updateFSR(const sexp_t* sexp);

  bool updateSee(const string& preName, const sexp_t* sexp);

  bool updateIMU(const sexp_t* sexp);

  Vector3d decomposeForce(double f, double fx, double fy, const Vector3d& c0, const Vector3d& c1, const Vector3d& c2);

  void calFSRForce(double f, double x, double y, FSRData::FSRID id0, FSRData::FSRID id1, FSRData::FSRID id2);

  void say();

  bool hear(const sexp_t* sexp);

  void beam();

  void autoBeam();

  void jointControl();

public:
  void motionLoop();

  void cognitionLoop();

  virtual void getMotionInput();

  virtual void setMotionOutput();

  virtual void getCognitionInput();

private:
  // members for threads
  pthread_mutex_t  theCognitionInputMutex;
  pthread_cond_t theCognitionInputCond;
  double maxJointAbsSpeed;

  //REPRESENTATION_PROVIDER(PlayerInfoInitializer, Cognition, PlayerInfo);
};

#endif	/* _SIMSPARKCONTROLLER_H */

