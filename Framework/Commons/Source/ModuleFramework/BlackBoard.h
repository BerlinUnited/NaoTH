/**
* @file BlackBoard.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class BlackBoard
*
*/

#ifndef _BlackBoard_h_
#define _BlackBoard_h_

#include <iostream>
#include <map>
#include <string>
#include <typeinfo>

#include "Tools/Debug/NaoTHAssert.h"
#include "Representation.h"
#include "DataHolder.h"

/**
* @class BlackBoard 
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
@code

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

@endcode
*/
class BlackBoard
{
public:

  /**
  * @class BlackBoardData
  * abstract class to be stored at the BlackBoard
  */
  class BlackBoardData
  {
  public:
    virtual const std::string getTypeName() const = 0;
    virtual const Representation& getRepresentation() const = 0;
    virtual Representation& getRepresentation() = 0;

    virtual ~BlackBoardData() {}
  };


  /** */
  typedef std::map<std::string, BlackBoardData*> Registry;


private:
  /**
  * @class BlackBoardDataHolder holds a data of type T as a member.
  * It requires the class T to be NOT abstract and to have a 
  * public default constrictor
  */
  template<class T>
  class BlackBoardDataHolder: public BlackBoardData
  {
  private:
    T instance;

  public:
    BlackBoardDataHolder(const std::string& name) : instance(name) {}

    T& operator*(){ return instance; }
    const T& operator*() const { return instance; }

    virtual const std::string getTypeName() const { return typeid(T).name(); }
    virtual const Representation& getRepresentation() const { return **this; }
    virtual Representation& getRepresentation() { return **this; }
  };


  /** holds the pointers to the representations */
  Registry registry;
  
  /*
  // exterimental:
  class BlackBoardPtr
  {
  public:
    BlackBoard* prt;
  };
  */

public:
  BlackBoard()
  {
    // exterimental: first trial to put the blackboard as a representation
    //DataHolder<BlackBoardPtr>& bb_pointer = getRepresentation<DataHolder<BlackBoardPtr> >("BlackBoard");
    //(*bb_pointer).prt = this;
  }

  virtual ~BlackBoard()
  {
    // delete the registry
    for(Registry::iterator iter = registry.begin(); iter != registry.end(); ++iter)
    {
      delete iter->second;
    }
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

    // the const method cannot create new representations
    if(iter == registry.end())
    {
      // TODO: throw
      std::cerr << "[BlackBoard] Representation not existing: " 
                << name << " of type " << typeid(T).name() 
                << "is not registred and cannot be created." << std::endl;
      assert(false);
    }


    // try to cast the representation to the given type
    BlackBoardDataHolder<T>* typedData = dynamic_cast<BlackBoardDataHolder<T>*>(iter->second);
  
    // couldn't cast the type
    if(typedData == NULL)
    {
      // TODO: throw
      std::cerr << "[BlackBoard] Representation type mismatch: " 
                << name << " is registered as " << iter->second->getTypeName() 
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
    // try to find the data with the given name;
    // in case it doesn't exist we get an iterator to the closest existing data
    Registry::iterator iter = registry.lower_bound(name);

    // the data with the given name doesn't exist; insert a new one
    if( iter == registry.end() || !(iter->first == name) ) {
      iter = registry.insert(iter, std::make_pair(name, new BlackBoardDataHolder<T>(name)));
    }

    // try to cast the representation to the given type
    BlackBoardDataHolder<T>* typedData = dynamic_cast<BlackBoardDataHolder<T>*>(iter->second);
  
    // couldn't cast the type
    if(typedData == NULL)
    {
      // TODO: throw
      std::cerr << "Representation type mismatch: " 
                << name << " is registered as " << iter->second->getTypeName() 
                << ", but " << typeid(T).name() << " is requested." << std::endl;
      assert(false);
    }//end if

    return **typedData;
  }//end getRepresentation


  const Registry& getRegistry() const {
    return registry;
  }

  Registry& getRegistry() {
    return registry;
  }

  template<class T>
  T& operator[] (const std::string& name) {
    return getRepresentation<T>(name);
  }

  template<class T>
  const T& operator[] (const std::string& name) const {
    return getRepresentation<T>(name);
  }

};

#endif //_BlackBoard_h_
