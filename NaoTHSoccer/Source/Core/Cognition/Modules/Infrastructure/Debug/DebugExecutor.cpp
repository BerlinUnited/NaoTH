/*
 * File:   DebugExecutor.cpp
 * Author: Heinrich Mellmann
 *
 */

#include "DebugExecutor.h"

DebugExecutor::DebugExecutor()
{
}

void DebugExecutor::execute()
{
  getDebugMessageOut().answers.clear();
  for(std::list<DebugMessageIn::Message>::const_iterator iter = getDebugMessageIn().messages.begin();
      iter != getDebugMessageIn().messages.end(); ++iter)
  {
    std::stringstream answer;
    DebugCommandManager::getInstance().handleCommand(iter->command, iter->arguments, answer);
    getDebugMessageOut().answers.push_back(answer.str());
  }
}//end execute




