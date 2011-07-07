/**
 * @file DebugParameterList.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * @brief handle parameter list with debug
 */

#include "DebugParameterList.h"
#include "Cognition/DebugCommandServer.h"
#include "PlatformInterface/Platform.h"

using namespace std;

DebugParameterList::DebugParameterList()
{
  REGISTER_DEBUG_COMMAND("ParameterList:list", "list all registered parameters", this);
}

void DebugParameterList::executeDebugCommand(
  const std::string& command, const std::map<std::string, std::string>& arguments,
  std::ostream &outstream)
{
  if ( command == "ParameterList:list" )
  {
    for(set<ParameterList*>::const_iterator itParamList = paramlists.begin();
        itParamList != paramlists.end(); ++itParamList)
    {
      outstream << (*itParamList)->getName() << "\n";
    }
    return;
  }

  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;

  for(set<ParameterList*>::const_iterator itParamList = paramlists.begin();
      itParamList != paramlists.end(); ++itParamList)
  {
    const string& name = (*itParamList)->getName();
    string cmd = "ParameterList:" + name;
    if (command == cmd + ":set")
    {
      for (std::map<std::string, std::string>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++)
      {
        // update global config when value changed
        if ( config.getRawValue(name, iter->first) != iter->second )
          config.setRawValue(name, iter->first, iter->second);
      }
      config.save();
      (*itParamList)->syncWithConfig();

      // always success
      outstream<<"set " << name << " successfully"<< std::endl;
    }
    else if (command == cmd + ":get")
    {
      set<string> keys = config.getKeys(name);
      for(set<string>::const_iterator it = keys.begin(); it != keys.end(); it++)
      {
        string val = config.getRawValue(name, *it);
        outstream << *it << "=" << val << std::endl;
      }
    }
  }
}

void DebugParameterList::add(ParameterList* pl)
{
  paramlists.insert(pl);
  REGISTER_DEBUG_COMMAND("ParameterList:"+pl->getName()+":set", "set parameters of "+pl->getName(), this);
  REGISTER_DEBUG_COMMAND("ParameterList:"+pl->getName()+":get", "get parameters of "+pl->getName(), this);

}

void DebugParameterList::remove(ParameterList* pl)
{
  paramlists.erase(paramlists.find(pl));
}
