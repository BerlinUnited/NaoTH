/**
* @file naoth.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* 
*/

#ifndef _NAOTH_H
#define	_NAOTH_H

#include "PlatformInterface/PlatformInterface.h"

class DummyMotion : public naoth::Callable
{
public:
  virtual void call() {};
  void init(naoth::PlatformInterfaceBase& platformInterface) {};
};

#endif	/* _NAOTH_H */

