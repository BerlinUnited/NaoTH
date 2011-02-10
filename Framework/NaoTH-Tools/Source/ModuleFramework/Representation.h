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

//#include "Module.h"
#include "BlackBoard.h"
#include "BlackBoardInterface.h"

#include "Node.h"

using namespace std;
using namespace naoth;

class Module;

class Representation : public Printable
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
//ostream& operator<<(ostream& stream, const Representation& representation);

#endif //__Representation_h_
