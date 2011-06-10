/**
* @file DummpyCallable.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* 
*/

#ifndef _DUMMPY_CALLABLE_H_
#define	_DUMMPY_CALLABLE_H_

#include "PlatformInterface/PlatformInterface.h"

class DummyCallable : public naoth::Callable
{
public:
  virtual void call() {};
  void init(naoth::PlatformInterfaceBase& platformInterface) {};
};

#endif	/* _DUMMPY_CALLABLE_H_ */


