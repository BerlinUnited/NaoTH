/**
 * @file Cognition.h
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef _Cognition_h_
#define	_Cognition_h_

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

  void init(naoth::ProcessInterface& platformInterface, const naoth::PlatformBase& platform);

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);

private:
  Stopwatch stopwatch;
};

#endif	/* _Cognition_h_ */

