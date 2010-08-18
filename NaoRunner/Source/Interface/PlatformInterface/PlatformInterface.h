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
#include "Interface/Tools/Debug/NaoTHAssert.h"

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


  /* 
   * the PlatformBase holds (and provides access to) 
   * some basic information about the platform
   */
  class PlatformBase
  {
  public:
    PlatformBase(const std::string& platformName, unsigned int basicTimeStep)
      :
      platformName(platformName),
      theBasicTimeStep(basicTimeStep)
    {}

    virtual ~PlatformBase() {}

    /////////////////////// get ///////////////////////
    virtual string getHardwareIdentity() const = 0;
    virtual string getBodyID() = 0;
    virtual string getBodyNickName() = 0;

    inline const string& getName() const { return platformName; }
    inline unsigned int getBasicTimeStep() const { return theBasicTimeStep; }

  private:
    std::string platformName;
    unsigned int theBasicTimeStep;
  };//end class PlatformBase



  /*  the platform interface responses for 4 kinds of functionalities:
   * - get sensor data
   * - set action data
   * - initialize the cognition module and motion module
   * - main loop to call cognition and motion
   */
  class PlatformInterface: public PlatformBase
  {
  public:
    virtual ~PlatformInterface();


    //template<class T> 
    //void get(T& data /* data */)
    //{
    //  THROW("try go get a not supported representation");
    //}

    //template<class T>
    //void set(const T& /* data */)
    //{
    //  THROW("try go set a not supported representation");
    //}

  protected:
    PlatformInterface(const std::string& name, unsigned int basicTimeStep);

    //void callCognition();
    virtual void getCognitionInput();
    virtual void setCognitionOutput();

    //void callMotion();
    virtual void getMotionInput();
    virtual void setMotionOutput();


  protected:
    ActionList motionInput;
    ActionList motionOutput;

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


  template<class PlatformType>
  class AbstractPlatform: public PlatformInterface
  {
  public:
    AbstractPlatform(const std::string& name, unsigned int basicTimeStep)
      : PlatformInterface(name, basicTimeStep),
        motionCallback(NULL),
        cognitionCallback(NULL)
    {}

    virtual ~AbstractPlatform(){};
      //////////////////// GET/SET Actions /////////////////////
  private:

    virtual PlatformType& getPlatform() = 0;

    template<class T>
    class RepresentationInputAction: public AbstractAction
    {
      PlatformType& platformInterface;
      T& representation;

    public:
      RepresentationInputAction(PlatformType& platformInterface, T& representation)
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
      PlatformType& platformInterface;
      T& representation;

    public:
      RepresentationOutputAction(PlatformType& platformInterface, T& representation)
        : platformInterface(platformInterface),
          representation(representation)
      {}

      virtual void execute()
      {
        platformInterface.set(representation);
      }
    };//end RepresentationInputAction

  public:
    void callMotion()
    {
      // TODO: assert?
      if(motionCallback != NULL)
      {
        getMotionInput();
        motionCallback->call();
        setMotionOutput();
      }
    }//callMotion 


    void callCognition()
    {
      // TODO: assert?
      if(cognitionCallback != NULL)
      {
        getCognitionInput();
        cognitionCallback->call();
        setCognitionOutput();
      }
    }//end callCognition

    template<class T>
    void registerCognitionInput(T& data, const std::string& name)
    {
      cout << getName() << " register Cognition input: " << name << endl;
      AbstractAction* action = new RepresentationInputAction<T>(getPlatform(), data);
      cognitionInput.push_back(action);
    }//end registerCognitionInput

    template<class T>
    void registerCognitionOutput(const T& data, const std::string& name)
    {
      cout << getName() << " register Cognition output: " << name;
      AbstractAction* action = new RepresentationOutputAction<const T>(getPlatform(), data);
      cognitionOutput.push_back(action);
    }//end registerCognitionOutput

    template<class T>
    void registerMotionInput(T& data, const std::string& name)
    {
      cout << getName() << " register Motion input: " << name << endl;
      AbstractAction* action = new RepresentationInputAction<T>(getPlatform(), data);
      motionInput.push_back(action);
    }//end registerCognitionInput

    template<class T>
    void registerMotionOutput(const T& data, const std::string& name)
    {
      cout << getName() << " register Motion output: " << name;
      AbstractAction* action = new RepresentationOutputAction<const T>(getPlatform(), data);
      motionOutput.push_back(action);
    }//end registerCognitionOutput


    //////// register own main loop callbacks /////////

    /**
     * Register callback objects for this platform interface that are regulary called
     * in the sense-think-act cycle.
     * @param motionCallback The callback object for the motion cycle or NULL if not active.
     * @param cognitionCallback The callback object for the cognition cycle or NULL if not active.
     */
    template<class T1, class T2>
    void registerCallbacks(T1* motionCallback, T2* cognitionCallback)
    {
      if(motionCallback != NULL)
      {
        std::cerr << "register MOTION callback" << std::endl;
        this->motionCallback = motionCallback;
        motionCallback->init(*this);
      }else
      {
        std::cerr << "could not register MOTION callback because it was NULL" << std::endl;
      }

      if(cognitionCallback != NULL)
      {
        std::cerr << "register COGNITION callback" << std::endl;
        this->cognitionCallback = cognitionCallback;
        cognitionCallback->init(*this);
      }else
      {
        std::cerr << "could not register COGNITION callback because it was NULL" << std::endl;
      }
    }//end registerCallbacks


    private:
      Callable* motionCallback;
      Callable* cognitionCallback;
  };
}

#endif	/* _PLATFORMINTERFACE_H */

