/**
 * @file NaoController.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief Interface for the real robot for both cognition and motion
 *
 */

#ifndef _NaoController_H_
#define _NaoController_H_

#include <string>
#include <fstream>
#include <iostream>

//
#include "PlatformInterface/PlatformInterface.h"
#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"
//#include "Tools/Debug/Stopwatch.h"

//
#include "V4lCameraHandler.h"
#include "SoundControl.h"
#include "SPLGameController.h"
#include "CPUTemperatureReader.h"
#include "DebugCommunication/DebugServer.h"

#include "Tools/Communication/Network/BroadCaster.h"
#include "Tools/Communication/Network/UDPReceiver.h"

// representations
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/TeamMessageData.h"
#include "Representations/Infrastructure/RemoteMessageData.h"
#include "Representations/Infrastructure/GameData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/WhistlePercept.h"
#include "Representations/Infrastructure/WhistleControl.h"
#include "Representations/Infrastructure/GPSData.h"

#include <sstream>
#include <algorithm>

// local tools
#include "Tools/IPCData.h"
#include "Tools/NaoTime.h"
#include "Tools/SharedMemoryIO.h"

namespace naoth
{

class NaoController : public PlatformInterface
{
public:
  NaoController();
  virtual ~NaoController();

  virtual string getBodyID() const { return theBodyID; }
  virtual string getBodyNickName() const { return theBodyNickName; }
  virtual string getHeadNickName() const { return theHeadNickName; }
  virtual string getRobotName() const { return theRobotName; }

  // camera stuff
  void get(Image& data){ theBottomCameraHandler.get(data); } // blocking
  void get(ImageTop& data){ theTopCameraHandler.get(data); } // non blocking
  void get(CurrentCameraSettings& data) { theBottomCameraHandler.getCameraSettings(data); }
  void get(CurrentCameraSettingsTop& data) { theTopCameraHandler.getCameraSettings(data); }
  void set(const CameraSettingsRequest& data);
  void set(const CameraSettingsRequestTop& data);

