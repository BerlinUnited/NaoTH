/* 
 * File:   StopwatchSender.h
 * Author: thomas
 *
 */

#ifndef STOPWATCHSENDER_H
#define	STOPWATCHSENDER_H

#include <map>

#include <ModuleFramework/Module.h>
#include <DebugCommandExecutor.h>

#include <DebugServer.h>

#include <Representations/Infrastructure/CameraSettings.h>
#include "Core/Representations/FieldInfo.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(StopwatchSender)
  
END_DECLARE_MODULE(StopwatchSender)

class StopwatchSender : public StopwatchSenderBase, public DebugCommandExecutor
{
public:
  StopwatchSender();
  virtual ~StopwatchSender();

  virtual void execute();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream);

private:
  std::map<std::string, ParameterList*> paramlists;
};

#endif	/* STOPWATCHSENDER_H */

