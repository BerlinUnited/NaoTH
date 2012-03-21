#ifndef ENGINEFACTORY_H
#define ENGINEFACTORY_H

#include <Tools/Debug/NaoTHAssert.h>

namespace xabsl
{

/** wraps a getter function providing the time to the xabsl engine */
template<class T>
class EngineFactory
{
private:

  EngineFactory(){}
  static const unsigned int* timestamp;

public:

  static xabsl::Engine* create(ErrorHandler& e, const unsigned int& time_ref)
  {
    timestamp = &time_ref;
    return new xabsl::Engine(e, &getTime);
  }//end createEngine

  static unsigned int getTime()
  {
    ASSERT(timestamp != NULL);
    return *timestamp;
  }//end getTime
};

template<class T> const unsigned int* EngineFactory<T>::timestamp = NULL;

}//end namespace xabsl
#endif // ENGINEFACTORY_H
