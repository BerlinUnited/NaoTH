/** 
* @file EngineFactory.h
* 
* The class EngineFactory wraps a getter function 
* providing the time to the xabsl engine.
*/

#ifndef ENGINEFACTORY_H
#define ENGINEFACTORY_H

#include <Tools/Debug/NaoTHAssert.h>
#include <Representations/Infrastructure/FrameInfo.h>

namespace xabsl
{


template<class T>
class EngineFactory
{
private:

  EngineFactory() {}
  static const naoth::FrameInfo* frameInfo;

public:

  static xabsl::Engine* create(ErrorHandler& e, const naoth::FrameInfo& frameInfo)
  {
    EngineFactory<T>::frameInfo = &frameInfo;
    return new xabsl::Engine(e, &getTime);
  }

  static unsigned int getTime()
  {
    ASSERT(frameInfo != NULL);
    return (*frameInfo).getTime();
  }
};

template<class T> const naoth::FrameInfo* EngineFactory<T>::frameInfo = NULL;

}//end namespace xabsl
#endif // ENGINEFACTORY_H
