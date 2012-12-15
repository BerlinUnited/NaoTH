/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PLATFORMINTERFACE_H
#define _PLATFORMINTERFACE_H

#include "Callable.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Communication/MessageQueue/MessageQueue.h"
#include "Tools/DataStructures/Serializer.h"

#include "PlatformDataInterface.h"

#include <map>
#include <list>
#include <string>
#include <iostream>

#include <typeinfo>

namespace naoth
{

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
    virtual std::string getBodyID() const = 0;
    virtual std::string getBodyNickName() const = 0;
    virtual std::string getHeadNickName() const = 0;

    inline const std::string& getName() const { return platformName; }
    inline unsigned int getBasicTimeStep() const { return theBasicTimeStep; }

  private:
    std::string platformName;
    unsigned int theBasicTimeStep;
  };//end class PlatformBase


  /**
   *
   */
  class PlatformChannelInterface: public PlatformDataInterface
  {
  protected:
    PlatformChannelInterface()
    {
    }

    virtual ~PlatformChannelInterface()
    {

    }

  public:
    template<class T, class ST>
    void registerCognitionInputChanel(T& data) { cognitionInput.push_back(createInputChanelAction<T,ST>(data)); }
    template<class T, class ST>
    void registerMotionInputChanel(T& data) { motionInput.push_back(createInputChanelAction<T,ST>(data)); }
    template<class T, class ST>
    void registerCognitionOutputChanel(const T& data) { cognitionOutput.push_back(createOutputChanelAction<T,ST>(data)); }
    template<class T, class ST>
    void registerMotionOutputChanel(const T& data) { motionOutput.push_back(createOutputChanelAction<T,ST>(data)); }

  private:
    template<class T, class ST>
    AbstractAction* createOutputChanelAction(const T& data)
    {
      class OutputChanelAction: public AbstractAction
      {
        MessageWriter writer;
        const T& data;
      
      public:
        OutputChanelAction(MessageQueue* messageQueue, const T& data) 
          : writer(messageQueue),
            data(data)
        {
        }

        ~OutputChanelAction(){}
      
        void execute()
        {
          std::stringstream hmmsg;
          ST::serialize(data, hmmsg);
          writer.write(hmmsg.str());
        }//end execute
      };

      return new OutputChanelAction(getMessageQueue(typeid(T).name()), data);
    }//end createOutputChanelAction

    template<class T, class ST>
    AbstractAction* createInputChanelAction(T& data)
    {
      class InputChanelAction: public AbstractAction
      {
        MessageReader reader;
        T& data;

      public:
        InputChanelAction(MessageQueue* messageQueue, T& data) 
          : reader(messageQueue),
            data(data)
        {
        }

        ~InputChanelAction(){}
      
        void execute()
        {
          if ( !reader.empty() )
          {
            std::string msg = reader.read();
            // drop old message
            while ( !reader.empty() )
            {
              msg = reader.read();
            }
            std::stringstream ss(msg);
            ST::deserialize(ss, data);
          }//end if
        }//end execute
      };

      return new InputChanelAction(getMessageQueue(typeid(T).name()), data);
    }//end createInputChanelAction
  };


  class PlatformInterfaceBase: public PlatformBase, public PlatformChannelInterface
  {
  public:
    PlatformInterfaceBase(const std::string& name, unsigned int basicTimeStep)
      : PlatformBase(name, basicTimeStep)
    {}
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
    virtual ~PlatformInterface(){}

  protected:
    PlatformInterface(const std::string& name, unsigned int basicTimeStep)
      : PlatformInterfaceBase(name, basicTimeStep)
    {
      std::cout << "NaoTH " << getName() << " starting..." << std::endl;
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

      void execute(){ std::cerr << "no action " << ACTION << " for " << typeid(representation).name() << std::endl; }
    };//end RepresentationAction


    template<class T, typename PT>
    class RepresentationAction<_NAOTH_INPUT_ACTION_,T,PT>: public AbstractAction
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
    class RepresentationAction<_NAOTH_OUTPUT_ACTION_,T,PT>: public AbstractAction
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
        return new RepresentationAction<ACTION,T,PT>(platform, data);
      }
    };//end InputActionCreator


  protected:
    template<class T, typename PT>
    void registerInput(PT& platform)
    {
      std::cout << getName() << " register input: " << typeid(T).name() << std::endl;
      registeredInputActions.set(typeid(T).name(), new ActionCreatorImp<T,PT,_NAOTH_INPUT_ACTION_>(platform));
    }//end registerInput

    template<class T, typename PT>
    void registerOutput(PT& platform)
    {
      std::cout << getName() << " register output: " << typeid(T).name() << std::endl;
      registeredOutputActions.set(typeid(T).name(), new ActionCreatorImp<T,PT,_NAOTH_OUTPUT_ACTION_>(platform));
    }//end registerOutput


  public:

    //////// register own main loop callbacks /////////

    /**
     * Register callback objects for this platform interface that are regulary called
     * in the sense-think-act cycle.
     * @param motionCallback The callback object for the motion cycle or NULL if not active.
     * @param cognitionCallback The callback object for the cognition cycle or NULL if not active.
     */
    //template<class T1, class T2>
    void registerCallbacks(Callable* motionCallback, Callable* cognitionCallback)
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

