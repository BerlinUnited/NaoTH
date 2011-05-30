/*
 * File:   ParameterListDebugLoader.cpp
 * Author: thomas
 *
 */
#include "ParameterListDebugLoader.h"

#include <Representations/Infrastructure/Configuration.h>
#include <PlatformInterface/Platform.h>
#include <Tools/Debug/DebugImageDrawings.h>

#include "Cognition/CognitionDebugServer.h"

ParameterListDebugLoader::ParameterListDebugLoader()
{
  // TODO: get the information automatically
  paramlists["CameraSettingsRequest"] = &getCameraSettingsRequest();
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

void ParameterListDebugLoader::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::ostream& outstream)
{
  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;

  std::map<std::string,ParameterList*>::const_iterator itParamList = paramlists.begin();
  while(itParamList != paramlists.end())
  {
    if (command == std::string(itParamList->first).append(":set"))
    {
      // save the old values
      itParamList->second->saveToConfig();
      
      for (std::map<std::string, std::string>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++)
      {
        // update global config
        config.setRawValue(itParamList->first, iter->first, iter->second);
      }
      // load from the changed config
      itParamList->second->loadFromConfig();

      // always success
      outstream<<"set " << itParamList->first << " successfully"<< std::endl;
    }
    else if (command == std::string(itParamList->first).append(":list"))
    {
      std::list<std::string> keys = config.getKeys(itParamList->first);
      for(std::list<std::string>::const_iterator it = keys.begin(); it != keys.end(); it++)
      {
        std::string val = config.getRawValue(itParamList->first, *it);
        outstream << *it << "=" << val << std::endl;
      }
    }
    else if (command == std::string(itParamList->first).append(":store"))
    {
      config.save(naoth::Platform::getInstance().theConfigDirectory);
      outstream << "saved to " << naoth::Platform::getInstance().theConfigDirectory << std::endl;
    }
    itParamList++;
  }
}

ParameterListDebugLoader::~ParameterListDebugLoader()
{
}
