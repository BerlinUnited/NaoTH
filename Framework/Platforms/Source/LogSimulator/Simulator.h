/* 
 * File:   Simulator.h
 * Author: thomas
 *
 * Created on 4. Dezember 2008, 16:51
 */

#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <set>

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

//

#include "DebugCommunication/DebugServer.h"
#include "PlatformInterface/PlatformInterface.h"
#include "Tools/DataStructures/Streamable.h"
#include <Tools/Debug/DebugRequest.h>
#include <ModuleFramework/Module.h>

#define CYCLE_TIME 20

class LogProvider: public Module, virtual private BlackBoardInterface
{
private:
  std::map<std::string, std::string>* representations;
  std::map<std::string, std::string> exludeMap;

public:
  LogProvider() : Module("LogProvider"), representations(NULL) 
  {
    //HACK: do not provide basic percepts (they are provided by get(...)
    exludeMap["AccelerometerData"] = "";
    exludeMap["SensorJointData"] = "";
    exludeMap["Image"] = "";
    exludeMap["FSRData"] = "";
    exludeMap["GyrometerData"] = "";
    exludeMap["InertialSensorData"] = "";
    exludeMap["IRReceiveData"] = "";
    exludeMap["CurrentCameraSettings"] = "";
    exludeMap["ButtonData"] = "";
    exludeMap["BatteryData"] = "";
    exludeMap["UltraSoundReceiveData"] = "";
    exludeMap["FrameInfo"] = "";
  }

  

  void init(std::map<std::string, std::string>& rep, std::set<std::string>& includedRepresentations)
  {
    representations = &rep;
    std::set<std::string>::iterator iter;

    for(iter = includedRepresentations.begin(); iter != includedRepresentations.end(); ++iter)
    {
      if(*iter != "")
      {
        DEBUG_REQUEST_REGISTER("LogProvider:"+(*iter), "", true);
      }
    }//end for

    //DEBUG_REQUEST_REGISTER("FrameInfo", "", false);
  }//end init


  void execute()
  {
    BlackBoard& blackBoard = BlackBoardInterface::getBlackBoard();
    BlackBoard::Registry::iterator iter;

    for(iter = blackBoard.getRegistry().begin(); iter != blackBoard.getRegistry().end(); ++iter)
    {
      Representation& theRepresentation = iter->second->getRepresentation();
      const std::string& name = iter->first;
      
      // look if there is a logged data for this representation
      std::map<std::string, std::string>::const_iterator iter = representations->find(name); 
      if(iter != representations->end() && exludeMap.find(iter->first) == exludeMap.end() && iter->first != "")
      {
        DEBUG_REQUEST_GENERIC("LogProvider:"+(iter->first),
          std::stringstream stream(iter->second);
          theRepresentation.deserialize(stream);
          );
      }//end if
    }//end for
  }//end execute
};


class Simulator : public PlatformInterface
{
public:
  Simulator(const char* filePath, bool compatibleMode, bool backendMode);
  virtual ~Simulator();

  virtual std::string getBodyID() const { return "naoth-logsimulator"; }

  virtual std::string getBodyNickName() const {return "naoth"; }

  virtual std::string getHeadNickName() const {return "naoth"; }

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

  template<class T> void generalGet(T& data, std::string name) const;

  /////////////////////// get ///////////////////////
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
  virtual void init();
  
  std::map<std::string, std::string>& getRepresentations()
  {
    return representations;
  }

  std::set<std::string>& getIncludedRepresentations()
  {
    return includedRepresentations;
  }

protected:
  virtual MessageQueue* createMessageQueue(const std::string& name);

private:

  bool noFrameInfo;
  unsigned int startFrameTime;

  std::ifstream logFile;

  std::list<unsigned int> frames;

  // list of representation names included in the logfile
  std::set<std::string> includedRepresentations;
  unsigned int maxFrame;
  unsigned int minFrame;
  
  //
  std::map<std::string, std::string> representations;

  std::list<unsigned int>::iterator currentFrame;
  unsigned int lastFrameTime;

  std::map<unsigned int, std::streampos> frameNumber2PosStart;
  std::map<unsigned int, std::streampos> frameNumber2PosEnd;
  
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

  bool compatibleExecute(const std::string& name, size_t dataSize);

  /** Initially parses the file */
  void parseFile();

  /** If true, do some corrections to the logfiles to be more compatible to old ones */
  bool compatibleMode;

  // the flag for backend mode, which is used by LogfilePlayer of RobotControl
  bool backendMode;


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

#endif  /* _SIMULATOR_H */


