/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _LOGFILE_H
#define	_LOGFILE_H

#include <set>
#include <map>

#include <Tools/DataStructures/Serializer.h>

#include "DebugCommandExecutor.h"
#include "Core/Tools/Logfile/LogfileManager.h"

class Logger : public DebugCommandExecutor
{
public:
  Logger(const std::string& cmd);
  ~Logger();

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string, std::string>& arguments,
    std::stringstream &outstream);

  /** add a representation then it can be logged,
   */
  void addRepresentation(const naoth::AbstractSerializer* representationSerializer, 
    const naoth::Streamable* representation, std::string name);

  void log(unsigned int frameNum);

  const string& getCommand() const { return command;}
  const string& getDescription() const { return description;}

protected:
  void handleCommand(const std::string& argName, const std::string& argValue, std::stringstream& outstream);
  
private:
  LogfileManager < 30 > logfileManager;
  std::string command;
  std::string description;
  std::map<std::string, const naoth::AbstractSerializer*> serializers;
  std::map<std::string, const naoth::Streamable*> streamables;
  std::set<std::string> activeRepresentations;
  bool activated;
  bool activatedOnce;
};

#endif	/* _LOGFILE_H */

