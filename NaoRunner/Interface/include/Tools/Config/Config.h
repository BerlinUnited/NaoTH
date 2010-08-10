/**
* @file Config.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Config
*/

#ifndef __Config_h_
#define __Config_h_

#include <string>
#include <map>
#include <cstdlib>
#include "Tools/DataConversion.h"

using namespace std;

class Config {

private:

  template <class T> class Parameter {
  public:
    Parameter(){};
    Parameter(T param): Value(param) {};
    T Value;
  };

  std::map<std::string, Parameter<std::string> > stringParameter;

public:
  Config(){};
  ~Config(){};

  template<typename T>
  void set(const std::string& name, T param)
  {
    Parameter<std::string> tempParameter(DataConversion::toStr<T>(param));
    stringParameter[name] = tempParameter;
  }

  template<typename T>
  bool get(const std::string& name, T& value) const {
    std::map<std::string, Parameter<std::string> >::const_iterator iter = stringParameter.find(name);
    if ( stringParameter.end() == iter )
      return false;

    return DataConversion::strTo((iter->second).Value, value);
  }

  bool get(const std::string& name) const;

  void toStream(ostream& stream, const std::string& separation=";\n") const;

  friend ostream& operator<<(ostream& stream, const Config& config);
};

ostream& operator<<(ostream& stream, const Config& config);

#endif //__Config_h_
