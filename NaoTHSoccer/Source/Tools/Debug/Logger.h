/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _LOGFILE_H
#define _LOGFILE_H

#include <set>
#include <map>

#include <Tools/DataStructures/Serializer.h>
#include <Tools/DataStructures/SerializerWrapper.h>

#include <DebugCommunication/DebugCommandExecutor.h>
#include "Tools/Logfile/LogfileManager.h"

class Logger : public DebugCommandExecutor
{
public:
  Logger(const std::string& cmd);
  ~Logger();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string, std::string>& arguments,
    std::ostream &outstream);

  /** 
  * add a representation to be logged
  */
  template<class T> void addRepresentation(const T* representation, std::string name)
  {
    if(representations.find(name) == representations.end())
    {
      representations[name] = new naoth::SerializerWrapper<T>(representation);
    }
  }

  void log(unsigned int frameNum);

  const std::string& getCommand() const { return command;}
  const std::string& getDescription() const { return description;}

protected:
  void handleCommand(const std::string& argName, const std::string& argValue, std::ostream& outstream);
  
private:

  LogfileManager logfileManager;
  typedef std::map<std::string, naoth::VoidSerializer*> RepresentationsMap;
  RepresentationsMap representations;

  //
  std::string command;
  std::string description;
  
  std::set<std::string> activeRepresentations;
  bool activated;
  bool activatedOnce;
};

#endif  /* _LOGFILE_H */

