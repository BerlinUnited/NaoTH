/**
* @file Representation.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Representation
*/

#ifndef _Representation_h_
#define _Representation_h_

// standart
#include <string>
#include <iostream>
#include <list>

#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/DataStructures/Printable.h"


template<class T>
class MsgIn
{
public: typedef std::istream type;
};

template<class T>
class MsgOut
{
public: typedef std::ostream type;
};


class Module;

class Representation : public naoth::Printable
{

private:
  std::string name;

protected:
  // pointers to the providing and requiring modules
  std::list<const Module*> provide;
  std::list<const Module*> require;

  Representation(const std::string& name)
    : name(name)
  {
  }


public:
  virtual ~Representation() {}
  const std::string& getName() const { return name; }

  void registerProvide(const Module& module)
  {
    provide.push_back(&module);
  }
  
  void unregisterProvide(const Module& module)
  {
    provide.remove(&module);
  }

  void registerRequire(const Module& module)
  {
    require.push_back(&module);
  }

  void unregisterRequire(const Module& module)
  {
    require.remove(&module);
  }

  /**
   * This method can be overwritten bei a particular
   * representation in order to stream out some specific (visible) data
   */
  virtual void print(std::ostream& stream) const
  {
    // use representation name as fallback
    stream << name;
  }//end print



  virtual void serialize(MsgOut<Representation>::type& stream) const = 0;
  virtual void deserialize(MsgIn<Representation>::type& stream) = 0;
};

/**
 * Streaming operator that writes a Representation to a stream.
 * @param stream The stream to write on.
 * @param image The Representation object.
 * @return The stream.
 */ 
//ostream& operator<<(ostream& stream, const Representation& representation);

#endif //_Representation_h_
