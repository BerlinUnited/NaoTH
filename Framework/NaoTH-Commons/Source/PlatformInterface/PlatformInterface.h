/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PLATFORMINTERFACE_H
#define  _PLATFORMINTERFACE_H

#include "Callable.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Communication/MessageQueue/MessageQueue.h"
//#include "DebugCommunication/DebugServer.h"

#include <map>
#include <list>
#include <string>
#include <iostream>

#include <typeinfo>

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

    virtual ~PlatformBase();

    /////////////////////// get ///////////////////////
    virtual string getBodyID() const = 0;
    virtual string getBodyNickName() const = 0;

    inline const string& getName() const { return platformName; }
    inline unsigned int getBasicTimeStep() const { return theBasicTimeStep; }

    MessageQueue* getMessageQueue(const std::string& name);
    
  protected:
    virtual MessageQueue* createMessageQueue(const std::string& name) = 0;
    
  private:
    std::string platformName;
    unsigned int theBasicTimeStep;
    std::map<std::string, MessageQueue*> theMessageQueue;
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


    template<class T>
    class ActionCreator
    {
    public:
      virtual AbstractAction* createAction(T& data) = 0;
    };//end ActionCreator




    std::map<std::string,void*> registeredInputActions;
    std::map<std::string,void*> registeredOutputActions;


    template<class T>
    ActionCreator<T>* getActionCreator(const std::map<std::string,void*>& registeredActions)
    {
      std::map<std::string,void*>::const_iterator iter = registeredActions.find(typeid(T).name());
      if(iter == registeredActions.end())
      {
        return NULL;
      }//end if

      return static_cast<ActionCreator<T>*>(iter->second);
    }//end getActionCreator



    template<class T>
    void registerCognitionInput(T& data)
    {
      ActionCreator<T>* creator = getActionCreator<T>(registeredInputActions);
      if(creator != NULL)
      {
        cognitionInput.push_back(creator->createAction(data));
        cout << /*getName() <<*/ " register Cognition input: " << typeid(T).name() << endl;
      }else
      {
        cerr << /*getName() <<*/ " doesn't provide Cognition input: " << typeid(T).name() << endl;
      }
    }//end registerCognitionInput


    template<class T>
    void registerCognitionOutput(const T& data)
    {
      ActionCreator<const T>* creator = getActionCreator<const T>(registeredOutputActions);
      if(creator != NULL)
      {
        cognitionOutput.push_back(creator->createAction(data));
        cout << /*getName() <<*/ " register Cognition output: " << typeid(T).name() << endl;
      }else
      {
        cerr << /*getName() <<*/ " doesn't provide Cognition output: " << typeid(T).name() << endl;
      }
    }//end registerCognitionOutput


    template<class T>
    void registerMotionInput(T& data)
    {
      ActionCreator<T>* creator = getActionCreator<T>(registeredInputActions);
      if(creator != NULL)
      {
        motionInput.push_back(creator->createAction(data));
        cout << /*getName() <<*/ " register Motion input: " << typeid(T).name() << endl;
      }else
      {
        cerr << /*getName() <<*/ " doesn't provide Motion input: " << typeid(T).name() << endl;
      }
    }//end registerMotionInput


    template<class T>
    void registerMotionOutput(const T& data)
    {
      ActionCreator<const T>* creator = getActionCreator<const T>(registeredOutputActions);
      if(creator != NULL)
      {
        motionOutput.push_back(creator->createAction(data));
        cout << /*getName() <<*/ " register Motion output: " << typeid(T).name() << endl;
      }else
      {
        cerr << /*getName() <<*/ " doesn't provide Motion output: " << typeid(T).name() << endl;
      }
    }//end registerMotionOutput
  };//end class PlatformDataInterface

  class PlatformInterfaceBase: public PlatformBase, public PlatformDataInterface
  {
  protected:
    PlatformInterfaceBase(const std::string& name, unsigned int basicTimeStep)
    : PlatformBase(name, basicTimeStep)
    {
    }
  };


  /*  the platform interface responses for 4 kinds of functionalities:
   * - get sensor data
   * - set action data
   * - initialize the cognition module and motion module
   * - main loop to call cognition and motion
   */
  template<class PlatformType>
  class PlatformInterface: public PlatformInterfaceBase
  {
  public:
    virtual ~PlatformInterface(){};

  protected:
    PlatformInterface(const std::string& name, unsigned int basicTimeStep)
      : PlatformInterfaceBase(name, basicTimeStep)
    {
      cout<<"NaoTH "<<getName()<<" starting..."<<endl;
    }

  //////////////////// GET/SET Actions /////////////////////
  private:

#define _NAOTH_INPUT_ACTION_ 7 
#define _NAOTH_OUTPUT_ACTION_ 11

    template<int ACTION, class T, typename PT>
    class RepresentationAction: public AbstractAction
    {
      PT& platform;
      T& representation;

    public:
      RepresentationAction(PT& platform, T& representation)
        : platform(platform),
          representation(representation)
      {
      }

      virtual void execute(){ std::cerr << "no action " << ACTION << " for " << typeid(representation).name() << endl; }
    };//end RepresentationAction


    template<class T, typename PT>
    class RepresentationAction<_NAOTH_INPUT_ACTION_,T,PT>
    {
      PT& platform;
      T& representation;

    public:
      RepresentationAction(PT& platform, T& representation)
        : platform(platform),
          representation(representation)
      {
      }

      virtual void execute(){ platform.get(representation); }
    };//end RepresentationAction

    template<class T, typename PT>
    class RepresentationAction<_NAOTH_OUTPUT_ACTION_,T,PT>
    {
      PT& platform;
      T& representation;

    public:
      RepresentationAction(PT& platform, T& representation)
        : platform(platform),
          representation(representation)
      {
      }

      virtual void execute(){ platform.set(representation); }
    };//end RepresentationAction



    template<class T, typename PT, int ACTION>
    class ActionCreatorImp: public ActionCreator<T>
    {
      PT& platform;

    public:
      ActionCreatorImp(PT& platform)
        : platform(platform)
      {}

      virtual AbstractAction* createAction(T& data)
      {
        return (AbstractAction*) new RepresentationAction<ACTION,T,PT>(platform, data);
      }
    };//end InputActionCreator


  protected:
    template<class T, typename PT>
    void registerInput(PT& platform)
    {
      cout << getName() << " register input: " << typeid(T).name() << endl;
      registeredInputActions[typeid(T).name()] = new ActionCreatorImp<T,PT,_NAOTH_INPUT_ACTION_>(platform);
    }//end registerInput

    template<class T, typename PT>
    void registerOutput(PT& platform)
    {
      cout << getName() << " register output: " << typeid(T).name() << endl;
      registeredOutputActions[typeid(T).name()] = new ActionCreatorImp<T,PT,_NAOTH_OUTPUT_ACTION_>(platform);
    }//end registerOutput


  public:

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
      }

      if(cognitionCallback != NULL)
      {
        std::cerr << "register COGNITION callback" << std::endl;
        this->cognitionCallback = cognitionCallback;
        cognitionCallback->init(*this);
      }
    }//end registerCallbacks


  };//end class PlatformInterface
}

#endif  /* _PLATFORMINTERFACE_H */

