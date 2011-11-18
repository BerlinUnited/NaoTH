/**
* @file BlackBoard.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BlackBoard
*
* BlackBoard (BB) is a kind of a very simple database providing a possibility 
* to store and to access instances of arbitrary classes. I.e., it allows to store 
* and to share heterogeneous information. The data units stored at 
* the blackboards are called "representations". A representation is 
* defined by a not abstract class which has a public default constructor.
*
* The basic functionality of BB is implemented in the method 
* getRepresentation. It provides access to a particular representation.
* If getRepresentation<T>("name") is called, BB creates an instance of the 
* class T (representations), store it in the internal map at "name" and return 
* a reference to it. If the there is already a representation with this name
* a reference to it is returned (if the type doesn't match an assert is triggered).
*
*
* Usage:

  class A
  {
  public:
    int x;
  };

  ...

  BlackBoard b;
  A& a = b.getRepresentation<A>("a");
  a.x = 2;

  ...

*/


#ifndef __BlackBoard_h_
#define __BlackBoard_h_

#include <iostream>
#include <map>
#include <string>
#include <typeinfo>
#include <assert.h>

//#include "Tools/Debug/NaoTHAssert.h"
#include "Representation.h"

/**
 *
 */
class BlackBoard
{
public:

  /**
   * abstract class to be stored at the BlackBoard
   */
  class BlackBoardData
  {
  public:
    virtual const std::string getTypeName() const = 0;
    virtual const std::string getName() const = 0;
    virtual const Representation& getRepresentation() const = 0;

    virtual ~BlackBoardData() {}
  };//end class BlackBoardData


  /** */
  typedef std::map<std::string, BlackBoardData*> Registry;


private:
  /**
   * class BlackBoardDataHolder holds a data of type T as a member.
   * It requires the class T to be NOT abstract and to have a 
   * public default constrictor
   */
  template<class T>
  class BlackBoardDataHolder: public BlackBoardData
  {
  private:
    std::string name;
    T instance;

  protected:
    //virtual void* getDataPtr() const { return static_cast<void*>(&instance); }

  public:
    BlackBoardDataHolder(const std::string& name)
      : name(name), 
        instance(name)
    {
    }

    T& operator*(){ return instance; }

    virtual const std::string getTypeName() const { return typeid(T).name(); }
    virtual const std::string getName() const { return name; }
    virtual const Representation& getRepresentation() const { return instance; }
  };



  /** holds the pointers to  */
  Registry registry;

public:
  BlackBoard(){}
  BlackBoard(BlackBoard& /*blackBoard*/){}

  virtual ~BlackBoard()
  {
    // delete the registry
    for(Registry::iterator iter = registry.begin(); iter != registry.end(); iter++)
    {
      delete iter->second;
    }
  }

  /**
   *
   */
  const Registry& getRegistry() const
  {
    return registry;
  }

  /**
   *  returns a reference to a representation stored 
   *  on the blackboard. Asserttion is thrown if no 
   *  according representation is avaliable.
   */
  template<class T>
  const T& getRepresentation(const std::string& name) const
  {
    // search for the representation
    Registry::const_iterator iter = registry.find(name);

    // create Representation, if necessary
    if(iter == registry.end())
    {
      // TODO: throw
      std::cerr << "Representation not existing: " 
                << name << " of type " << typeid(T).name() 
                << "is not registred and cannot be created." << std::endl;
      assert(false);
    }//end if


    // retrive the representation and try to cast
    BlackBoardData* data = iter->second;
    BlackBoardDataHolder<T>* typedData = dynamic_cast<BlackBoardDataHolder<T>*>(data);
  
    // couldn't cast the type
    if(typedData == NULL)
    {
      // TODO: throw
      std::cerr << "Representation type mismatch: " 
                << name << " is registered as " << data->getTypeName() 
                << ", but " << typeid(T).name() << " is requested." << std::endl;
      assert(false);
    }//end if

    return **typedData;
  }//end getRepresentation


  /**
   *  returns a reference to a representation stored 
   *  on the blackboard. A new instance is created if the required 
   *  representation is not existing.
   */
  template<class T>
  T& getRepresentation(const std::string& name)
  {
    // search for the representation
    Registry::iterator iter = registry.find(name);

    // create Representation, if necessary
    if(iter == registry.end())
    {
      BlackBoardDataHolder<T>* typedData = new BlackBoardDataHolder<T>(name);
      registry[name] = typedData;
      return **typedData;
    }//end if


    // retrive the representation and try to cast
    BlackBoardData* data = iter->second;
    BlackBoardDataHolder<T>* typedData = dynamic_cast<BlackBoardDataHolder<T>*>(data);
  
    // couldn't cast the type
    if(typedData == NULL)
    {
      // TODO: throw
      std::cerr << "Representation type mismatch: " 
                << name << " is registered as " << data->getTypeName() 
                << ", but " << typeid(T).name() << " is requested." << std::endl;
      assert(false);
    }//end if

    return **typedData;
  }//end getRepresentation

};

#endif //__BlackBoard_h_
