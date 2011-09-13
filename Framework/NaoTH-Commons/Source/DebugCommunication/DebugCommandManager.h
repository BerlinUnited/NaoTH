/* 
 * File:   DebugCommandManager.h
 * Author: thomas
 *
 * Created on 12. November 2010, 09:50
 */

#ifndef DebugCommandManager_H
#define  DebugCommandManager_H

#include <Tools/DataStructures/Singleton.h>
#include <Tools/DataStructures/DestructureSentinel.h>
#include "DebugCommandExecutor.h"
#include <string>
#include <iostream>

class DebugCommandManager 
  : 
  public DebugCommandExecutor,
  public naoth::Singleton<DebugCommandManager>, 
  public DestructionListener<DebugCommandExecutor>
{
public:
  DebugCommandManager();
  virtual ~DebugCommandManager();

  /**
   *
   */
  void handleCommand(  
    const std::string& command, 
    const std::map<std::string, std::string>& arguments, 
    std::ostream& answer);

  /**
   * Register a command and a handler for this command.
   * @param commmand The name of the command
   * @param description A description displayed to the user if he uses the help function.
   *                    You might want to describe possible arguments here, too.
   * @param executor The callback handler.
   * @return
   */
  bool registerCommand(
    const std::string& command, 
    const std::string& description,
    DebugCommandExecutor* executor);

  /**
   *
   */
  virtual void objectDestructed(DebugCommandExecutor* object);

  /**
   *
   */
  virtual void executeDebugCommand(const std::string& command,
    const std::map<std::string,std::string>& arguments,
    std::ostream& out);

private:
  /** hash map with all registered callback function  */
  std::map<std::string, DebugCommandExecutor*> executorMap;
  std::map<std::string, std::string> descriptionMap;

};

#undef REGISTER_DEBUG_COMMAND

#ifdef DEBUG
/** register a command only in DEBUG mode */
#define REGISTER_DEBUG_COMMAND(command, description, executor) \
DebugCommandManager::getInstance().registerCommand(command, description, executor);
#else //DEBUG
#define REGISTER_DEBUG_COMMAND(command, description, executor) {}
#endif //DEBUG

#endif  /* DebugCommandManager_H */