  // sound
  void set(const SoundPlayData& data)
  {
    theSoundHandler->setSoundData(data);
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { theTeamCommListener->receive(data.data); }
  void set(const TeamMessageDataOut& data) { theTeamCommSender->send(data.data); }

  void get(RemoteMessageDataIn& data) { theRemoteCommandListener->receive(data.data); }

  // gamecontroller stuff
  void get(GameData& data){ theGameController->get(data); }
  void set(const GameReturnData& data) { theGameController->set(data); }

  // debug comm
  void get(DebugMessageInCognition& data) { theDebugServer->getDebugMessageInCognition(data); }
  void get(DebugMessageInMotion& data) { theDebugServer->getDebugMessageInMotion(data); }
  void set(const DebugMessageOut& data) { theDebugServer->setDebugMessageOut(data); }

  // time
  void get(FrameInfo& data)
  {
    //TODO: use naoSensorData.data().timeStamp
    data.setTime(NaoTime::getNaoTimeInMilliSeconds());
    data.setFrameNumber(data.getFrameNumber()+1);
  }


  // read directly from the shared memory
  void get(SensorJointData& data) { naoSensorData.get(data); }
  void get(AccelerometerData& data) { naoSensorData.get(data); }
  void get(GyrometerData& data) { naoSensorData.get(data); }
  void get(FSRData& data) { naoSensorData.get(data); }
  void get(InertialSensorData& data) { naoSensorData.get(data); }
  void get(IRReceiveData& data) { naoSensorData.get(data); }
  void get(ButtonData& data) { naoSensorData.get(data); }
  void get(BatteryData& data) { naoSensorData.get(data); }
  void get(UltraSoundReceiveData& data) { naoSensorData.get(data); }
  void get(WhistlePercept& data) {data.counter = whistleSensorData.data(); }
  void get(CpuData& data) { theCPUTemperatureReader.get(data); }

  std::vector<std::string> gpsdata;
  static const int NAT_FRAMEOFDATA = 7;

  void get(GPSData& data) 
  { 
    gpsdata.clear();
    theGPSListener->receive(gpsdata);
    //std::cout << gpsdata.size() << std::endl;

    for (const std::string& s : gpsdata) {
      //std::cout << s.size() << std::endl;
      stringstream ss(s);

      unsigned short messageID(0);
      ss.read((char*)&messageID, 2);

      unsigned short packetSize(0);
      ss.read((char*)&packetSize, 2);

      if (messageID == NAT_FRAMEOFDATA) {
        unsigned int frameNumber = 0;
        ss.read((char*)&frameNumber, 4);

        unsigned int markerSetCount = 0;
        ss.read((char*)&markerSetCount, 4);
        
        //ASSERT(markerSetCount == 0); // not supported

        for (unsigned int i = 0; i < markerSetCount; ++i)
        {
          // read the name of the representation
          std::string modelName;
          char c = '\0';
          ss.read(&c, 1);
          while (c != '\0') {
            modelName += c;
            ss.read(&c, 1);
          }
          
          unsigned int markerCount = 0;
          ss.read((char*)&markerCount, 4);

          for (unsigned int i = 0; i < markerCount; ++i) {
            Vector3f pos;
            ss.read((char*)&pos.x, 4);
            ss.read((char*)&pos.y, 4);
            ss.read((char*)&pos.z, 4);
          }
        }

        unsigned int unlabeledMarkersCount = 0;
        ss.read((char*)&unlabeledMarkersCount, 4);
        
        for (unsigned int i = 0; i < unlabeledMarkersCount; ++i) {
          Vector3f pos;
          ss.read((char*)&pos.x, 4);
          ss.read((char*)&pos.y, 4);
          ss.read((char*)&pos.z, 4);
        }

        unsigned int rigidBodyCount = 0;
        ss.read((char*)&rigidBodyCount, 4);

        for (unsigned int i = 0; i < rigidBodyCount; ++i) {
          unsigned int id = 0;
          ss.read((char*)&id, 4);

          Vector3f pos;
          ss.read((char*)&pos.x, 4);
          ss.read((char*)&pos.y, 4);
          ss.read((char*)&pos.z, 4);

          Vector3f new_pos;
          new_pos.x = -pos.z;
          new_pos.y = -pos.x;
          new_pos.z =  pos.y;

          data.data.translation.x = new_pos.x;
          data.data.translation.y = new_pos.y;
          data.data.translation.z = new_pos.z;
          data.data.translation *= 1000.0;

          //std::cout << data.data.translation.x << " " << data.data.translation.y << " " << data.data.translation.z << id << std::endl;


          // rotation
          float qx, qy, qz, qw;
          ss.read((char*)&qx, 4);
          ss.read((char*)&qy, 4);
          ss.read((char*)&qz, 4);
          ss.read((char*)&qw, 4);

          if (qx == 0) {
            continue;
          }

          double ysqr = qy * qy;


          // roll(x - axis rotation)
          double t0 = +2.0 * (qw * qx + qy * qz);
          double t1 = +1.0 - 2.0 * (qx * qx + ysqr);
          double roll = atan2(t0, t1);

          //pitch(y - axis rotation)
          double t2 = +2.0 * (qw * qy - qz * qx);
          t2 = t2 > 1.0f ? 1.0 : t2;
          t2 = t2 < -1.0f ? -1.0 : t2;
          double pitch = asin(t2); // HACK: constant offset

          //yaw(z - axis rotation)
          double t3 = +2.0 * (qw * qz + qx * qy);
          double t4 = +1.0 - 2.0 * (ysqr + qz * qz);
          double yaw = atan2(t3, t4);

          
          data.data.rotation = RotationMatrix(yaw, pitch, roll); //RotationMatrix::getRotationZ(pitch + Math::fromDegrees(25.0));
          
          yaw = data.data.rotation.getZAngle();
          pitch = data.data.rotation.getYAngle();// +Math::fromDegrees(25.0);
          roll = data.data.rotation.getXAngle();

          //data.data.rotation = RotationMatrix::getRotationZ(-pitch + Math::fromDegrees(25.0));
          
          Pose2D pose(-data.data.rotation.getYAngle() + Math::fromDegrees(27.0), data.data.translation.x, data.data.translation.y);
          pose.translate(150.0, 0.0);
          data.data = Pose3D::embedXY(pose);

          //std::cout << Math::toDegrees(yaw) << " " << Math::toDegrees(pitch) << " " << Math::toDegrees(roll) << id << std::endl;
        }
      }
    }

    // process message
    //if (gpsdata.size() > 0) {
      //stringstream ss;
      //std::copy(gpsdata.begin(), gpsdata.end());
      //unsigned char messageID;


    //}
  }

  // write directly to the shared memory
  // ACHTUNG: each set calls swapWriting()
  void set(const MotorJointData& data) { naoCommandMotorJointData.set(data); }
  void set(const LEDData& data) { naoCommandLEDData.set(data); }
  void set(const IRSendData& data) { naoCommandIRSendData.set(data); }
  void set(const UltraSoundSendData& data) { naoCommandUltraSoundSendData.set(data); }
  void set(const WhistleControl& data) { whistleControlData.set(data.onOffSwitch); }


  virtual void getMotionInput()
  {
    //STOPWATCH_START("getMotionInput");
    // try to get some data from the DCM
    if ( !naoSensorData.swapReading() )
    {
      std::cerr << "[NaoController] didn't get new sensor data" << std::endl;
    }
    PlatformInterface::getMotionInput();
    //STOPWATCH_STOP("getMotionInput");
  }


  virtual void setMotionOutput()
  {
    //STOPWATCH_START("setMotionOutput");
    PlatformInterface::setMotionOutput();
    //STOPWATCH_STOP("setMotionOutput");
  }


  virtual void getCognitionInput()
  {
    //STOPWATCH_START("getCognitionInput");
    PlatformInterface::getCognitionInput();
    //STOPWATCH_STOP("getCognitionInput");
  }


  virtual void setCognitionOutput()
  {
    //STOPWATCH_START("setCognitionOutput");
    PlatformInterface::setCognitionOutput();
    //STOPWATCH_STOP("setCognitionOutput");
  }


protected:
  virtual MessageQueue* createMessageQueue(const std::string& /*name*/)
  {
    return new MessageQueue4Threads();
  }


protected:
  std::string theBodyID;
  std::string theBodyNickName;
  std::string theHeadNickName;
  std::string theRobotName;

  // -- begin -- shared memory access --
  // DCM --> NaoController
  SharedMemoryReader<NaoSensorData> naoSensorData;

  // NaoController --> DCM
  SharedMemoryWriter<Accessor<MotorJointData> > naoCommandMotorJointData;
  SharedMemoryWriter<Accessor<UltraSoundSendData> > naoCommandUltraSoundSendData;
  SharedMemoryWriter<Accessor<IRSendData> > naoCommandIRSendData;
  SharedMemoryWriter<Accessor<LEDData> > naoCommandLEDData;

  // WhistleDetector --> NaoController
  SharedMemoryReader<int> whistleSensorData;
  SharedMemoryWriter<Accessor<int> > whistleControlData;

  // -- end -- shared memory access --

  //
  V4lCameraHandler theBottomCameraHandler;
  V4lCameraHandler theTopCameraHandler;
  SoundControl *theSoundHandler;
  BroadCaster* theTeamCommSender;
  UDPReceiver* theTeamCommListener;
  UDPReceiver* theRemoteCommandListener;
  UDPReceiver* theGPSListener;
  SPLGameController* theGameController;
  DebugServer* theDebugServer;
  CPUTemperatureReader theCPUTemperatureReader;
};

} // end namespace naoth

#endif // _NAO_CONTROLLER_BASE_H_
