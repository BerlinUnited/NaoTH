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

  // TODO: use the player and team number for defining the port
  theDebugServer.start(5401, true);
}

void DebugExecutor::execute()
{
  //getDebugMessageIn().messages.clear();

  // important both are needed!!!
  //getDebugMessageOut().answers.clear();

  //std::cout << "DebugExecutor" << std::endl;
 // theDebugServer.execute(); // try reconect

  theDebugServer.getDebugMessageIn(getDebugMessageIn());

  while(!getDebugMessageIn().messages.empty())
  {
    const DebugMessageIn::Message& message = getDebugMessageIn().messages.front();
    std::stringstream answer;
    DebugCommandServer::getInstance().handleCommand(message.command, message.arguments, answer);
    getDebugMessageOut().answers.push(answer.str());
    getDebugMessageIn().messages.pop();
  }//end for

  if(getDebugMessageOut().answers.size() > 0)
    theDebugServer.setDebugMessageOut(getDebugMessageOut());
}//end execute




