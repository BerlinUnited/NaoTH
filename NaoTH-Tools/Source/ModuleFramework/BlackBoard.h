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

#include "Tools/Debug/NaoTHAssert.h"


/**
 *
 */
class BlackBoard
{
private:
  /**
   * abstract class to be stored at the BlackBoard
   */
  class BlackBoardData
  {
  public:
    virtual const std::string getTypeName() const = 0;
  };

  /**
   * class BlackBoardDataHolder holds a data of type T as a member.
   * It requires the class T to be NOT abstract and to have a 
   * public default constrictor
   */
  template<class T>
  class BlackBoardDataHolder: public BlackBoardData
  {
  private:
    T instance;

  public:
    T& operator*(){ return instance; }
    virtual const std::string getTypeName() const { return typeid(T).name(); }
  };

  typedef std::map<std::string, BlackBoardData*> BlackBoardRegistry;

  /** holds the pointers to  */
  BlackBoardRegistry registry;

public:
  BlackBoard(){}

  BlackBoard(BlackBoard& blackBoard)
  {
  }

  ~BlackBoard()
  {
    // delete the registry
    BlackBoardRegistry::iterator iter;
    for(iter = registry.begin(); iter != registry.end(); iter++)
    {
      delete iter->second;
    }
  }

  /**
   *  getRepresentation returns a reference to a representation stored 
   *  on the blackboard. A new instance is created if the required 
   *  representation is not existing.
   */
  template<class T>
  T& getRepresentation(std::string name)
  {
    // search for the representation
    BlackBoardRegistry::iterator iter = registry.find(name);

    // create Representation, if necessary
    if(iter == registry.end())
    {
      BlackBoardDataHolder<T>* typedData = new BlackBoardDataHolder<T>();
      registry[name] = typedData;
      return **typedData;
    }//end if


    // retrive the representation and try to cast
    BlackBoardData* data = registry.find(name)->second;
    BlackBoardDataHolder<T>* typedData = dynamic_cast<BlackBoardDataHolder<T>*>(data);
  
    // couldn't cast the type
    if(typedData == NULL)
    {
      std::cerr << "Representation type mismatch: " << name << " is registered as "
                << data->getTypeName() << ", but "
                << typeid(T).name() << " requested." << std::endl;
      assert(false);
    }//end if

    return **typedData;
  }//end getRepresentation


  /**
   *  getConstRepresentation wraps getRepresentation:
   *  it returns a const reference to a representation
   */
  template<class T>
  const T& getConstRepresentation(std::string name)
  {
    // getRepresentation checks the type, thus no further check necessary
    return static_cast<const T&>(getRepresentation<T>(name));
  }//end getConstRepresentation
};

#endif //__BlackBoard_h_
