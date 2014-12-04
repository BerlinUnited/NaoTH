/* 
 * File:   DebugCommandManager.cpp
 * Author: thomas
 * 
 * Created on 12. November 2010, 09:50
 */

#include "DebugCommandManager.h"

DebugCommandManager::DebugCommandManager()
{
  registerCommand("help", "list available commands or get the description of a specific command", this);
}


DebugCommandManager::~DebugCommandManager()
{
}

void DebugCommandManager::handleCommand(  
  const std::string& command, 
  const std::map<std::string, std::string>& arguments, 
  std::ostream& answer) const
{
  ExecutorMap::const_iterator iter = executorMap.find(command);
  if (iter != executorMap.end()) {
    iter->second.executor->executeDebugCommand(command, arguments, answer);
  } else {
    answer << "Unknown command \"" << command
      << "\", use \"help\" for a list of available commands" << std::endl;
  }
}//end handleCommand

bool DebugCommandManager::registerCommand(
  const std::string& command, 
  const std::string& description,
  DebugCommandExecutor* executor)
{
  ExecutorMap::iterator iter = executorMap.lower_bound(command);
  if (iter == executorMap.end() || iter->first != command)
  {
    // new command
    executorMap.insert(iter,std::make_pair(command, DebugCommand(executor, description)));
    executor->registerDestructionListener(*this);
    return true;
  }
  return false;
}//end registerCommand


void DebugCommandManager::objectDestructed(DebugCommandExecutor* object)
{
  // search all registered keys of the object
  ExecutorMap::iterator iter = executorMap.begin();
  while(iter != executorMap.end())
  {
    if ((*iter).second.executor == object) {
      executorMap.erase(iter++);
    } else {
      ++iter;
    }
  }
}//end objectDestructed


void DebugCommandManager::executeDebugCommand(
  const std::string& command, 
  const std::map<std::string, std::string>& arguments,
  std::ostream& out)
{
  if (command == "help")
  {
    if (arguments.empty())
    {
      // list all available commands
      out << "Available commands, use \"help <command>\" for a description:\n";
      ExecutorMap::const_iterator iter = executorMap.begin();
      while (iter != executorMap.end())
      {
        out << iter->first;
        ++iter;
        if (iter != executorMap.end()) {
          out << ": " << iter->second.desctiption << "\n";
        }
      }
    }
    else
    {
      std::string firstArg = arguments.begin()->first;
      ExecutorMap::iterator iter = executorMap.find(firstArg);

      if (iter != executorMap.end()) {
        out << firstArg << "\n";
        out << "------------------\n";
        out << iter->second.desctiption;
        out << "\n";
      } else {
        out << "Unknown command \"" << firstArg
          << "\", use \"help\" for a list of available commands";
      }
    }
    out << "\n";
  }
}//end executeDebugCommand
