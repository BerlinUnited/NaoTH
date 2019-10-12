/**
 * @author <a href="xu:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#include "Logger.h"

Logger::Logger(const std::string& cmd) 
  : command(cmd)
{
  description = command + " on | off | close |activate=<name> | deactivate=<name>";
  activated = false;
  activatedOnce = false;
}

Logger::~Logger()
{
  // cleanup all serializer wrappers
  for (RepresentationsMap::iterator iter = representations.begin(); iter != representations.end(); ++iter)
  {
    delete iter->second;
  }
}

void Logger::executeDebugCommand(const std::string& command, const ArgumentMap& arguments, std::ostream &outstream)
{
  ASSERT(command == this->command);

  for(std::map<std::string, std::string>::const_iterator iter=arguments.begin(); iter!=arguments.end(); ++iter)
  {
    handleCommand(iter->first, iter->second, outstream);
  }
}//end executeDebugCommand

void Logger::handleCommand(const std::string& argName, const std::string& argValue, std::ostream& outstream)
{
  if ("open" == argName) {
    logfileManager.openFile(argValue.c_str());
    activeRepresentations.clear();
    activated = false;

    // list all logable representations
    for(RepresentationsMap::const_iterator iter=representations.begin(); iter!=representations.end(); ++iter){
      outstream << iter->first <<" ";
    }
  }
  else if ("activate" == argName) {
    for(RepresentationsMap::const_iterator iter=representations.begin(); iter!=representations.end(); ++iter)
    {
      if ( argValue == iter->first)
      {
        activeRepresentations.insert(argValue);
        outstream << "activated logging for " << argValue;
        break;
      }
    }//end for
  }
  else if ("deactivate" == argName) {
    for(std::set<std::string>::iterator iter=activeRepresentations.begin(); iter!=activeRepresentations.end(); ++iter)
    {
      if ( argValue == *iter )
      {
        activeRepresentations.erase(argValue);
        outstream << "deactivated logging for " << argValue;
        break;
      }
    }//end for
  }
  else if( "status" == argName) {
    outstream << logfileManager.getWrittentBytesCount();
  }
  else if ("on" == argName) {
    activated = true;
    outstream << "logging on";
  }
  else if ("off" == argName) {
    activated = false;
    logfileManager.flush();
    outstream << "logging off";
  }
  else if ("once" == argName) {
    activatedOnce = true;
    outstream << "log once";
  }
  else if ("close" == argName) {
    logfileManager.closeFile();
    activated = false;
    outstream << "logfile closed";
  }
  else{
    outstream << "Logger Error: unsupport argument: "<<argName;
  }
}//end handleCommand

void Logger::log(unsigned int frameNum)
{
  if (!activated && !activatedOnce) {
    return;
  }

  if(!logfileManager.is_ready()) {
    return;
  }
  
  for (std::set<std::string>::const_iterator iter = activeRepresentations.begin();
    iter != activeRepresentations.end(); ++iter) 
  {
    RepresentationsMap::iterator representation = representations.find(*iter);
    
    if(representation != representations.end())
    { 
      std::ostream& stream = logfileManager.log(frameNum, representation->first);
      representation->second->serialize(stream);
    }
  }

  activatedOnce = false;
}//end log
