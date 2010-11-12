/* 
 * File:   Debug.cpp
 * Author: thomas
 * 
 * Created on 11. November 2010, 18:32
 */

#include "Debug.h"

#include "Core/Cognition/CognitionDebugServer.h"

Debug::Debug()
{
  // TODO: use the player and team number for defining the port
  CognitionDebugServer::getInstance().start(5401);

  REGISTER_DEBUG_COMMAND("ping",  "gives you a pong", this);
}

void Debug::execute()
{
  CognitionDebugServer::getInstance().execute();
}

void Debug::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::stringstream& outstream)
{
  if(command == "ping")
  {
    outstream << "pong";
  }
}

Debug::~Debug()
{
}

