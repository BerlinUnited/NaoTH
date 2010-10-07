/**
* @file Representation.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Representation
*/

#ifndef __Representation_h_
#define __Representation_h_

// standart
#include <string>
#include <iostream>
#include <list>

#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/DataStructures/Printable.h"

#include "Module.h"
#include "BlackBoard.h"
#include "BlackBoardInterface.h"

using namespace std;

class Representation : public naoth::Printable
{

private:
  std::string name;

protected:
  // pointers to the providing and requiring modules
  Node<const Module, const Module> node;

  Representation(const std::string name)
    : name(name)
  {
    //std::cout << "Load " << getModuleName() << endl;
  }


public:
  virtual ~Representation() {}
  const std::string& getName() const { return (*this).name; }


  void registerUsingModule(const Module& module)
  {
    node.registerTo(module);
  }//end registerUsingModule
  
  void unregisterUsingModule(const Module& module)
  {
    node.unregisterTo(module);
  }//end unRegisterUsingModule



  void registerProvidingModule(const Module& module)
  {
    node.registerFrom(module);
  }//end registerProvidingModule
  
  void unregisterProvidingModule(const Module& module)
  {
    node.unregisterFrom(module);
  }//end unregisterProvidingModule


  /**
   * This method can be overwritten bei a particular
   * representation in order to stream out some specific (visible) data
   */
  virtual void print(ostream& stream) const
  {
    // use representation name as fallback
    stream << name;
  }//end print
};

/**
 * Streaming operator that writes a Representation to a stream.
 * @param stream The stream to write on.
 * @param image The Representation object.
 * @return The stream.
 */ 
ostream& operator<<(ostream& stream, const Representation& representation);


/**
 * Connects a arbitrary class with Representation
 */ 
template<class T>
class DataHolder: public Representation
{
private:
  // creates an object of the data
  // (this requires the class T to have a default constructor)
  T data;

public:
  DataHolder() : Representation(typeid(T).name()){}
  virtual ~DataHolder(){}

  T& operator*(){ return data; }
  const T& operator*() const{ return data; }
  
  // wrap the print, fromDataStream and toDataStream of the data member

  virtual void print(ostream& stream) const
  {
    const T* asPointer =&data;
    const Printable* asPrintable = dynamic_cast<const Printable*>(asPointer);
    if(asPrintable != NULL)
    {
      stream << *asPrintable;
    }
    else
    {
      // use representation by default
      stream << getName();
    }
  }//end print
};

#endif //__Representation_h_
