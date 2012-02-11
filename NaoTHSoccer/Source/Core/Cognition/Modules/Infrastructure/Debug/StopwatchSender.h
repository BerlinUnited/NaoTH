/* 
 * File:   StopwatchSender.h
 * Author: thomas
 *
 */

#ifndef STOPWATCHSENDER_H
#define  STOPWATCHSENDER_H

#include <map>

#include <ModuleFramework/Module.h>
#include <DebugCommunication/DebugCommandExecutor.h>

#include <Tools/DataStructures/ParameterList.h>

#include <Representations/Infrastructure/FrameInfo.h>
//#include "Representations/Infrastructure/FieldInfo.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(StopwatchSender)
  REQUIRE(FrameInfo)
END_DECLARE_MODULE(StopwatchSender)

class StopwatchSender : public StopwatchSenderBase, public DebugCommandExecutor
{
public:
  StopwatchSender();
  virtual ~StopwatchSender();

  virtual void execute();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);

private:
  std::map<std::string, ParameterList*> paramlists;
};

#endif  /* STOPWATCHSENDER_H */

