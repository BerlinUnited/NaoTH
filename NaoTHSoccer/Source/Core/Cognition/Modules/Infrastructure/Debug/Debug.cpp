/* 
 * File:   Debug.cpp
 * Author: thomas
 * 
 * Created on 11. November 2010, 18:32
 */

#include "Debug.h"


Debug::Debug()
: debugServer(5401)
{
  debugServer.registerCommand("ping",  "gives you a pong", this);
}

void Debug::execute()
{
  debugServer.execute();
}

void Debug::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::stringstream& outstream)
{
  
}

Debug::~Debug()
{
}

