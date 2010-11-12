/**
 * @file ParameterList.h
 *
 * @author <a href="mailto:aschulz@informatik.hu-berlin.de">Alexander Schulz</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
 * This class is intendes as superclass of all Representations holding parameters that can be changed
 * via the debug-console
 */

#ifndef __ParameterList_h
#define __ParameterList_h

#include <map>
#include <sstream>
#include <DebugCommandExecutor.h>

class ParameterList : public DebugCommandExecutor
{
public:
  ParameterList(const std::string& parentClassName);

  bool& registerParameter(const std::string& name, bool& parameter);
  unsigned int& registerParameter(const std::string& name, unsigned int& parameter);
  int& registerParameter(const std::string& name, int& parameter);
  double& registerParameter(const std::string& name, double& parameter);
  std::string& registerParameter(const std::string& name, std::string& parameter);

  
  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string, std::string>& arguments,
    std::stringstream &outstream);

  virtual void loadFromConfig();
  virtual void saveToConfig();

private:
  std::string parentClassName;

  std::map<std::string, unsigned int*> unsignedIntParameterReferences;
  std::map<std::string, int*> intParameterReferences;
  std::map<std::string, double*> doubleParameterReferences;
  std::map<std::string, std::string*> stringParameterReferences;
  std::map<std::string, bool*> boolParameterReferences;
};

#define PARAMETER_REGISTER(parameter) registerParameter(#parameter, parameter)

#endif // __ParameterList_h
