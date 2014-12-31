// 
// File:   DebugCommandExecutor.h
// Author: thomas
//
// Created on 8. march 2008, 11:52
//

#ifndef _DEBUGCOMMANDEXECUTOR_H
#define _DEBUGCOMMANDEXECUTOR_H

#include <map>
#include <ostream>

#include <Tools/DataStructures/DestructureSentinel.h>

class DebugCommandExecutor : public DestructionSentinel<DebugCommandExecutor>
{
public:
  
  typedef std::map<std::string,std::string> ArgumentMap;

  /**
   * Executes a specific command and returns the result as string.
   */
  virtual void executeDebugCommand(
    const std::string& command, const ArgumentMap& arguments,
    std::ostream &outstream) = 0;
  
  virtual ~DebugCommandExecutor() {}
};


#endif  /* _DEBUGCOMMANDEXECUTOR_H */

