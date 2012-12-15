/* 
 * File:   Callable.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:07
 */

#ifndef CALLABLE_H
#define CALLABLE_H

namespace naoth
{
  class PlatformInterfaceBase;

  class Callable
  {
  public:

    virtual void init(PlatformInterfaceBase& platformInterface) = 0;

    virtual void call() = 0;

  };
}

#endif  /* CALLABLE_H */

