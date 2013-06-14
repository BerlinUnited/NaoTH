// 
// File:   DebugRequest.cpp
// Author: thomas
//
// Created on 19. march 2008, 21:52
//

#include "DebugRequest.h"

#include <DebugCommunication/DebugCommandManager.h>
#include <Tools/Debug/NaoTHAssert.h>

DebugRequest::DebugRequest() : requestMap()
{
  REGISTER_DEBUG_COMMAND("debug_request:list",
    "return the debug request which where collected in the internal buffer", this);
}

DebugRequest::~DebugRequest(){}


void DebugRequest::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream)
{
  if(command == "debug_request:list")
  {
    std::map<std::string, bool>::const_iterator iter = requestMap.begin();
    
    while(iter != requestMap.end())
    {
      outstream << iter->first << "|" << iter->second << "|" << descriptionMap[iter->first] << std::endl;
      iter++;
    }
  }
  else
  {
    // search in the map if we know this command/request
    std::map<std::string, bool>::iterator iter = requestMap.find(command);
    if(iter != requestMap.end())
    {
      // enable or disable depending on the command
      if(arguments.find("off") != arguments.end()) {
        iter->second = false;
      } else if(arguments.find("on") != arguments.end()) {
        iter->second = true;
      }
      // print result
      outstream << command << (iter->second ? " is on" : " is off");
    }
  }
}//end executeDebugCommand


const bool& DebugRequest::registerRequest(const std::string& name, const std::string& description, bool defaultValue = false)
{
  // only add if not known already
  std::map<std::string, bool>::iterator iter = requestMap.lower_bound(name);
  if(iter == requestMap.end() || iter->first != name)
  {
    std::string d =  description;
    d = d.append(" (debug request, usage: ");
    d = d.append(name);
    d = d.append(" [on|off|status]");

    // there should not be any other command with the same name
    ASSERT(DebugCommandManager::getInstance().registerCommand(name, d, this));
    
    iter = requestMap.insert(iter, std::make_pair(name, defaultValue));
    descriptionMap[name] = description;
  } else {
    // TODO:
    //some places use the fact that a debug request may be registered several times
    //eg, when several instances of ImageProcessing/BlobFinder.cpp are created
    //should we prohibit it?
    //THROW( "[ERROR] there is already a request with the name \"" + name + "\"");
  }

  return iter->second;
}//end registerRequest


bool DebugRequest::isActive(const std::string& name) const
{
  std::map<std::string, bool>::const_iterator iter = requestMap.find(name);
  return iter != requestMap.end() && iter->second;
}//end isActive

const bool& DebugRequest::getValueReference(const std::string& name) const
{
  std::map<std::string, bool>::const_iterator iter = requestMap.find(name);
  if(iter == requestMap.end()) {
    THROW( "[ERROR] Could not find reference for debug request \"" + name + "\"");
  }
  return iter->second;
}//end getValueReference


std::string get_sub_core_path(std::string fullpath)
{
  unsigned p = static_cast<unsigned int> ( fullpath.find("core"));
  if (p < fullpath.size()-5) {
    return fullpath.substr(p+5); // size of "core/"
  } else {
    return fullpath;
  }
}
