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

  virtual void serialize(std::ostream& stream) = 0;
  //virtual void deserialize(std::istream& stream) = 0;
};

template<class T>
class  SerializerWrapper : public VoidSerializer
{
private:
  const T* const representation;

public:
  SerializerWrapper(const T* representation) : representation(representation) {}
  virtual ~SerializerWrapper() {}

  virtual void serialize(std::ostream& stream) {
    Serializer<T>::serialize(*representation, stream);
  }
  /*
  virtual void deserialize(std::istream& stream) {
    Serializer<T>::deserialize(stream, *representation);
  }*/
};

}
#endif // SERIALIZERWRAPPER_H
