#ifndef _SERIALIZER_H
#define _SERIALIZER_H


#include "Tools/Debug/NaoTHAssert.h"

#include <iostream>
#include <typeinfo>

//#define DEBUG_INFRASTRUCTURE

namespace naoth
{  

  /**
   * Serializer is the base class for all Serializers implementations
   */
  template<class T>
  class Serializer
  {
    public:
      static void serialize(const T& /*representation*/, std::ostream& /*stream*/)
      {
        #ifdef DEBUG_INFRASTRUCTURE
          std::cerr << "no serizalizer for " << typeid(T).name() << std::endl; 
        #endif // DEBUG_INFRASTRUCTURE
        // do nothing
      }
      static void deserialize(std::istream& /*stream*/, T& /*representation*/)
      {
        #ifdef DEBUG_INFRASTRUCTURE
          std::cerr << "no deserizalizer for " << typeid(T).name() << std::endl; 
        #endif // DEBUG_INFRASTRUCTURE
        // do nothing
      }
  };

}

#endif // _SERIALIZER_H
