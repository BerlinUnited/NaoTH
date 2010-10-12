/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PLATFORMINTERFACE_H
#define	_PLATFORMINTERFACE_H

#include "Callable.h"
#include "Tools/Debug/NaoTHAssert.h"

#include <list>
#include <string>
#include <iostream>

namespace naoth
{
  using namespace std;

  /*
  * AbstractAction defines an interface for an action.
  * It is used to wrap the call of a get/set method to read/write 
  * a representation.
  */
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



  /*
   * Holds the lists of references to representations 
   * which have to be exchanged between platform and motion/cognition
   */
  class PlatformDataInterface
  {
  public:
    virtual ~PlatformDataInterface();

  protected:
    PlatformDataInterface();

  protected:
    Callable* motionCallback;
    Callable* cognitionCallback;

    ActionList motionInput;
    ActionList motionOutput;

    ActionList cognitionInput;
    ActionList cognitionOutput;

  public:
    virtual void callCognition();
    virtual void getCognitionInput();
    virtual void setCognitionOutput();

    virtual void callMotion();
    virtual void getMotionInput();
    virtual void setMotionOutput();
  
  public:
    inline void delete_action_list(ActionList& actionList);
    inline void execute(ActionList& actionList) const;
  };//end class PlatformDataInterface



  /*  the platform interface responses for 4 kinds of functionalities:
   * - get sensor data
   * - set action data
   * - initialize the cognition module and motion module
   * - main loop to call cognition and motion
   */
  template<class PlatformType>
  class PlatformInterface: public PlatformBase, public PlatformDataInterface
  {
  public:
    virtual ~PlatformInterface(){};

  protected:
    PlatformInterface(const std::string& name, unsigned int basicTimeStep)
      : PlatformBase(name, basicTimeStep)
    {
      cout<<"NaoTH "<<getName()<<" starting..."<<endl;
    }

  //////////////////// GET/SET Actions /////////////////////
  private:

    // uncomment to have a runtime check of representation registration
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

    virtual PlatformType& getPlatform() = 0;

    template<class T>
    class RepresentationInputAction: public AbstractAction
    {
      PlatformType& platform;
      T& representation;

    public:
      RepresentationInputAction(PlatformType& platform, T& representation)
        : platform(platform),
          representation(representation)
      {}

      virtual void execute()
      {
        platform.get(representation);
      }
    };//end RepresentationInputAction

    template<class T>
    class RepresentationOutputAction: public AbstractAction
    {
      PlatformType& platform;
      T& representation;

    public:
      RepresentationOutputAction(PlatformType& platform, T& representation)
        : platform(platform),
          representation(representation)
      {}

      virtual void execute()
      {
        platform.set(representation);
      }
    };//end RepresentationInputAction


  public:
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


  };//end class PlatformInterface
}

#endif	/* _PLATFORMINTERFACE_H */

