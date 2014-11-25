/**
 * @file Cognition.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef _Cognition_h_
#define _Cognition_h_

#include <iostream>

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>

#include <Tools/Debug/ModuleManagerWithDebug.h>

class Cognition : public naoth::Callable, public ModuleManagerWithDebug
{
public:
  Cognition();
  virtual ~Cognition();

  virtual void call();

  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);

private:
  Stopwatch stopwatch;
  
};

#endif  /* _Cognition_h_ */

