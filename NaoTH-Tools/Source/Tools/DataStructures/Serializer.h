#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/DataStructures/Streamable.h"

#include <ostream>
#include <istream>

namespace naoth
{

  /** Non-template abstract serializer base class. */
  class AbstractSerializer
  {
  public:
      /** Serialize a data type from a specific instance to an outstream */
      virtual void serialize(const Streamable& /*representation*/, std::ostream& /*stream*/) const = 0 ;
      /** Deserialize from an inputstream  to a given instance of the datatype*/
      virtual void deserialize(const std::istream& /*stream*/, Streamable& /*representation*/) const = 0 ;
  };
  
 /*
  * Serializer is the base class for all Serializers
  */
  template<class T> class Serializer : public AbstractSerializer
  {
  public:
      /** Serialize a data type from a specific instance to an outstream */
      virtual void serialize(const T& /*representation*/, std::ostream& /*stream*/) const = 0;
      /** Deserialize from an inputstream  to a given instance of the datatype*/
      virtual void deserialize(const std::istream& /*stream*/, T& /*representation*/) const = 0;
  };

}

#endif // SERIALIZER_H
