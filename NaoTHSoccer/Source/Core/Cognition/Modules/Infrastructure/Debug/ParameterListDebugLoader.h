/* 
 * File:   ParameterListDebugLoader.h
 * Author: thomas
 *
 */

#ifndef PARAMETERLISTDEBUGLOADER_H
#define	PARAMETERLISTDEBUGLOADER_H

#include <map>

#include <ModuleFramework/Module.h>
#include <DebugCommandExecutor.h>

#include <DebugServer.h>

#include <Representations/Infrastructure/CameraSettings.h>
#include "Core/Representations/FieldInfo.h"

using namespace naoth;

BEGIN_DECLARE_MODULE(ParameterListDebugLoader)
  PROVIDE(CameraSettings)  
  PROVIDE(FieldInfo)
END_DECLARE_MODULE(ParameterListDebugLoader)

class ParameterListDebugLoader : public ParameterListDebugLoaderBase, public DebugCommandExecutor
{
public:
  ParameterListDebugLoader();
  virtual ~ParameterListDebugLoader();

  virtual void execute();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream);

private:
  std::map<std::string, ParameterList*> paramlists;
};

#endif	/* PARAMETERLISTDEBUGLOADER_H */

