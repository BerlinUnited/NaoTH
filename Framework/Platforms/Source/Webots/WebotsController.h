/**
 * @file WebotsController.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
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
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/GPSData.h>
#include <Representations/Infrastructure/Image.h>
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
#include <Tools/DataStructures/RingBufferWithSum.h>

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
  
  // extend the lola SensorData with additional Webots speceific sensors
  struct WebotsSensorData 
  {
    // NAO V6 like sensor data
    SensorData lolaSensors;

    struct {
      std::array<float,3> Position;
      std::array<float,9> Rotation;
      MSGPACK_DEFINE_MAP(Position, Rotation);
    } GPS;

    double Time;

    struct {
      bool Seen;
      std::array<float,3> Position;
      std::string Cam;
      MSGPACK_DEFINE_MAP(Seen, Position, Cam);
    } Ball;

    MSGPACK_DEFINE_MAP(lolaSensors,GPS,Time,Ball);
  } webotsSensors;

  // for easier access
  SensorData& lolaSensors = webotsSensors.lolaSensors;
  
  //
  std::string receiveBuffer;

  // HACK:
  MotorJointData theLastMotorJointData;
  // (Experimental) delay sensor jooint data to simulate the real robot
  RingBuffer<SensorJointData,2> sensorJointDataBuffer;

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
  void get(VirtualVision& data) {
    data.clear();
    if (webotsSensors.Ball.Seen && webotsSensors.Ball.Cam == "bottom") 
    {
      // ball in naoth coordinates (in mm)
      Vector3d ball(-webotsSensors.Ball.Position[2]*1000.0, -webotsSensors.Ball.Position[0]*1000.0, webotsSensors.Ball.Position[1]*1000.0);
      // virtual vision processor expects spheric coordinates
      data.data["B"] = { ball.abs(), atan2(ball.y, ball.x), atan2(ball.z, Vector2d(ball.x, ball.y).abs()) };
    }
  }
  void get(VirtualVisionTop& data) {
    data.clear();
    if (webotsSensors.Ball.Seen && webotsSensors.Ball.Cam == "top") 
    {
      // ball in naoth coordinates (in mm)
      Vector3d ball(-webotsSensors.Ball.Position[2]*1000.0, -webotsSensors.Ball.Position[0]*1000.0, webotsSensors.Ball.Position[1]*1000.0);
      // virtual vision processor expects spheric coordinates
      data.data["B"] = { ball.abs(), atan2(ball.y, ball.x), atan2(ball.z, Vector2d(ball.x, ball.y).abs()) };
    }
  }
  
  void get(GPSData& data) {
    data.data.translation.x =  webotsSensors.GPS.Position[0]; //  x
    data.data.translation.y = -webotsSensors.GPS.Position[2]; // -z
    data.data.translation.z =  webotsSensors.GPS.Position[1]; //  y

    // from m to mm
    data.data.translation *= 1000;

    // note: data.data.rotation[col][row]
    data.data.rotation[0][0] = webotsSensors.GPS.Rotation[0];
    data.data.rotation[1][0] = webotsSensors.GPS.Rotation[1];
    data.data.rotation[2][0] = webotsSensors.GPS.Rotation[2];

    data.data.rotation[0][1] = webotsSensors.GPS.Rotation[3];
    data.data.rotation[1][1] = webotsSensors.GPS.Rotation[4];
    data.data.rotation[2][1] = webotsSensors.GPS.Rotation[5];

    data.data.rotation[0][2] = webotsSensors.GPS.Rotation[6];
    data.data.rotation[1][2] = webotsSensors.GPS.Rotation[7];
    data.data.rotation[2][2] = webotsSensors.GPS.Rotation[8];

    static Matrix3d t = {
      {1,  0,  0},
      {0,  0, -1},
      {0, -1,  0}
    };

    data.data.rotation = t*data.data.rotation*t.transpose();
    //data.data.rotation.rotateX(-Math::pi_2);
  }

  void get(TeamMessageDataIn& data){}
  void get(GameData& data){}

  /////////////////////// set ///////////////////////
  void set(const MotorJointData& data);
  void set(const TeamMessageDataOut& data) { }

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name);
  
private:
  bool connect(const std::string& host, int port);
  bool getSensorData();
  void sendCommands();

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
    if(!data.answers.empty()) {
      theDebugServer.setDebugMessageOut(data);
    }
  }
};

#endif  /* WEBOTS_CONTROLLER_H */
