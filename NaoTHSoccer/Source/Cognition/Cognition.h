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

// debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugDrawings.h"

BEGIN_DECLARE_MODULE(Cognition)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugDrawings)
END_DECLARE_MODULE(Cognition)

class Cognition : public naoth::Callable, private CognitionBase, public ModuleManagerWithDebug
{
public:
  Cognition();
  virtual ~Cognition();

  virtual void call();

  // TODO: unify with Callable
  void execute() {}

  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);

private:
  Stopwatch stopwatch;
  
};

#endif  /* _Cognition_h_ */

