/**
 * @file ParameterList.h
 *
 * @author <a href="mailto:aschulz@informatik.hu-berlin.de">Alexander Schulz</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
 * This class is intendes as superclass of all Representations holding parameters that can be changed
 * via the debug-console
 */

#ifndef _ParameterList_h_
#define _ParameterList_h_

#include <map>
#include <sstream>

class ParameterList
{
public:
  ParameterList(const std::string& name) : name(name) {}
  virtual ~ParameterList() {}

  bool& registerParameter(const std::string& name, bool& parameter);
  unsigned int& registerParameter(const std::string& name, unsigned int& parameter);
  int& registerParameter(const std::string& name, int& parameter);
  double& registerParameter(const std::string& name, double& parameter);
  std::string& registerParameter(const std::string& name, std::string& parameter);

  // change some key characters, e.g. []
  static std::string convertName(std::string name);

  void syncWithConfig();
  void saveToConfig();

  const std::string& getName() const { return name; }

private:
  std::string name;

  std::map<std::string, unsigned int*> unsignedIntParameters;
  std::map<std::string, int*> intParameters;
  std::map<std::string, double*> doubleParameters;
  std::map<std::string, std::string*> stringParameters;
  std::map<std::string, bool*> boolParameters;

};

#define PARAMETER_REGISTER(parameter) registerParameter(convertName(#parameter), parameter)

#endif // _ParameterList_h_
