/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformDataInterface_H
#define _PlatformDataInterface_H

//#include "Tools/Debug/NaoTHAssert.h"
#include "Callable.h"
#include "Tools/Communication/MessageQueue/MessageQueue.h"
#include "Tools/DataStructures/Serializer.h"

#include <map>
#include <list>
#include <string>
#include <iostream>

#include <typeinfo>

namespace naoth
{

  /*
  * AbstractAction defines an interface for an action.
  * It is used to wrap the call of a get/set method to read/write 
  * a representation.
  */
  class AbstractAction
  {
  public:
    virtual void execute() = 0;
  };

  /**
   * 
   */
  template<class T>
  class ActionCreator
  {
  public:
    virtual AbstractAction* createAction(T& data) = 0;
  };//end ActionCreator


  /**
   * 
   */
  class ActionCreatorMap
  {
  private:
    typedef std::map<std::string,void*> ActionCreatorMapT;
    ActionCreatorMapT registeredActions;

  protected:
    /**
     * try to find an action in the given list and returns the according typed ActionCreator
     */
    template<class T>
    ActionCreator<T>* getActionCreator() const
    {
      ActionCreatorMapT::const_iterator iter = registeredActions.find(typeid(T).name());
      if(iter == registeredActions.end())
      {
        return NULL;
      }//end if

      return static_cast<ActionCreator<T>*>(iter->second);
    }//end getActionCreator


  public:
    /**
     * creates an action for the type T if an apropriate creator is avaliable
     */
    template<class T>
    AbstractAction* createAction(T& data) const
    {
      ActionCreator<T>* creator = getActionCreator<T>();
      
      if(creator == NULL)
        return NULL;

      return creator->createAction(data);
    }//end createAction


    /**
     * adds a new creator or overwrites an existing one
     */
    template<class T>
    void set(std::string name, ActionCreator<T>* actionCreator)
    {
      registeredActions[name] = actionCreator;
    }
  };
  
  /**
   * 
   */
  class ActionList
  {
  protected:
    typedef std::list<AbstractAction*> ActionListT;

    // 
    ActionListT actions;

  public:

    ~ActionList()
    {
      // delete all actions
      for(ActionListT::iterator iter = actions.begin(); iter != actions.end(); ++iter)
      {
        delete (*iter);
      }//end for
      actions.clear();
    }

    void execute() const
    {
      for(ActionListT::const_iterator iter = actions.begin(); iter != actions.end(); ++iter)
      {
        (*iter)->execute();
      }//end for
    }//end execute

    void push_back(AbstractAction* action)
    {
      assert(action != NULL);
      actions.push_back(action);
    }
  };//end class ActionList


  /*
   * Holds the lists of references to representations 
   * which have to be exchanged between platform and motion/cognition
   */
  class PlatformDataInterface
  {
  public:
    ~PlatformDataInterface();

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

    // those methods can be overloaded to execute custom code in the platform
    virtual void callCognition();
    virtual void getCognitionInput();
    virtual void setCognitionOutput();

    virtual void callMotion();
    virtual void getMotionInput();
    virtual void setMotionOutput();
  
  public:


    template<class T>
    bool registerAction(T& data, const ActionCreatorMap& avaliableActions, ActionList& actionList)
    {
      AbstractAction* action = avaliableActions.createAction(data);
      
      // if an action could be created put it in the list
      if(action) actionList.push_back(action);

      return action != NULL;
    }//end registerCognitionInput


    template<class T>
    void registerCognitionInput(T& data);


    template<class T>
    void registerCognitionOutput(const T& data);


    template<class T>
    void registerMotionInput(T& data);


    template<class T>
    void registerMotionOutput(const T& data);



  // ------  registered actions  ------


  protected:
    // registered (available) actions
    ActionCreatorMap registeredInputActions;
    ActionCreatorMap registeredOutputActions;



  // ------  messages  ------
  protected:
    virtual MessageQueue* createMessageQueue(const std::string& name) = 0;


  protected:
      std::map<std::string, MessageQueue*> theMessageQueue;

      MessageQueue* getMessageQueue(const std::string& name);
    
  };//end class PlatformDataInterface



  //TODO: put it to *.ipp
  template<class T>
  void PlatformDataInterface::registerCognitionInput(T& data)
  {
    if(registerAction(data, registeredInputActions, cognitionInput))
      std::cout << /*getName() <<*/ " register Cognition input: " << typeid(T).name() << std::endl;
    else
      std::cerr << /*getName() <<*/ " doesn't provide Cognition input: " << typeid(T).name() << std::endl;
  }//end registerCognitionInput


  template<class T>
  void PlatformDataInterface::registerCognitionOutput(const T& data)
  {
    if(registerAction(data, registeredOutputActions, cognitionOutput))
      std::cout << /*getName() <<*/ " register Cognition output: " << typeid(T).name() << std::endl;
    else
      std::cerr << /*getName() <<*/ " doesn't provide Cognition output: " << typeid(T).name() << std::endl;
  }//end registerCognitionOutput


  template<class T>
  void PlatformDataInterface::registerMotionInput(T& data)
  {
    if(registerAction(data, registeredInputActions, motionInput))
      std::cout << /*getName() <<*/ " register Motion input: " << typeid(T).name() << std::endl;
    else
      std::cerr << /*getName() <<*/ " doesn't provide Motion input: " << typeid(T).name() << std::endl;
  }//end registerMotionInput


  template<class T>
  void PlatformDataInterface::registerMotionOutput(const T& data)
  {
    if(registerAction(data, registeredOutputActions, motionOutput))
      std::cout << /*getName() <<*/ " register Motion output: " << typeid(T).name() << std::endl;
    else
      std::cerr << /*getName() <<*/ " doesn't provide Motion output: " << typeid(T).name() << std::endl;
  }//end registerMotionOutput

}// end namespace naoth

#endif // _PlatformDataInterface_H