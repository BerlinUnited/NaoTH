/**
* @file DummpyCallable.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* 
*/

#ifndef _DUMMPY_CALLABLE_H_
#define	_DUMMPY_CALLABLE_H_

#include "PlatformInterface/ProcessInterface.h"
#include "PlatformInterface/PlatformBase.h"

class DummyCallable : public naoth::Callable
{
public:
  void call() {}
  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform) {};
};

#endif	/* _DUMMPY_CALLABLE_H_ */


