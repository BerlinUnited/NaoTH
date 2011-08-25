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

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);

private:
  StopwatchItem stopwatch;
};

#endif	/* COGNITION_H */

