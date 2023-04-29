/*
* File:   DummySimulator.h
* Author: schlottb[at]informatik.hu-berlin.de
*
* Created on 2017.05.21
*/

#ifndef _DUMMY_SIMULATOR_H
#define _DUMMY_SIMULATOR_H

#include <iostream>
#include <strstream>

#include <Representations/Infrastructure/FrameInfo.h>

#include "DebugCommunication/DebugServer.h"
#include "PlatformInterface/PlatformInterface.h"
#include <ModuleFramework/ModuleManager.h>
#include <Tools/GameController/SPLGameController.h>
#include <Tools/Communication/Network/BroadCaster.h>
#include <Tools/Communication/Network/UDPReceiver.h>

// simple robot
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/TeamMessageData.h>

extern ModuleManager* getModuleManager(Cognition* c);

class DummySimulator : public naoth::PlatformInterface
{
public:
  DummySimulator(bool backendMode, unsigned short port);
  virtual ~DummySimulator();

  virtual std::string getBodyID() const { return "dummy-simulator"; }
  virtual std::string getBodyNickName() const { return "naoth"; }
  virtual std::string getHeadNickName() const { return "naoth"; }
  virtual std::string getRobotName() const { return "dummy-simulator"; }
  virtual std::string getPlatformName() const { return "DummySimulator"; }
  virtual unsigned int getBasicTimeStep() const { return 20; }

  void main();
  void enableGameController();
  void enableTeamComm(std::string interface = "wlan0");

  static const unsigned int frameExecutionTime = 33;

public: // platform getter and setter

  void get(naoth::FrameInfo& data) const { 
    data.setTime(data.getTime() + frameExecutionTime);
    data.setFrameNumber(data.getFrameNumber() + 1);

    if (!backendMode) {
      std::cout << "Frame: " << data.getFrameNumber() << "\t\r";
      std::cout.flush();
    }
  }

  void get(naoth::DebugMessageInCognition& data) {
    theDebugServer.getDebugMessageInCognition(data);
  }
  void get(naoth::DebugMessageInMotion& data) {
    theDebugServer.getDebugMessageInMotion(data);
  }

  void set(const naoth::DebugMessageOut& data) {
    if (data.answers.size() > 0) {
      theDebugServer.setDebugMessageOut(data);
    }
  }


public: // a dummy robot simulator
  naoth::SensorJointData robotJointData;
  naoth::FSRData fsrData;

  void set(const naoth::MotorJointData& data) {
    for(size_t i = 0; i < naoth::JointData::numOfJoint; ++i) {
      robotJointData.position[i] = data.position[i];
      robotJointData.stiffness[i] = data.stiffness[i];
      //robotJointData.temperature[i] = 40.0;
      //robotJointData.electricCurrent[i] = 0.0;
    }

    // HACK: emulate robot standing on the longer leg
    double leftPressure = 0.0;
    double rightPressure = 0.0;
    if(std::fabs(robotJointData.position[naoth::JointData::LKneePitch] - robotJointData.position[naoth::JointData::RKneePitch]) < 0.1) {
      leftPressure = 1.0;
      rightPressure = 1.0;
    } else if(robotJointData.position[naoth::JointData::LKneePitch] > robotJointData.position[naoth::JointData::RKneePitch]) {
      leftPressure = 0.0;
      rightPressure = 1.0;
    } else {
      leftPressure = 1.0;
      rightPressure = 0.0;
    }

    for(size_t i = 0; i < naoth::FSRData::numOfFSR; ++i) {
        fsrData.dataLeft[i] = leftPressure;
        fsrData.dataRight[i] = rightPressure;
      }
  }

  void get(naoth::SensorJointData& data) const { data = robotJointData; }
  void get(naoth::FSRData& data) const { data = fsrData; }

  void get(naoth::AccelerometerData& data) {
    data.data.z = Math::g;
    data.rawData.z = Math::g;
  }

  // teamcomm stuff
  void get(TeamMessageDataIn& data) { theTeamCommListener->receive(data.data); }
  void set(const TeamMessageDataOut& data) { theTeamCommSender->send(data.data); }

  // gamecontroller stuff
  void get(GameData& data){ theGameController->get(data); }
  void set(const GameReturnData& data) { theGameController->set(data); }

  /*
  void get(BatteryData& data);
  void get(FSRData& data);
  void get(AccelerometerData& data);
  void get(GyrometerData& data);
  void get(InertialSensorData& data);
  */


protected:
  virtual MessageQueue* createMessageQueue(const std::string& /*name*/) {
    // for single thread
    return new MessageQueue();
  }

public:
  // the flag for backend mode, which is used by LogfilePlayer of RobotControl
  bool backendMode;

  void printHelp();
  char getInput();
  void executeFrame();
  void play();
  bool doPlay;

private:
  DebugServer theDebugServer;
  naoth::FrameInfo theFrameInfo;
  SPLGameController* theGameController = nullptr;
  BroadCaster* theTeamCommSender = nullptr;
  UDPReceiver* theTeamCommListener = nullptr;
};

#endif  /* _DUMMY_SIMULATOR_H */


