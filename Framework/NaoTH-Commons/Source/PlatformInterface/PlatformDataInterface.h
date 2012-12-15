/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformDataInterface_h_
#define _PlatformDataInterface_h_

#include "ActionList.h"
#include "Process.h"

#include <typeinfo>


namespace naoth
{

/*
 *
 */
class PlatformDataInterface
{
private:

  static const int DATA_INPUT_ACTION = 7;
  static const int DATA_OUTPUT_ACTION = 11;

  template<int A, class T, class PT>
  class DataAction: public AbstractAction
  {
    PT& platform;
    T& data;

  public:
    DataAction(PT& platform, T& data)
      : platform(platform),
        data(data)
    {
    }

    void execute(){ std::cerr << "no action " << A << " for " << typeid(data).name() << std::endl; }
  };//end DataAction


  template<class T, class PT>
  class DataAction<DATA_INPUT_ACTION,T,PT>: public AbstractAction
  {
    PT& platform;
    T& data;

  public:
    DataAction(PT& platform, T& data)
      : platform(platform),
        data(data)
    {
    }

    virtual void execute(){ platform.get(data); }
  };//end DataAction

  template<class T, class PT>
  class DataAction<DATA_OUTPUT_ACTION,T,PT>: public AbstractAction
  {
    PT& platform;
    T& data;

  public:
    DataAction(PT& platform, T& data)
      : platform(platform),
        data(data)
    {
    }

    virtual void execute(){ platform.set(data); }
  };//end DataAction

    
  template<class T, class PT, int ACTION>
  class DataActionCreator: public TypedActionCreator<T>
  {
    PT& platform;

  public:
    DataActionCreator(PT& platform)
      : platform(platform)
    {}

    virtual AbstractAction* createAction(T& data)
    {
      return new DataAction<ACTION,T,PT>(platform, data);
    }
  };//end class DataActionCreator


private:
  ProsessEnvironment& environment;

protected:
  PlatformDataInterface(ProsessEnvironment& environment)
    : environment(environment)
  {
  }

  /**
    *
    */
  template<class T, class PT>
  void registerInput(PT& platform)
  {
    std::cout << "platform register input: " << typeid(T).name() << std::endl;
    TypedActionCreator<T>* creator = new DataActionCreator<T,PT,DATA_INPUT_ACTION>(platform);
    environment.inputActions.set(typeid(T).name(), creator);
  }//end registerInput

  /**
    *
    */
  template<class T, class PT>
  void registerOutput(PT& platform)
  {
    std::cout << "platform register output: " << typeid(T).name() << std::endl;
    TypedActionCreator<T>* creator = new DataActionCreator<T,PT,DATA_OUTPUT_ACTION>(platform);
    environment.outputActions.set(typeid(T).name(), creator);
  }//end registerOutput

};//end class PlatformDataInterface


}// end namespace naoth

#endif // _PlatformDataInterface_h_