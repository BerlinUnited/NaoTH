/*
 * File:   DebugExecutor.cpp
 * Author: Heinrich Mellmann
 *
 */

#include "DebugExecutor.h"

DebugExecutor::DebugExecutor()
{
  // init the debug server
  DebugCommandServer::getInstance();
}

void DebugExecutor::execute()
{
  //std::cout << "DebugExecutor" << std::endl;
 // theDebugServer.execute(); // try reconect

  getDebugMessageOut().answers.clear();
  for(std::list<DebugMessageIn::Message>::const_iterator iter = getDebugMessageIn().messages.begin();
      iter != getDebugMessageIn().messages.end(); ++iter)
  {
    std::stringstream answer;
    DebugCommandServer::getInstance().handleCommand(iter->command, iter->arguments, answer);
    getDebugMessageOut().answers.push_back(answer.str());
  }//end for


}//end execute




