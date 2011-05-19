// 
// File:   DebugRequest.cpp
// Author: thomas
//
// Created on 19. März 2008, 21:52
//

#include "DebugRequest.h"

#include "Cognition/CognitionDebugServer.h"

#include <Tools/Debug/NaoTHAssert.h>



DebugRequest::DebugRequest() : requestMap()
{
  REGISTER_DEBUG_COMMAND("list_debug_request", 
    "return the debug request which where collected in the internal buffer", this);
}

DebugRequest::~DebugRequest(){}


void DebugRequest::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream)
{
  // search in the map if we know this command/request
  if(requestMap.find(command) != requestMap.end())
  {
    // enable or disable depending on the command
    if(arguments.find("off") != arguments.end())
    {
      requestMap[command] = false;
    }
    else if(arguments.find("on") != arguments.end())
    {
      requestMap[command] = true;
    }
    
    // print result
    outstream << command << (requestMap[command] ? " is on" : " is off");
  }
  else if(command == "list_debug_request")
  {
    std::map<std::string, bool>::const_iterator iter = requestMap.begin();
    
    while(iter != requestMap.end())
    {
      outstream << iter->first << "|" << iter->second << "|" << descriptionMap[iter->first] << endl;
      iter++;
    }
  }
}//end executeDebugCommand


void DebugRequest::registerRequest(const std::string& name, const std::string& description, bool defaultValue = false)
{
  // only add if not known already
  if(requestMap.find(name) == requestMap.end())
  {
    // try to register
    std::string d =  description;
    d = d.append(" (debug request, usage: ");
    d = d.append(name);
    d = d.append(" [on|off|status]");

    if(CognitionDebugServer::getInstance().registerCommand(name, d, this))
    {
      requestMap[name] = defaultValue;
      descriptionMap[name] = description;
    }
  }//end if
}//end registerRequest


bool DebugRequest::isActive(const std::string& name) const
{
  std::map<std::string, bool>::const_iterator iter = requestMap.find(name);
  return iter != requestMap.end() && iter->second;
}//end isActive

const bool& DebugRequest::getValueReference(const std::string& name) const
{
  std::map<std::string, bool>::const_iterator iter = requestMap.find(name);
  if(iter == requestMap.end())
  {
    THROW( "[ERROR] Could not find reference for debug request \"" + name + "\"");
  }
  return iter->second;
}//end getValueReference
