/*
 * File:   ParameterListDebugLoader.cpp
 * Author: thomas
 *
 */
#include "ParameterListDebugLoader.h"

#include "Core/Cognition/CognitionDebugServer.h"
#include <Tools/Debug/DebugImageDrawings.h>

ParameterListDebugLoader::ParameterListDebugLoader()
{
  // TODO: get the information automatically
  paramlists["CameraSettings"] = &getCameraSettings();
  paramlists["FieldInfo"] = &getFieldInfo();
  
  std::map<std::string,ParameterList*>::const_iterator it = paramlists.begin();
  while(it != paramlists.end())
  {
    REGISTER_DEBUG_COMMAND(std::string(it->first).append(":set"),
                           std::string("set the parameters for ").append(it->first), this);

    REGISTER_DEBUG_COMMAND(std::string(it->first).append(":list"),
                           "get the list of available parameters", this);

    REGISTER_DEBUG_COMMAND(std::string(it->first).append(":store"),
                           "store the configure file according to the path", this);
    it++;
  }
}

void ParameterListDebugLoader::execute()
{

}

void ParameterListDebugLoader::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::stringstream& outstream)
{
//  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
//
//  for(int i=0; i < )
//  if (command == std::string(parentClassName).append(":set"))
//  {
//    // save the old values
//    saveToConfig();
//
//    for (std::map<std::string, std::string>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++)
//    {
//      // update global config
//      config.setRawValue(parentClassName, iter->first, iter->second);
//    }
//    // load from the changed config
//    loadFromConfig();
//
//    // always success
//    outstream<<"set " << parentClassName << " successfully"<< std::endl;
//  }
//  else if (command == std::string(parentClassName).append(":list"))
//  {
//    std::list<std::string> keys = config.getKeys(parentClassName);
//    for(std::list<std::string>::const_iterator it = keys.begin(); it != keys.end(); it++)
//    {
//      std::string val = config.getRawValue(parentClassName, *it);
//      outstream << *it << "=" << val << std::endl;
//    }
//  }
//  else if (command == std::string(parentClassName).append(":store"))
//  {
//    config.save(naoth::Platform::getInstance().theConfigDirectory);
//    outstream << "saved to " << naoth::Platform::getInstance().theConfigDirectory << std::endl;
//  }
}

ParameterListDebugLoader::~ParameterListDebugLoader()
{
}
