/**
 * @file PlatformDataInterface.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _PlatformDataInterface_h_
#define _PlatformDataInterface_h_

#include "ActionList.h"
#include "Process.h"

#include <typeinfo>

#undef PRINT_DEBUG
#ifdef DEBUG_PLATFORM
#  define PRINT_DEBUG(m) std::err << m << std::endl
#else
#  define PRINT_DEBUG(m) ((void)0)
#endif

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

  ~DataAction()
  {
    PRINT_DEBUG("destruct DataAction<" << A << "> " << typeid(T).name());
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

  ~DataAction()
  {
    PRINT_DEBUG("destruct DataAction<DATA_INPUT_ACTION> " << typeid(T).name());
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

  ~DataAction()
  {
    PRINT_DEBUG("destruct DataAction<DATA_OUTPUT_ACTION> " << typeid(T).name());
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

  ~DataActionCreator()
  {
    PRINT_DEBUG("destruct DataActionCreator " << typeid(T).name());
  }

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
  ProcessEnvironment& environment;

protected:
  PlatformDataInterface(ProcessEnvironment& environment)
    : environment(environment)
  {
  }

  ~PlatformDataInterface()
  {
    PRINT_DEBUG("destruct PlatformDataInterface");
  }

  template<class T, class PT>
  void registerInput(PT& platform)
  {
    PRINT_DEBUG("platform register input: " << typeid(T).name());
    TypedActionCreator<T>* creator = new DataActionCreator<DATA_INPUT_ACTION,T,PT>(platform);
    environment.inputActions.add(creator);
  }

  template<class T, class PT>
  void registerOutput(PT& platform)
  {
    PRINT_DEBUG("platform register output: " << typeid(T).name());
    TypedActionCreator<T>* creator = new DataActionCreator<DATA_OUTPUT_ACTION,T,PT>(platform);
    environment.outputActions.add(creator);
  }

};//end class PlatformDataInterface


}// end namespace naoth

#endif // _PlatformDataInterface_h_
