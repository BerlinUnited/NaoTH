/**
 * @file DebugParameterList.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * @brief handle parameter list with debug
 */

#ifndef DEBUGPARAMETERLIST_H
#define DEBUGPARAMETERLIST_H

#include <map>

#include <DebugCommunication/DebugCommandExecutor.h>
#include <Tools/DataStructures/ParameterList.h>

class DebugParameterList: public DebugCommandExecutor
{
public:
    DebugParameterList();

    virtual void executeDebugCommand(
      const std::string& command, const ArgumentMap& arguments,
      std::ostream &outstream);

    void add(ParameterList* pl);

    void remove(ParameterList* pl);

private:
  typedef std::map<std::string, ParameterList*> ParameterMap;
  ParameterMap paramlists;
};

#endif // DEBUGPARAMETERLIST_H
