/**
 * @file WebotsController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief Interface for the Webots simulator
 *
 */

#ifndef WEBOTS_CONTROLLER_H
#define WEBOTS_CONTROLLER_H

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/GPSData.h>
#include <Representations/Infrastructure/VirtualVision.h>
#include <Representations/Infrastructure/TeamMessageData.h>
#include <Representations/Infrastructure/GameData.h>

#include <DCM/Tools/LolaData.h>
#include <DCM/Tools/LolaDataConverter.h>

// SPL team comm
#include "MessagesSPL/SPLStandardMessage.h"

#include "PlatformInterface/PlatformInterface.h"
#include "DebugCommunication/DebugServer.h"

// communication with the server
#include <Tools/Communication/SocketStream/SocketStream.h>
#include <msgpack.hpp>

// tools
#include <map>
#include <set>
#include <mutex>
#include <condition_variable>

using namespace naoth;

class WebotsController : public PlatformInterface
{
private:
  std::string thePlatformName;

  GSocket* socket;
  PrefixedSocketStream theSocket;

  bool theSyncMode;
  double theSenseTime;
  double theStepTime; // the time of last step in seconds
  int motionCount = 0;
  
public:
  WebotsController(const std::string& name);

  virtual ~WebotsController();

  // basic platform info
  virtual std::string getBodyID()         const { return "none"; };
  virtual std::string getRobotName()      const { return"webots_nao"; }
  virtual std::string getBodyNickName()   const { return getRobotName(); }
  virtual std::string getHeadNickName()   const { return getRobotName(); }
  virtual std::string getPlatformName()   const { return thePlatformName; }
  virtual unsigned int getBasicTimeStep() const { return 10; }

  /////////////////////// init ///////////////////////
  bool init(const std::string& modelPath, const std::string& teamName, unsigned int teamNumber, unsigned int num, const std::string& server, unsigned int port, bool sync);

  void main();

private: // internal data
  ActuatorData lolaActuators;
  
  SensorData lolaSensors;

  // HACK:
  MotorJointData theLastMotorJointData;

  struct {
    std::string model;
    MSGPACK_DEFINE_MAP(model);
  } header;

public:
  /////////////////////// get ///////////////////////
  void get(FrameInfo& data);

  void get(SensorJointData& data);

  void get(AccelerometerData& data)
  { 
    data.data.x = lolaSensors.Accelerometer.x;
    data.data.y = -lolaSensors.Accelerometer.y;
    data.data.z = -lolaSensors.Accelerometer.z;

    data.rawData = data.data;
  }
  
  void get(GyrometerData& data) {
    data.data.x = lolaSensors.Gyroscope.x;
    data.data.y = lolaSensors.Gyroscope.y;
    data.data.z = lolaSensors.Gyroscope.z;

    data.rawData = data.data;
  }

  void get(InertialSensorData& data) {
    data.data.x = lolaSensors.Angles.x;
    data.data.y = -lolaSensors.Angles.y;
  }

  void get(FSRData& data) 
  {
    data.dataLeft = {
      lolaSensors.FSR.LFoot.FrontLeft,
      lolaSensors.FSR.LFoot.FrontRight,
      lolaSensors.FSR.LFoot.RearLeft,
      lolaSensors.FSR.LFoot.RearRight
    };
    data.dataRight = {
      lolaSensors.FSR.RFoot.FrontLeft,
      lolaSensors.FSR.RFoot.FrontRight,
      lolaSensors.FSR.RFoot.RearLeft,
      lolaSensors.FSR.RFoot.RearRight
    };
  }
  
  void get(BatteryData& data){}
  
  void get(Image& data) {}
  void get(VirtualVision& data){}
  void get(VirtualVisionTop& data){}
  void get(GPSData& data){}

  void get(TeamMessageDataIn& data);
  void get(GameData& data){}

  /////////////////////// set ///////////////////////
  void set(const MotorJointData& data);
  void set(const TeamMessageDataOut& data);

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name);
  
private:
  bool connect(const std::string& host, int port);
  bool getSensorData(std::string& data);

  // the main loop in single thread
  void singleThreadMain();

  // the main loop in threads
  void multiThreadsMain();

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
    if(data.answers.size() > 0) {
      theDebugServer.setDebugMessageOut(data);
    }
  }
};

#endif  /* WEBOTS_CONTROLLER_H */

