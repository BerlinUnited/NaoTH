/* 
 * File:   Simulator.h
 * Author: thomas
 *
 * Created on 4. Dezember 2008, 16:51
 */

#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include <iostream>
#include <strstream>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/SoundData.h>
#include <Representations/Infrastructure/GameData.h>

#include "DebugCommunication/DebugServer.h"
#include "PlatformInterface/PlatformInterface.h"

#include <Tools/Debug/DebugRequest.h>

//in runtime as constant defined width and heigth of the input image
#include "Representations/Infrastructure/CameraInfoConstants.h"

#include "LogFileScanner.h"
#include "LogProviderModule.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#define CYCLE_TIME 20

class Simulator : public PlatformInterface
{
public:
  Simulator(const std::string& filePath, bool backendMode, bool realTime);
  virtual ~Simulator(){}

  virtual std::string getBodyID() const { return "naoth-logsimulator"; }
  virtual std::string getBodyNickName() const {return "naoth"; }
  virtual std::string getHeadNickName() const {return "naoth"; }

  void main();

  void printRepresentations();
  void printHelp();
  void printCurrentLineInfo();

  void stepForward();
  void stepBack();
  void jumpToBegin();
  void jumpToEnd();
  void jumpTo(unsigned int position);
  void play(bool loop = false);

  int getCurrentFrame() { return *currentFrame; }
  int getMinFrame() { return *(logFileScanner.begin());}
  int getMaxFrame() { return *(logFileScanner.last());}
  
  /////////////////////// get ///////////////////////
  template<class T> void generalGet(T& data, std::string name) const;

  #define SIM_GET(rep) void get(rep& data) const {generalGet(data,#rep);}
  
  // generic get
  template<class T> void get(T& data) const { generalGet(data, typeid(T).name()); }
  
  // do nothing
  template<class T> void set(const T& /*data*/){}

  //SIM_GET(FrameInfo);
  void get(unsigned int& /*timestamp*/) const {}

  
  SIM_GET(FrameInfo);
  SIM_GET(SensorJointData);
  SIM_GET(AccelerometerData);
  SIM_GET(Image);
  SIM_GET(ImageTop);
  SIM_GET(GyrometerData);
  SIM_GET(FSRData);
  SIM_GET(InertialSensorData);
  SIM_GET(IRReceiveData);
  SIM_GET(CurrentCameraSettings);
  SIM_GET(ButtonData);
  SIM_GET(BatteryData);
  SIM_GET(UltraSoundReceiveData);
  

  /////////////////////// set ///////////////////////
  //virtual void set(const MotorJointData& /*data*/){};
  //virtual void set(const CameraSettingsRequest& /*data*/){};
  //virtual void set(const LEDData& /*data*/){};
  //virtual void set(const IRSendData& /*data*/){};
  //virtual void set(const UltraSoundSendData& /*data*/){};
  //virtual void set(const SoundData& /*data*/){};

  /////////////////////// init ///////////////////////
  void open(const std::string& filePath);
  virtual void init();
  
  const LogFileScanner::Frame& getRepresentations() {
    return representations;
  }

  const std::set<std::string>& getIncludedRepresentations() {
    return logFileScanner.getIncludedRepresentations();
  }

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name);


public:
  // the flag for backend mode, which is used by LogfilePlayer of RobotControl
  bool backendMode;
  // play the logfie according to the time of the frames
  bool realTime;

  LogFileScanner logFileScanner;
  LogFileScanner::FrameIterator currentFrame;
  LogFileScanner::Frame representations;

  unsigned int lastFrameTime;
  // the simulated time and the framenumber contineously increases even if the logfile is played backwards (!)
  unsigned int simulatedTime;
  unsigned int simulatedFrameNumber;

  char getInput();

  /**
   * reads in the frame which begins at the current frame position and executes
   * the "processes"
   */
  void executeCurrentFrame();

  /** 
   * if the frame info is not available: create new one
   * the time grows monotonously
   */
  void adjust_frame_time();

private:
  DebugServer theDebugServer;

public:
  void get(DebugMessageIn& data) {
    theDebugServer.getDebugMessageIn(data);
  }

  void set(const DebugMessageOut& data) {
    if(data.answers.size() > 0) {
      theDebugServer.setDebugMessageOut(data);
    }
  }
};

#endif  /* _SIMULATOR_H */


