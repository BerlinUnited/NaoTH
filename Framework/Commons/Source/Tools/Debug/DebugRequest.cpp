// 
// File:   DebugRequest.cpp
// Author: thomas
//
// Created on 19. march 2008, 21:52
//

#include "DebugRequest.h"

#include <DebugCommunication/DebugCommandManager.h>
#include <Tools/Debug/NaoTHAssert.h>

DebugRequest::DebugRequest()
{
}

DebugRequest::~DebugRequest(){}


void DebugRequest::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream)
{
  if(command == "debugrequest:set")
  {
    std::map<std::string,std::string>::const_iterator iter_arg = arguments.begin();
    for(;iter_arg != arguments.end(); ++iter_arg)
    {

      // search in the map if we know this command/request
      std::map<std::string, Request>::iterator iter = requestMap.find(iter_arg->first);
      if(iter != requestMap.end())
      {
        // enable or disable depending on the command
        if(iter_arg->second == "off") {
          iter->second.value = false;
        } else if(iter_arg->second == "on") {
          iter->second.value = true;
        }
        // print result
        outstream << command << (iter->second.value ? " is on" : " is off");
      }

    }
  }
  else
  {
    THROW( "[DebugRequest] unknown command" << command );
  }
}//end executeDebugCommand


const bool& DebugRequest::registerRequest(const std::string& name, const std::string& description, bool defaultValue = false)
{
  // only add if not known already
  std::map<std::string, Request>::iterator iter = requestMap.lower_bound(name);
  if(iter == requestMap.end() || iter->first != name)
  {
    std::string d =  description;
    d = d.append(" (debug request, usage: ");
    d = d.append(name);
    d = d.append(" [on|off|status]");
    
    iter = requestMap.insert(iter, std::make_pair(name, Request(defaultValue)));
    iter->second.description = description;
  } else {
    // TODO:
    //some places use the fact that a debug request may be registered several times
    //eg, when several instances of ImageProcessing/BlobFinder.cpp are created
    //should we prohibit it?
    //THROW( "[ERROR] there is already a request with the name \"" + name + "\"");
  }

  return iter->second.value;
}//end registerRequest


bool DebugRequest::isActive(const std::string& name) const
{
  std::map<std::string, Request>::const_iterator iter = requestMap.find(name);
  return iter != requestMap.end() && iter->second.value;
}

const bool& DebugRequest::getValueReference(const std::string& name) const
{
  std::map<std::string, Request>::const_iterator iter = requestMap.find(name);
  if(iter == requestMap.end()) {
    THROW( "[ERROR] Could not find reference for debug request \"" + name + "\"");
  }
  return iter->second.value;
}


std::string get_sub_core_path(std::string fullpath)
{
  unsigned p = static_cast<unsigned int> ( fullpath.find("core"));
  if (p < fullpath.size()-5) {
    return fullpath.substr(p+5); // size of "core/"
  } else {
    return fullpath;
  }
}

void naoth::Serializer<DebugRequest>::serialize(const DebugRequest& r, std::ostream& stream)
{
  DebugRequest::RequestMap::const_iterator iter = r.getRequestMap().begin();
    
  while(iter != r.getRequestMap().end())
  {
    stream << iter->first << "|" << iter->second.value << "|" << iter->second.description << std::endl;
    ++iter;
  }
}

void naoth::Serializer<DebugRequest>::deserialize(std::istream& /*stream*/, DebugRequest& /*object*/)
{
}
