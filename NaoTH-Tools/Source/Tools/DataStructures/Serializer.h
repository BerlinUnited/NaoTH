#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/DataStructures/Singleton.h"
#include "Tools/DataStructures/Streamable.h"

#include <ostream>
#include <istream>

namespace naoth
{  
  
  /**
   * Serializer is the base class for all Serializers implementations
   */
  template<class T>
  class Serializer
  {
    public:
      static void serialize(const T& representation, std::ostream& stream)
      {
        // do nothing
      }
      static void deserialize(const std::istream& stream, T& representation)
      {
        // do nothing
      }
  };
  
  class StreamableSerializer
  {
    public:
      static void serialize(const Streamable& representation, std::ostream& stream)
      {
        // do nothing
      }
      static void deserialize(const std::istream& stream, Streamable& representation)
      {
        // do nothing
      }
  };

}

#endif // SERIALIZER_H
