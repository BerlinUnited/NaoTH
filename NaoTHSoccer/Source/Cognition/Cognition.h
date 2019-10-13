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
#include <Representations/Debug/Stopwatch.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings3D.h"

BEGIN_DECLARE_MODULE(Cognition)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings3D)

  REQUIRE(FrameInfo)
END_DECLARE_MODULE(Cognition)

ModuleManager* getModuleManager(Cognition* c);

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

