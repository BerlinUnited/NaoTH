/* 
 * File:   DebugCommandManager.h
 * Author: thomas
 *
 * Created on 12. November 2010, 09:50
 */

#ifndef DebugCommandManager_H
#define DebugCommandManager_H

#include <Tools/DataStructures/DestructureSentinel.h>
#include "DebugCommandExecutor.h"
#include <string>
#include <iostream>

class DebugCommandManager 
  : 
  public DebugCommandExecutor,
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
    std::ostream& answer) const;

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
  class DebugCommand
  {
  public:
    DebugCommand():executor(NULL){}
    DebugCommand(DebugCommandExecutor* executor, const std::string& desctiption)
      :
      executor(executor),
      desctiption(desctiption)
    {}
    DebugCommandExecutor* executor;
    std::string desctiption;
  };

  /** hash map with all registered callback function  */
  typedef std::map<std::string, DebugCommand> ExecutorMap;
  ExecutorMap executorMap;

};

#undef REGISTER_DEBUG_COMMAND

#ifdef DEBUG
/** register a command only in DEBUG mode */
#define REGISTER_DEBUG_COMMAND(command, description, executor) \
  getDebugCommandManager().registerCommand(command, description, executor);
#else //DEBUG
#define REGISTER_DEBUG_COMMAND(command, description, executor) {}
#endif //DEBUG

#endif  /* DebugCommandManager_H */

