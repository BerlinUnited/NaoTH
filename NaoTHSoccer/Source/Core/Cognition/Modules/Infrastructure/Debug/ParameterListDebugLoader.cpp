/*
 * File:   ParameterListDebugLoader.cpp
 * Author: thomas
 *
 */
#include "ParameterListDebugLoader.h"

#include <Representations/Infrastructure/Configuration.h>
#include <PlatformInterface/Platform.h>
#include <Tools/Debug/DebugImageDrawings.h>

#include "Cognition/DebugCommandServer.h"

ParameterListDebugLoader::ParameterListDebugLoader()
{
  // TODO: get the information automatically
  paramlists["CameraSettings"] = &getCameraSettingsRequest();
  paramlists["FieldInfo"] = &getFieldInfo();
  
  std::map<std::string,ParameterList*>::const_iterator it = paramlists.begin();
  while(it != paramlists.end())
  {
    REGISTER_DEBUG_COMMAND(std::string(it->first).append(":set"),
                           std::string("set and save the parameters for ").append(it->first), this);

    REGISTER_DEBUG_COMMAND(std::string(it->first).append(":list"),
                           "get the list of available parameters", this);
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
      for (std::map<std::string, std::string>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++)
      {
        // update global config when value changed
        if ( config.getRawValue(itParamList->first, iter->first) != iter->second )
          config.setRawValue(itParamList->first, iter->first, iter->second);
      }
      config.save();
      // load from the changed config
      itParamList->second->loadFromConfig();

      // always success
      outstream<<"set " << itParamList->first << " successfully"<< std::endl;
    }
    else if (command == std::string(itParamList->first).append(":list"))
    {
      set<string> keys = config.getKeys(itParamList->first);
      for(set<string>::const_iterator it = keys.begin(); it != keys.end(); it++)
      {
        string val = config.getRawValue(itParamList->first, *it);
        outstream << *it << "=" << val << std::endl;
      }
    }
    itParamList++;
  }
}

ParameterListDebugLoader::~ParameterListDebugLoader()
{
}
