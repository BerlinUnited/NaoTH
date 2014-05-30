/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 */

#ifndef _ActionList_H
#define _ActionList_H

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <cassert>
#include <typeinfo>

#undef PRINT_DEBUG
#ifdef DEBUG_PLATFORM
#  define PRINT_DEBUG(m) std::err << m << std::endl
#else
#  define PRINT_DEBUG(m) ((void)0)
#endif

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
  virtual ~AbstractAction(){} //ACHTUNG: needed, for the desructor of the derived classes to be called
  virtual void execute() = 0;
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

  virtual ~ActionList()
  {
    PRINT_DEBUG("destruct ActionList");
    // delete all actions
    for(ActionListT::iterator iter = actions.begin(); iter != actions.end(); ++iter)
    {
      delete (*iter);
    }
  }

  void execute() const
  {
    for(ActionListT::const_iterator iter = actions.begin(); iter != actions.end(); ++iter)
    {
      (*iter)->execute();
    }
  }

  void push_back(AbstractAction* action)
  {
    assert(action != NULL);
    actions.push_back(action);
  }
};//end class ActionList


/**
* HACK
*/
class Void
{
public:
  virtual ~Void(){}
};

/**
  * @class TypedActionCreator
  * Abstract interface for a class which creates an action
  * based on given data and its type.
  * @see TypedActionCreatorMap
  */
template<class T>
class TypedActionCreator : public Void
{
public:
  virtual ~TypedActionCreator(){} //ACHTUNG: needed, for the desructor of the derived classes to be called
  virtual AbstractAction* createAction(T& data) = 0;
};//end TypedActionCreator


/**
  * @class TypedActionCreatorMap
  * Inplements a creator for AbstractAction. An action is created using a TypedActionCreatorMap 
  * if avaliable. All avaliable creators are stored in a map.
  */
class TypedActionCreatorMap
{
private:
  typedef std::map<std::string, Void*> TypedActionCreatorMapT;

  // avaliable creators
  TypedActionCreatorMapT registeredActions;

  /**
  * try to find an action in the given list and returns the according typed ActionCreator
  */
  template<class T>
  TypedActionCreator<T>* getActionCreator() const
  {
    TypedActionCreatorMapT::const_iterator iter = registeredActions.find(typeid(T).name());
    if(iter == registeredActions.end())
    {
      return NULL;
    }//end if

    return static_cast<TypedActionCreator<T>*>(iter->second);
  }//end getActionCreator

public:

  /**
  * 
  */
  virtual ~TypedActionCreatorMap()
  {
    PRINT_DEBUG("destruct TypedActionCreatorMap ");

    for(TypedActionCreatorMapT::iterator iter = registeredActions.begin(); iter != registeredActions.end(); ++iter)
    {
      delete iter->second;
    }//end for
  }


  /**
  * creates an action for the type T if an apropriate creator is avaliable
  */
  template<class T>
  AbstractAction* createAction(T& data) const
  {
    TypedActionCreator<T>* creator = getActionCreator<T>();
      
    if(creator == NULL)
      return NULL;

    return creator->createAction(data);
  }//end createAction

  /**
  * adds a new creator or overwrites an existing one
  */
  template<class T>
  void add(TypedActionCreator<T>* actionCreator)
  {
    registeredActions[typeid(T).name()] = actionCreator;
  }
};//end class TypedActionCreatorMap


}// end namespace naoth

#endif // _ActionList_H
