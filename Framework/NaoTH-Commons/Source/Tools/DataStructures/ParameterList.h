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

class ParameterList
{
public:
  ParameterList(const std::string& parentClassName);

  bool& registerParameter(const std::string& name, bool& parameter);
  unsigned int& registerParameter(const std::string& name, unsigned int& parameter);
  int& registerParameter(const std::string& name, int& parameter);
  double& registerParameter(const std::string& name, double& parameter);
  std::string& registerParameter(const std::string& name, std::string& parameter);

  // change some key characters, e.g. []
  std::string covertName(std::string name);

  void loadFromConfig();
  void saveToConfig();

  const std::string& getName() const { return parentClassName; }

private:
  std::string parentClassName;

  std::map<std::string, unsigned int*> unsignedIntParameterReferences;
  std::map<std::string, int*> intParameterReferences;
  std::map<std::string, double*> doubleParameterReferences;
  std::map<std::string, std::string*> stringParameterReferences;
  std::map<std::string, bool*> boolParameterReferences;
};

#define PARAMETER_REGISTER(parameter) registerParameter(covertName(#parameter), parameter)

#endif // __ParameterList_h
