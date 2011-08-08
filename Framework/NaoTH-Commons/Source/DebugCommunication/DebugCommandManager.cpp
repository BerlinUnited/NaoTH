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
  std::ostream& answer)
{
  if (executorMap.find(command) != executorMap.end())
  {
    executorMap[command]->executeDebugCommand(command, arguments, answer);
  } else
  {
    answer << "Unknown command \"" << command
      << "\", use \"help\" for a list of available commands" << std::endl;
  }
}//end handleCommand

bool DebugCommandManager::registerCommand(
  const std::string& command, 
  const std::string& description,
  DebugCommandExecutor* executor)
{
  if (executorMap.find(command) == executorMap.end())
  {
    // new command
    executorMap[command] = executor;
    descriptionMap[command] = description;
    executor->registerDestructionListener(*this);
    return true;
  }
  return false;
}//end registerCommand


void DebugCommandManager::objectDestructed(DebugCommandExecutor* object)
{
  std::list<std::string> registeredKeys;

  // search all registered keys of the object
  std::map<std::string, DebugCommandExecutor*>::const_iterator iter;
  for (iter = executorMap.begin(); iter != executorMap.end(); iter++)
  {
    if ((*iter).second == object)
    {
      registeredKeys.push_back((*iter).first);
    }//end if
  }//end for

  // unregister all found commands
  std::list<std::string>::const_iterator iter_key;
  for (iter_key = registeredKeys.begin(); iter_key != registeredKeys.end(); iter_key++)
  {
    executorMap.erase(*iter_key);

    std::cout << "unregistering command " << (*iter_key) << std::endl;
  }//end for
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
      std::map<std::string, std::string>::const_iterator iter = descriptionMap.begin();
      while (iter != descriptionMap.end())
      {
        out << iter->first;
        iter++;
        if (iter != descriptionMap.end())
        {
          out << ": " << iter->second << "\n";
        }
      }
    }
    else
    {
      std::string firstArg = arguments.begin()->first;
      if (descriptionMap.find(firstArg) != descriptionMap.end())
      {
        out << firstArg << "\n";
        out << "------------------\n";
        out << descriptionMap[firstArg];
        out << "\n";
      } else
      {
        out << "Unknown command \"" << firstArg
          << "\", use \"help\" for a list of available commands";
      }

    }
    out << "\n";
  }
}//end executeDebugCommand