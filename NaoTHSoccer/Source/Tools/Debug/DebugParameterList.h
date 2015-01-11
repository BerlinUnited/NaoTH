/**
 * @file DebugParameterList.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 * @brief handle parameter list with debug
 */

#ifndef DEBUGPARAMETERLIST_H
#define DEBUGPARAMETERLIST_H

#include <set>

#include <Tools/DataStructures/Singleton.h>
#include <DebugCommunication/DebugCommandExecutor.h>
#include <Tools/DataStructures/ParameterList.h>

class DebugParameterList: public naoth::Singleton<DebugParameterList>, public DebugCommandExecutor
{
public:
    DebugParameterList();

    virtual void executeDebugCommand(
      const std::string& command, const std::map<std::string, std::string>& arguments,
      std::ostream &outstream);

    void add(ParameterList* pl);

    void remove(ParameterList* pl);

private:
  std::set<ParameterList*> paramlists;
};

#endif // DEBUGPARAMETERLIST_H
