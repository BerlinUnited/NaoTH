#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <iostream>

namespace crf
{  

/**
  * Serializer is the base class for all Serializers implementations
  */
template<class R>
class Serializer
{
  public:
    template<class T>
    static void serialize(const R& representation, T& target)
    {
      #ifdef DEBUG_INFRASTRUCTURE
        std::cerr << "no serizalizer for " << typeid(T).name() << std::endl; 
      #endif // DEBUG_INFRASTRUCTURE
      // do nothing
    }

    template<class S>
    static void deserialize(S& source, R& representation)
    {
      #ifdef DEBUG_INFRASTRUCTURE
        std::cerr << "no deserizalizer for " << typeid(T).name() << std::endl; 
      #endif // DEBUG_INFRASTRUCTURE
      // do nothing
    }
};

}//crf



#endif // SERIALIZER_H
