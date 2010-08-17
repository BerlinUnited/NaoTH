/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _PLATFORMINTERFACE_H
#define	_PLATFORMINTERFACE_H

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GyrometerData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/InertialSensorData.h"
#include "Representations/Infrastructure/BumperData.h"
#include "Representations/Infrastructure/IRData.h"
#include "Representations/Infrastructure/CameraSettings.h"
#include "Representations/Infrastructure/LEDData.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/SoundData.h"
#include "Representations/Infrastructure/ButtonData.h"
#include "Representations/Infrastructure/BatteryData.h"

#include "Callable.h"
#include "naorunner/Tools/Debug/NaoTHAssert.h"

#include <list>
#include <string>
#include <iostream>

namespace naorunner
{
  using namespace std;

  // some interfaces
  struct AbstractAction
  {
    virtual void execute() = 0;
  };

  typedef std::list<AbstractAction*> ActionList;


  /*  the platform interface responses for 4 kinds of functionalities:
   * - get sensor data
   * - set action data
   * - initialize the cognition module and motion module
   * - main loop to call cognition and motion
   */
  class PlatformInterface
  {
  public:
    virtual ~PlatformInterface();

    //////// register own main loop callbacks /////////

    /**
     * Register callback objects for this platform interface that are regulary called
     * in the sense-think-act cycle.
     * @param motionCallback The callback object for the motion cycle or NULL if not active.
     * @param cognitionCallback The callback object for the cognition cycle or NULL if not active.
     */
    virtual void registerCallbacks(Callable* motionCallback, Callable* cognitionCallback);

    /////////////////////// get ///////////////////////
    virtual string getHardwareIdentity() const = 0;
    virtual string getBodyID() = 0;
    virtual string getBodyNickName() = 0;

    inline const string& getName() const { return platformName; }
    inline unsigned int getBasicTimeStep() const { return theBasicTimeStep; }


  protected:
    /////////////////////// get ///////////////////////
    virtual void get(AccelerometerData& data) = 0;
    virtual void get(FrameInfo& data) = 0;
    virtual void get(SensorJointData& data) = 0;
    virtual void get(Image& data) = 0;
    virtual void get(FSRData& data) = 0;
    virtual void get(GyrometerData& data) = 0;
    virtual void get(InertialSensorData& data) = 0;
    virtual void get(BumperData& data) = 0;
    virtual void get(IRReceiveData& data) = 0;
    virtual void get(CurrentCameraSettings& data) = 0;
    virtual void get(ButtonData& data) = 0;
    virtual void get(BatteryData& data) = 0;
    virtual void get(UltraSoundReceiveData& data) = 0;

    /////////////////////// set ///////////////////////
    virtual void set(const CameraSettingsRequest& data) = 0;
    virtual void set(const LEDData& data) = 0;
    virtual void set(const IRSendData& data) = 0;
    virtual void set(const UltraSoundSendData& data) = 0;
    virtual void set(const SoundData& data) = 0;
    virtual void set(const MotorJointData& data) = 0;

    //////////////////// GET/SET Actions /////////////////////
  private:
    template<class T>
    class RepresentationInputAction: public AbstractAction
    {
      PlatformInterface& platformInterface;
      T& representation;

    public:
      RepresentationInputAction(PlatformInterface& platformInterface, T& representation)
        : platformInterface(platformInterface),
          representation(representation)
      {}

      virtual void execute()
      {
        platformInterface.get(representation);
      }
    };//end RepresentationInputAction

    template<class T>
    class RepresentationOutputAction: public AbstractAction
    {
      PlatformInterface& platformInterface;
      T& representation;

    public:
      RepresentationOutputAction(PlatformInterface& platformInterface, T& representation)
        : platformInterface(platformInterface),
          representation(representation)
      {}

      virtual void execute()
      {
        platformInterface.set(representation);
      }
    };//end RepresentationInputAction

  public:
    template<class T>
    void registerCognitionInput(T& data, const std::string& name)
    {
      cout << platformName << " register Cognition input: " << name << endl;
      AbstractAction* action = new RepresentationInputAction<T>(*this, data);
      cognitionInput.push_back(action);
    }//end registerCognitionInput

    template<class T>
    void registerCognitionOutput(const T& data, const std::string& name)
    {
      cout << platformName << " register Cognition output: " << name;
      AbstractAction* action = new RepresentationOutputAction<const T>(*this, data);
      cognitionOutput.push_back(action);
    }//end registerCognitionOutput

    template<class T>
    void registerMotionInput(T& data, const std::string& name)
    {
      cout << platformName << " register Motion input: " << name << endl;
      AbstractAction* action = new RepresentationInputAction<T>(*this, data);
      motionInput.push_back(action);
    }//end registerCognitionInput

    template<class T>
    void registerMotionOutput(const T& data, const std::string& name)
    {
      cout << platformName << " register Motion output: " << name;
      AbstractAction* action = new RepresentationOutputAction<const T>(*this, data);
      motionOutput.push_back(action);
    }//end registerCognitionOutput


    template<class T> 
    void get(T& data /* data */)
    {
      THROW("try go get a not supported representation");
    }

    template<class T>
    void set(const T& /* data */)
    {
      THROW("try go set a not supported representation");
    }

  protected:
    PlatformInterface(const std::string& name, unsigned int basicTimeStep);

    void callCognition();
    virtual void getCognitionInput();
    virtual void setCognitionOutput();

    void callMotion();
    virtual void getMotionInput();
    virtual void setMotionOutput();


  private:
    std::string platformName;
    unsigned int theBasicTimeStep;

    Callable* motionCallback;
    ActionList motionInput;
    ActionList motionOutput;

    Callable* cognitionCallback;
    ActionList cognitionInput;
    ActionList cognitionOutput;

    inline void execute(ActionList& actionList) const
    {
      for(ActionList::iterator iter = actionList.begin(); iter != actionList.end(); iter++)
      {
        (*iter)->execute();
      }//end for
    }//end execute

  };
}

#endif	/* _PLATFORMINTERFACE_H */

