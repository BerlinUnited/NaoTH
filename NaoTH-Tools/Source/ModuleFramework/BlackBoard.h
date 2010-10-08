/**
* @file BlackBoard.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class BlackBoard
*/


#ifndef __BlackBoard_h_
#define __BlackBoard_h_

#include <iostream>
#include <map>
#include <string>
#include <typeinfo>

#include "Tools/Debug/NaoTHAssert.h"

class Representation;


class BlackBoardData
{
public:
  virtual const std::string getTypeName() const = 0;
};

template<class T>
class BlackBoardDataHolder: public BlackBoardData
{
private:
  T instance;

public:
  T& operator*(){ return instance; }
  virtual const std::string getTypeName() const { return typeid(T).name(); }
};


class BlackBoard
{
private:
  typedef std::map<std::string, BlackBoardData*> BlackBoardRegistry;

  BlackBoardRegistry registry;

public:
  BlackBoard(){}

  ~BlackBoard()
  {
    // delete the registry
    BlackBoardRegistry::iterator iter;
    for(iter = registry.begin(); iter != registry.end(); iter++)
    {
      delete iter->second;
    }
  }

  template<class T>
  T& getRepresentation(std::string name)
  {
    // create Representation, if necessary
    if(registry.find(name) == registry.end())
    {
      registry[name] = new BlackBoardDataHolder<T>();
    }

    // runtime type check
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

  template<class T>
  const T& getConstRepresentation(std::string name)
  {
    // getRepresentation checks the type, thus no further check necessary
    return static_cast<const T&>(getRepresentation<T>(name));
  }//end getConstRepresentation
};

#endif //__BlackBoard_h_
