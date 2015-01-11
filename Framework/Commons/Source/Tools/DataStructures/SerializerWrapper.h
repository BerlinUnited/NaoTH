#ifndef SERIALIZERWRAPPER_H
#define SERIALIZERWRAPPER_H

#include <iostream>

#include "Serializer.h"

namespace naoth
{


class VoidSerializer
{
public:

  VoidSerializer() {}

  virtual ~VoidSerializer() {}

  virtual void serialize(const void* representation, std::ostream& stream) = 0;
  virtual void deserialize(std::istream& stream, void* representation) = 0;
};

template<class T>
class  SerializerWrapper : public VoidSerializer
{
public:
  SerializerWrapper() {}

  virtual ~SerializerWrapper() {}

  virtual void serialize(const void* representation, std::ostream& stream)
  {
    const T* pointer = (const T*) representation;
    const T& ref = *pointer;
    Serializer<T>::serialize(ref, stream);
  }
  virtual void deserialize(std::istream& stream, void* representation)
  {
    T* pointer = ( T*) representation;
    T& ref = *pointer;
    Serializer<T>::deserialize(stream, ref);
  }
};

}
#endif // SERIALIZERWRAPPER_H
