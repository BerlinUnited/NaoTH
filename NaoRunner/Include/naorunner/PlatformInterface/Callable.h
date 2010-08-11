/* 
 * File:   Callable.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:07
 */

#ifndef CALLABLE_H
#define	CALLABLE_H

namespace naorunner
{

  class Callable
  {
  public:

    virtual void init()
    {
    };
    virtual void call() = 0;

  };
}

#endif	/* CALLABLE_H */

