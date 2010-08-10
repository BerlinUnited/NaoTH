/* 
 * File:   Simulator.h
 * Author: thomas
 *
 * Created on 4. Dezember 2008, 16:51
 */

#ifndef _SIMULATOR_H
#define	_SIMULATOR_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <set>

#include "Messages/representations.pb.h"

#include "PlatformInterface/PlatformInterface.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/BehaviorStatus.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Motion/MotionRequest/MotionRequest.h"
#include "Tools/DataStructures/Streamable.h"

#define CYCLE_TIME 20

class Simulator : public PlatformInterface
{
public:
  Simulator(const char* filePath, bool compatibleMode, bool backendMode);
  ~Simulator();

  virtual string getHardwareIdentity() const { return "logsimulator"; }

  virtual string getBodyID() { return "naoth-logsimulator"; }

  virtual string getBodyNickName() {return "naoth"; }

  void main();
  void printHelp();
  void printCurrentLineInfo();

  void stepForward();
  void stepBack();
  void jumpToBegin();
  void jumpToEnd();
  void jumpTo(unsigned int position);
  void play();
  void loop();

  virtual void getCognitionInput();

  virtual void getMotionInput();

  virtual bool registerCognitionInput(PlatformInterchangeable* data, const std::string& name);

  virtual bool registerMotionInput(PlatformInterchangeable* data, const std::string& name);

  virtual void setCognitionOutput();

  virtual void setMotionOutput();

  virtual bool registerCognitionOutput(const PlatformInterchangeable* data, const std::string& name);

  virtual bool registerMotionOutput(const PlatformInterchangeable* data, const std::string& name);

public:
  /////////////////////// get ///////////////////////
  virtual void get(unsigned int& /*timestamp*/){};

  virtual void get(FrameInfo& /*data*/){};

  virtual void get(SensorJointData& /*data*/){};

  virtual void get(AccelerometerData& /*data*/){};

  virtual void get(Image& /*data*/){};

  virtual void get(GyrometerData& /*data*/){};

  virtual void get(FSRData& /*data*/){};

  virtual void get(InertialSensorData& /*data*/){};

  virtual void get(BumperData& /*data*/){};

  virtual void get(IRReceiveData& /*data*/){};

  virtual void get(CurrentCameraSettings& /*data*/){};

  virtual void get(ButtonData& /*data*/){};

  virtual void get(BatteryData& /*data*/){};

  virtual void get(UltraSoundReceiveData& /*data*/) {};

  /////////////////////// set ///////////////////////
  virtual void set(const MotorJointData& /*data*/){};

  virtual void set(const CameraSettingsRequest& /*data*/){};

  virtual void set(const LEDData& /*data*/){};

  virtual void set(const IRSendData& /*data*/){};

  virtual void set(const UltraSoundSendData& /*data*/){};

  virtual void set(const SoundData& /*data*/){};

  /////////////////////// init ///////////////////////
  virtual void init();
  

private:
  bool noFrameInfo;
  unsigned int startFrameTime;

  CommunicationCollection* comm;
  std::ifstream logFile;

  std::list<unsigned int> frames;
  std::set<std::string> includedRepresentations;
  unsigned int maxFrame;
  unsigned int minFrame;

  list<unsigned int>::iterator currentFrame;
  unsigned int lastFrameTime;

  std::map<unsigned int, streampos> frameNumber2PosStart;
  std::map<unsigned int, streampos> frameNumber2PosEnd;
  std::map<std::string, Streamable*> logableCognitionRepresentations;
  std::map<std::string, Streamable*> logableMotionRepresentations;

//  REPRESENTATION_PROVIDER(PlayerInfoInitializer, Cognition, PlayerInfo);
//  REPRESENTATION_PROVIDER(FrameInfoStream, Cognition, FrameInfo);
  REPRESENTATION_PROVIDER(CameraMatrixStream, Cognition, CameraMatrix);
  REPRESENTATION_PROVIDER(BehaviorStatusStream, Cognition, BehaviorStatus);
  REPRESENTATION_PROVIDER(MotionRequestStream, Cognition, MotionRequest);
  REPRESENTATION_PROVIDER(BallPerceptStream, Cognition, BallPercept);
  REPRESENTATION_PROVIDER(LinePerceptStream, Cognition, LinePercept);
  REPRESENTATION_PROVIDER(GoalPerceptStream, Cognition, GoalPercept);
  
  char getInput();

  /**
   * reads in the frame which begins at the current frame position and executes
   * the "processes"
   */
  void executeCurrentFrame();

  bool compatibleExecute(const string& name, size_t dataSize);

  /** Initially parses the file */
  void parseFile();

  /** If true, do some corrections to the logfiles to be more compatible to old ones */
  bool compatibleMode;

  // the flag for backend mode, which is used by LogfilePlayer of RobotControl
  bool backendMode;
};

#endif	/* _SIMULATOR_H */


