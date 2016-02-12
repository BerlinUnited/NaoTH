/* 
 * File:   ModuleManagerWithDebug.cpp
 * Author: Heinrich Mellmann
 *
 */

#include "ModuleManagerWithDebug.h"
#include <PlatformInterface/Platform.h>

// list the modules and representations on the blackboard
#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>


ModuleManagerWithDebug::ModuleManagerWithDebug(const std::string& name)
{
  std::string prefix = (name.length() > 0)?(name + ":"):"";
  
  // define command strings
  commandModulesList = prefix + "modules:list";
  commandModulesSet = prefix + "modules:set";
  commandModulesStore = prefix + "modules:store";
  commandModulesStopwatch = prefix + "modules:stopwatch";

  commandRepresentationList = prefix + "representation:list";
  commandRepresentationGet = prefix + "representation:get"; // deprecated: change to Print
  commandRepresentationSet = prefix + "representation:set";
  commandRepresentationGetbinary = prefix + "representation:getbinary"; // deprecated: change to Get

  commandDebugRequestSet = prefix + "debugrequest:set";

  REGISTER_DEBUG_COMMAND(commandModulesList,
    "return the list of registered modules with provided and required representations", this);
  REGISTER_DEBUG_COMMAND(commandModulesSet,
    "enables or diables the execution of a module. usage: modules:set [moduleName=on|off]+", this);
  REGISTER_DEBUG_COMMAND(commandModulesStore,
    "store te corrent module configuration to the config file listed in the scheme", this);
  REGISTER_DEBUG_COMMAND(commandModulesStopwatch,
    "output the stopwatches of all modules", this);


  REGISTER_DEBUG_COMMAND(commandRepresentationList, 
    "Stream out the list of all registered representations", this);
  REGISTER_DEBUG_COMMAND(commandRepresentationGet, 
    "Stream out all the representations listet", this);
  REGISTER_DEBUG_COMMAND(commandRepresentationSet, 
    "deserialize listed representations onto the blackboard", this);
  REGISTER_DEBUG_COMMAND(commandRepresentationGetbinary, 
    "Stream out serialized represenation", this);


  REGISTER_DEBUG_COMMAND(prefix + "debugrequest:set",
    "Set a debug request value", this);
}

ModuleManagerWithDebug::~ModuleManagerWithDebug(){}




