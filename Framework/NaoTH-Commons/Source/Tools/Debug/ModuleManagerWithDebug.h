/* 
 * File:   ModuleManagerWithDebug.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _ModuleManagerWithDebug_h_
#define _ModuleManagerWithDebug_h_

#include <ModuleFramework/ModuleManager.h>
#include <DebugCommunication/DebugCommandExecutor.h>

class ModuleManagerWithDebug: public ModuleManager, public DebugCommandExecutor
{
public:
  ModuleManagerWithDebug();
  virtual ~ModuleManagerWithDebug();

  virtual void executeDebugCommand(const std::string& command, 
                           const std::map<std::string,std::string>& arguments, 
                           std::ostream& outstream);

private:
  void printRepresentation(std::ostream& outstream, const std::string& name, bool binary);

  void printRepresentationList(std::ostream& outstream);
};

#endif  /* _ModuleManagerWithDebug_h_ */

