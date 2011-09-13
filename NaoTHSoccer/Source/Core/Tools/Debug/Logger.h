/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _LOGFILE_H
#define  _LOGFILE_H

#include <set>
#include <map>

#include <Tools/DataStructures/Serializer.h>
#include <Tools/DataStructures/SerializerWrapper.h>
#include <Tools/DataStructures/Streamable.h>

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

  /** add a representation then it can be logged,
   */
  template<class T> void addRepresentation(const T* representation, std::string name)
  {
    if(streamables.find(name) == streamables.end())
    {
      streamables[name] = representation;
      serializers[name] = new naoth::SerializerWrapper<T>();
    }
  }//end addRepresentation

  void log(unsigned int frameNum);

  const string& getCommand() const { return command;}
  const string& getDescription() const { return description;}

protected:
  void handleCommand(const std::string& argName, const std::string& argValue, std::ostream& outstream);
  
private:

  LogfileManager < 30 > logfileManager;
  std::string command;
  std::string description;
  std::map<std::string, const void*> streamables;
  std::map<std::string, naoth::VoidSerializer*> serializers;
  
  std::set<std::string> activeRepresentations;
  bool activated;
  bool activatedOnce;
};

#endif  /* _LOGFILE_H */

