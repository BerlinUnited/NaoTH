/* 
 * File:   DebugExecutor.h
 * Author: Heinrich Mellmann
 */

#ifndef __DebugExecutor_h__
#define	__DebugExecutor_h__

#include <ModuleFramework/Module.h>
#include "Representations/Infrastructure/DebugMessage.h"
#include <DebugCommunication/DebugServer.h>
#include "Cognition/DebugCommandServer.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(DebugExecutor)
  PROVIDE(DebugMessageIn)
  PROVIDE(DebugMessageOut)
END_DECLARE_MODULE(DebugExecutor)

class DebugExecutor : public DebugExecutorBase
{
public:
  DebugExecutor();
  virtual ~DebugExecutor(){};

  virtual void execute();

private:
  DebugServer theDebugServer;
};

#endif	/* __DebugExecutor_h__ */

