/* 
 * File:   Cognition.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:27
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <naorunner/PlatformInterface/Callable.h>

class Cognition : public Callable
{
public:
  Cognition();
  virtual ~Cognition();

  virtual void call();

private:

};

#endif	/* COGNITION_H */

