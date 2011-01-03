#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <ostream>
#include <istream>

namespace naoth
{

/*
  * Serializer is the base class for all Serializers
  */
  template<class T> class Serializer
  {
  public:
      /** Serialize a data type from a specific instance to an outstream */
      virtual void serialize(const T& /*representation*/, std::ostream& /*stream*/) = 0;
      /** Deserialize from an inputstream  to a given instance of the datatype*/
      virtual void deserialize(const std::istream& /*stream*/, T& /*representation*/) = 0;
  };

}

#endif // SERIALIZER_H
