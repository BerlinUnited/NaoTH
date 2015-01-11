/**
 * @file DebugParameterList.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * @brief handle parameter list with debug
 */

#include "DebugParameterList.h"
#include <DebugCommunication/DebugCommandManager.h>
#include "PlatformInterface/Platform.h"

using namespace std;

DebugParameterList::DebugParameterList()
{
}

void DebugParameterList::executeDebugCommand(
  const std::string& command, const ArgumentMap& arguments,
  std::ostream &outstream)
{
  if ( command == "ParameterList:list" )
  {
    for(ParameterMap::const_iterator itParamList = paramlists.begin();
        itParamList != paramlists.end(); ++itParamList)
    {
      outstream << itParamList->second->getName() << "\n";
    }
  } 
  else if( command == "ParameterList:get" )
  {
    ArgumentMap::const_iterator iter = arguments.find("<name>");
    if(iter != arguments.end()) 
    {
      const std::string& name = iter->second;

      naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
      set<string> keys = config.getKeys(name);

      for(set<string>::const_iterator it = keys.begin(); it != keys.end(); ++it)
      {
        string val = config.getRawValue(name, *it);
        outstream << *it << "=" << val << std::endl;
      }
    }
  }
  else if( command == "ParameterList:set" )
  {
    ArgumentMap::const_iterator iter = arguments.find("<name>");
    if(iter != arguments.end()) 
    {
      const std::string& name = iter->second;
      
      naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
      
      for (ArgumentMap::const_iterator iter = arguments.begin(); iter != arguments.end(); ++iter)
      {
        // update global config
        if (  config.hasKey(name, iter->first) ) {
          config.setRawValue(name, iter->first, iter->second);
        }
      }
      config.save();

      ParameterMap::iterator pIter = paramlists.find(name);
      if(pIter == paramlists.end() ) {
        outstream << "[error] " << name << " not registered" << std::endl;
      } else {
        pIter->second->syncWithConfig();
        outstream << "[info] set " << name << " successfully" << std::endl;
      }
    }
  }
}

void DebugParameterList::add(ParameterList* pl)
{
  paramlists[pl->getName()] = pl;
}

void DebugParameterList::remove(ParameterList* pl)
{
  paramlists.erase(paramlists.find(pl->getName()));
}
