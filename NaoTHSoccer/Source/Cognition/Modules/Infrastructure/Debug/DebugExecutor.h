/* 
 * File:   DebugExecutor.h
 * Author: Heinrich Mellmann
 */

#ifndef _DebugExecutor_h_
#define _DebugExecutor_h_

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/DebugMessage.h>
#include <DebugCommunication/DebugCommandManager.h>

BEGIN_DECLARE_MODULE(DebugExecutor)
  REQUIRE(DebugMessageInCognition)
  PROVIDE(DebugMessageOut)
  REQUIRE(DebugCommandManager)
END_DECLARE_MODULE(DebugExecutor)

class DebugExecutor : public DebugExecutorBase
{
public:
  DebugExecutor();
  virtual ~DebugExecutor(){}

  virtual void execute();

private:
  std::stringstream answer_stream;

};

#endif  /* _DebugExecutor_h_ */

