/**
 * @file PlatformDataInterface.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformDataInterface_h_
#define _PlatformDataInterface_h_

#include "ActionList.h"
#include "Process.h"

#include <typeinfo>


namespace naoth
{

static const int DATA_INPUT_ACTION = 7;
static const int DATA_OUTPUT_ACTION = 11;

/*
 * @class DataAction
 * In general, this is a basis template for as action involving two objects (data and platform) 
 * of (possibly) different data types.
 * This template implements only a dummy action and needs to be specialized.
 * Template parameter A is needed to declare different actions for same types of data and platform.
 */
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


/*
 * @class DataAction
 * Spezialization: this action calls a member method get() on the platform with data as parameter
 */
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


/*
 * @class DataAction
 * Spezialization: this action calls a member method set() on the platform with data as parameter
 */
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



/*
 * @class DataActionCreator
 * Implements a TypedActionCreator. 
 * Main idea is: you need to know the types T and PT while creating an object of
 * DataActionCreator, but you only need to know the type T to create an action
 * using this action creator later on.
 */
template<int ACTION, class T, class PT>
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



/*
 * @class
 * 
 */
class PlatformDataInterface
{
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
    TypedActionCreator<T>* creator = new DataActionCreator<DATA_INPUT_ACTION,T,PT>(platform);
    environment.inputActions.add(creator);
  }//end registerInput

  /**
    *
    */
  template<class T, class PT>
  void registerOutput(PT& platform)
  {
    std::cout << "platform register output: " << typeid(T).name() << std::endl;
    TypedActionCreator<T>* creator = new DataActionCreator<DATA_OUTPUT_ACTION,T,PT>(platform);
    environment.outputActions.add(creator);
  }//end registerOutput

};//end class PlatformDataInterface


}// end namespace naoth

#endif // _PlatformDataInterface_h_