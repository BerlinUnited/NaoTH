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
  getDebugMessageIn().messages.clear();

  // important both are needed!!!
  getDebugMessageOut().answer.clear();
  getDebugMessageOut().answer.str("");

 // theDebugServer.execute(); // try reconect

  theDebugServer.getDebugMessageIn(getDebugMessageIn());

  for(unsigned int i = 0; i < getDebugMessageIn().messages.size(); i++)
  {
    const DebugMessageIn::Message& message = getDebugMessageIn().messages[i];
    DebugCommandServer::getInstance().handleCommand(message.command, message.arguments, getDebugMessageOut().answer);
  }//end for

  if(getDebugMessageIn().messages.size() > 0)
    theDebugServer.setDebugMessageOut(getDebugMessageOut());
}//end execute