void ModuleManagerWithDebug::executeDebugCommand(const std::string& command, 
                                    const std::map<std::string,std::string>& arguments, 
                                    std::ostream& outstream)
{
  if (command == commandModulesList) {
    modulesList(outstream);
  } else if( command == commandModulesStore ) {
    modulesStore(outstream);
  } else if (command == commandModulesSet) {
    modulesSet(outstream, arguments);
  } else if(command == commandModulesStopwatch) {
    modulesStopwatch(outstream);
  } else if (command == commandRepresentationList) {
    representationList(outstream);
  } else if (command == commandRepresentationGet)
  {
    std::map<std::string, std::string>::const_iterator iter;
    for (iter = arguments.begin(); iter != arguments.end(); ++iter)
    {
      printRepresentation(outstream, iter->first, false);
    }
  }
  else if(command == commandRepresentationSet)
  {
    DebugCommandManager::ArgumentMap::const_iterator iter;
    for (iter = arguments.begin(); iter != arguments.end(); ++iter)
    {
      setRepresentation(outstream, iter->first, iter->second);
    }
  }
  else if (command == commandRepresentationGetbinary)
  {
    DebugCommandManager::ArgumentMap::const_iterator iter;
    for (iter = arguments.begin(); iter != arguments.end(); ++iter)
    {
      printRepresentation(outstream, iter->first, true);
    }
  }
  else if(command == commandDebugRequestSet)
  {
    DebugCommandManager::ArgumentMap::const_iterator iter_arg = arguments.begin();
    for(;iter_arg != arguments.end(); ++iter_arg)
    {

      // search in the map if we know this command/request
      DebugRequest::RequestMap::iterator iter = getDebugRequest().getRequestMap().find(iter_arg->first);
      if(iter != getDebugRequest().getRequestMap().end())
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
}//end executeDebugCommand


void ModuleManagerWithDebug::modulesList(std::ostream& outstream)
{
  naothmessages::ModuleList msg;
    
  std::list<std::string>::const_iterator iterModule;
  for (iterModule = getExecutionList().begin(); iterModule != getExecutionList().end(); ++iterModule)
  {
    naothmessages::Module* m = msg.add_modules();

    // get the module holder
    AbstractModuleCreator* moduleCreator = getModule(*iterModule);
    ASSERT(moduleCreator != NULL); // should never happen

    // module name
    m->set_name(*iterModule + "|" + moduleCreator->modulePath());
    m->set_active(moduleCreator->isEnabled());
      
    //if(moduleCreator->isEnabled())
    {
      //Module* module = moduleCreator->getModule();
      //ASSERT(module != NULL); // should never happen

      RegistrationInterfaceMap::const_iterator iterRep;
      for (iterRep = moduleCreator->staticRequired().begin();
        iterRep != moduleCreator->staticRequired().end(); ++iterRep)
      {
        m->add_usedrepresentations(iterRep->second->getName());
      }

      for (iterRep = moduleCreator->staticProvided().begin();
        iterRep != moduleCreator->staticProvided().end(); ++iterRep)
      {
        m->add_providedrepresentations(iterRep->second->getName());
      }
    }//end if
  }//end for iterModule

  google::protobuf::io::OstreamOutputStream buf(&outstream);
  msg.SerializeToZeroCopyStream(&buf);
}//end modulesList


void ModuleManagerWithDebug::modulesStore(std::ostream& outstream)
{
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  for(std::list<std::string>::const_iterator name=getExecutionList().begin();
    name != getExecutionList().end(); ++name)
  {
    config.setBool("modules", *name, getModule(*name)->isEnabled());
  }//end for

  // write the config to file
  config.save();
  outstream << "modules saved to private/modules.cfg" << std::endl;
}//end modulesStore


void ModuleManagerWithDebug::modulesSet(
  std::ostream& outstream,
  const std::map<std::string,std::string>& arguments)
{
  std::map<std::string, std::string>::const_iterator iter;
  for (iter = arguments.begin(); iter != arguments.end(); ++iter)
  {
    AbstractModuleCreator* moduleCreator = getModule(iter->first);
    if (moduleCreator != NULL)
    {
      if ((iter->second).compare("on") == 0) {
        moduleCreator->setEnabled(true);
        outstream << "set " << (iter->first) << " on" << std::endl;
      } else if ((iter->second).compare("off") == 0) {
        moduleCreator->setEnabled(false);
        outstream << "set " << (iter->first) << " off" << std::endl;
      } else {
        outstream << "unknown value " << (iter->second) << std::endl;
      }
    } else {
      outstream << "unknown module " << (iter->first) << std::endl;
    }
  }//end for
}//end modulesSet

void ModuleManagerWithDebug::modulesStopwatch(std::ostream& outstream)
{
  naothmessages::Stopwatches all;
  std::list<AbstractModuleCreator*>::const_iterator iterModule;
  for (iterModule = getModuleExecutionList().begin();
       iterModule != getModuleExecutionList().end(); ++iterModule)
  {
    AbstractModuleCreator* moduleCreator = *iterModule;
    const Stopwatch& item = moduleCreator->getStopwatch();
    if(moduleCreator->isEnabled())
    {
      naothmessages::StopwatchItem* s = all.add_stopwatches();
      s->set_name(moduleCreator->getModule()->getName());
      s->set_time(item.lastValue);
    }
  }
  google::protobuf::io::OstreamOutputStream buf(&outstream);
  all.SerializeToZeroCopyStream(&buf);
}

void ModuleManagerWithDebug::setRepresentation(std::ostream &outstream, const std::string& name, const std::string& data)
{
  const BlackBoard& blackBoard = getBlackBoard();
  BlackBoard::Registry::const_iterator iter = blackBoard.getRegistry().find(name);

  if(iter != blackBoard.getRegistry().end())
  {
    Representation& theRepresentation = iter->second->getRepresentation();
      
    std::stringstream ss(data);
    theRepresentation.deserialize(ss);
  } else {
    outstream << "unknown representation" << std::endl;
  }
}//end printRepresentation

void ModuleManagerWithDebug::printRepresentation(std::ostream &outstream, const std::string& name, bool binary)
{
  const BlackBoard& blackBoard = getBlackBoard();
  BlackBoard::Registry::const_iterator iter = blackBoard.getRegistry().find(name);

  if(iter != blackBoard.getRegistry().end())
  {
    const Representation& theRepresentation = iter->second->getRepresentation();
      
    if(binary) {
      theRepresentation.serialize(outstream);
    } else {
      theRepresentation.print(outstream);
    }
  } else {
    outstream << "unknown representation" << std::endl;
  }
}//end printRepresentation

void ModuleManagerWithDebug::representationList(std::ostream &outstream)
{
  const BlackBoard& blackBoard = getBlackBoard();
  BlackBoard::Registry::const_iterator iter;

  for(iter = blackBoard.getRegistry().begin(); iter != blackBoard.getRegistry().end(); ++iter)
  {
    outstream << iter->first << std::endl;
  }
}//end printRepresentationList
