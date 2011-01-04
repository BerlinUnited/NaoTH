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

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/BumperData.h>
#include <Representations/Infrastructure/IRData.h>
#include <Representations/Infrastructure/CameraSettings.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/FSRData.h>
#include <Representations/Infrastructure/BatteryData.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/LEDData.h>
#include <Representations/Infrastructure/SoundData.h>

#include "PlatformInterface/PlatformInterface.h"
#include "Tools/DataStructures/Streamable.h"

#define CYCLE_TIME 20

namespace naoth
{
  class Simulator : public PlatformInterface<Simulator>
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

//    virtual void getCognitionInput();
//    virtual void getMotionInput();
//    virtual bool registerCognitionInput(PlatformInterchangeable* data, const std::string& name);
//    virtual bool registerMotionInput(PlatformInterchangeable* data, const std::string& name);
//    virtual void setCognitionOutput();
//    virtual void setMotionOutput();
//    virtual bool registerCognitionOutput(const PlatformInterchangeable* data, const std::string& name);
//    virtual bool registerMotionOutput(const PlatformInterchangeable* data, const std::string& name);

    template<class T> void generalGet(T& data, std::string name);

    /////////////////////// get ///////////////////////
    #define SIM_GET(rep) virtual void get(rep& data) {generalGet(data,#rep);}
    virtual void get(unsigned int& /*timestamp*/){};

    SIM_GET(FrameInfo);
    SIM_GET(SensorJointData);
    SIM_GET(AccelerometerData);
    SIM_GET(Image);
    SIM_GET(GyrometerData);
    SIM_GET(FSRData);
    SIM_GET(InertialSensorData);
    SIM_GET(BumperData);
    SIM_GET(IRReceiveData);
    SIM_GET(CurrentCameraSettings);
    SIM_GET(ButtonData);
    SIM_GET(BatteryData);
    SIM_GET(UltraSoundReceiveData);

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

    std::ifstream logFile;

    std::list<unsigned int> frames;
    std::set<std::string> includedRepresentations;
    unsigned int maxFrame;
    unsigned int minFrame;

    std::map<std::string, std::string> representations;

    list<unsigned int>::iterator currentFrame;
    unsigned int lastFrameTime;

    std::map<unsigned int, streampos> frameNumber2PosStart;
    std::map<unsigned int, streampos> frameNumber2PosEnd;
    
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

    bool compatibleExecute(const string& name, size_t dataSize);

    /** Initially parses the file */
    void parseFile();

    /** If true, do some corrections to the logfiles to be more compatible to old ones */
    bool compatibleMode;

    // the flag for backend mode, which is used by LogfilePlayer of RobotControl
    bool backendMode;
  };
}
#endif	/* _SIMULATOR_H */


