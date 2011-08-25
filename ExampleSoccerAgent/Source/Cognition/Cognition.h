/**
 * @file Cognition.h
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>
#include <ModuleFramework/ModuleManager.h>
#include <DebugCommunication/DebugCommandExecutor.h>

#include <Tools/Debug/Stopwatch.h>

class Cognition : public naoth::Callable, public ModuleManager, public DebugCommandExecutor
{
public:
  Cognition();
  virtual ~Cognition();

  virtual void call();

  void init(naoth::PlatformInterfaceBase& platformInterface);

private:
  StopwatchItem stopwatch;
};

#endif	/* COGNITION_H */

