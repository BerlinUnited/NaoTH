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



/**
  * 
  */
template<class T>
class TypedActionCreator
{
public:
  virtual AbstractAction* createAction(T& data) = 0;
};//end TypedActionCreator


/**
  * 
  */
class TypedActionCreatorMap
{
private:
  typedef std::map<std::string,void*> TypedActionCreatorMapT;
  TypedActionCreatorMapT registeredActions;

protected:
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
  void set(std::string name, TypedActionCreator<T>* actionCreator)
  {
    registeredActions[name] = actionCreator;
  }
};


}// end namespace naoth

#endif // _ActionList_H
