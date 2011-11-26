#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <iostream>

namespace crf
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
      #ifdef DEBUG_INFRASTRUCTURE
        std::cerr << "no serizalizer for " << typeid(T).name() << std::endl; 
      #endif // DEBUG_INFRASTRUCTURE
      // do nothing
    }
    static void deserialize(std::istream& stream, T& representation)
    {
      #ifdef DEBUG_INFRASTRUCTURE
        std::cerr << "no deserizalizer for " << typeid(T).name() << std::endl; 
      #endif // DEBUG_INFRASTRUCTURE
      // do nothing
    }
};

}//crf

#endif // SERIALIZER_H
