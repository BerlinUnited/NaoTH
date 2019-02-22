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

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// debug
#include <Representations/Debug/Representations.h>
#include <Representations/Debug/Config.h>
#include <Representations/Debug/Stopwatch.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(Cognition)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugPlot)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugRequest)

  // NOTE: these are required here so they are exposed on the BlackBoard for debugging
  REQUIRE(Representations)
  REQUIRE(Config)

  REQUIRE(FrameInfo)
END_DECLARE_MODULE(Cognition)

class Cognition : public naoth::Callable, private CognitionBase, public ModuleManager
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

