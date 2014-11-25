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
  ModuleManagerWithDebug(const std::string& name);
  virtual ~ModuleManagerWithDebug();

  virtual void executeDebugCommand(const std::string& command, 
                           const std::map<std::string,std::string>& arguments, 
                           std::ostream& outstream);

private:
  std::string commandModulesList;
  std::string commandModulesSet;
  std::string commandModulesStore;
  std::string commandModulesStopwatch;

  std::string commandRepresentationList;
  std::string commandRepresentationGet;
  std::string commandRepresentationGetbinary;

  
  
  void modulesList(std::ostream& outstream);
  void modulesStore(std::ostream& outstream);
  void modulesSet( std::ostream& outstream,
                   const std::map<std::string,std::string>& arguments);
  void modulesStopwatch(std::ostream& outstream);
  
  void printRepresentation(std::ostream& outstream, const std::string& name, bool binary);
  void representationList(std::ostream& outstream);
};

#endif  /* _ModuleManagerWithDebug_h_ */

