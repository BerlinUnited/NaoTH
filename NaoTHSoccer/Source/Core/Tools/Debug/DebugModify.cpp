// 
// File:   DebugModify.cpp
// Author: Heinrich Mellmann
//
// Created on 19. march 2008, 21:52
//

#include "DebugModify.h"
#include <DebugCommunication/DebugCommandManager.h>

using namespace naoth;

DebugModify::DebugModify()
{
  REGISTER_DEBUG_COMMAND("modify:list", 
    "return the list of registered modifiable values", this);

  REGISTER_DEBUG_COMMAND("modify:set", 
    "set a modifiable value (i.e. the value will be always overwritten by the new one) ", this);

  REGISTER_DEBUG_COMMAND("modify:release", 
    "release a modifiable value (i.e. the value will not be overwritten anymore)", this);
}

DebugModify::~DebugModify(){}


void DebugModify::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream)
{
  if(command == "modify:list")
  {
    std::map<std::string, ModifyValue>::iterator iter = valueMap.begin();
    
    while(iter != valueMap.end())
    {
      outstream << iter->second.modify << ";" << iter->first << "=" << iter->second.value << std::endl;
      iter++;
    }//end while
  }else if(command == "modify:set")
  {
    std::map<std::string, std::string>::const_iterator iter;

    for (iter = arguments.begin(); iter != arguments.end(); ++iter) {
      
      if(valueMap.find(iter->first) != valueMap.end())
      {
        double value = 0.0;
        if(DataConversion::strTo(iter->second,value))
        {
          valueMap.find(iter->first)->second.modify = true;
          valueMap.find(iter->first)->second.value = value;
          outstream << 1 << ";" << iter->first << "=" << value << std::endl;
        }
        else
        {
          outstream << "double value expected " << iter->first << std::endl;
        }
      }
      else
      {
        outstream << "unknown value " << iter->first << std::endl;
      }
    }//end for
  }else if(command == "modify:release")
  {
    std::map<std::string, std::string>::const_iterator iter;

    for (iter = arguments.begin(); iter != arguments.end(); ++iter) {
      if(valueMap.find(iter->first) != valueMap.end())
      {
        valueMap.find(iter->first)->second.modify = false;
        outstream << iter->first << " released" << std::endl;
      }
      else
      {
        outstream << "unknown value " << iter->first << std::endl;
      }
    }//end for
  }
}//end executeDebugCommand

