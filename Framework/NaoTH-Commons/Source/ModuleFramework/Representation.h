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

using namespace std;
using namespace naoth;

class Module;

class Representation : public Printable
{

private:
  std::string name;

protected:
  // pointers to the providing and requiring modules
  list<const Module*> provided;
  list<const Module*> used;
  list<const Module*> required;

  Representation(const std::string name)
    : name(name)
  {
    //std::cout << "Load " << getModuleName() << endl;
  }


public:
  virtual ~Representation() {}
  const std::string& getName() const { return name; }

  void registerProvidingModule(const Module& module)
  {
    provided.push_back(&module);
  }//end registerProvidingModule
  
  void unregisterProvidingModule(const Module& module)
  {
    provided.remove(&module);
  }//end unregisterProvidingModule


  void registerRequiringModule(const Module& module)
  {
    required.push_back(&module);
  }//end registerRequiringModule

  void unregisterRequiringModule(const Module& module)
  {
    required.remove(&module);
  }//end unregisterRequiringModule

  /**
   * This method can be overwritten bei a particular
   * representation in order to stream out some specific (visible) data
   */
  virtual void print(ostream& stream) const
  {
    // use representation name as fallback
    stream << name;
  }//end print



  virtual void serialize(std::ostream& stream) const = 0;
  virtual void deserialize(std::istream& stream) = 0;
};

/**
 * Streaming operator that writes a Representation to a stream.
 * @param stream The stream to write on.
 * @param image The Representation object.
 * @return The stream.
 */ 
//ostream& operator<<(ostream& stream, const Representation& representation);

#endif //__Representation_h_
