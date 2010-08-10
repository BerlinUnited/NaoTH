/**
* @file Config.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class Config
*/

#include <sstream>
#include <map>

#include "Tools/Config/Config.h"

bool Config::get(const std::string& name) const
{
  return stringParameter.find(name) != stringParameter.end();
}

void Config::toStream(ostream& stream, const std::string& separation) const
{
  std::map<std::string, Parameter<std::string> >::const_iterator iter;
  for(iter = stringParameter.begin(); iter != stringParameter.end(); iter++)
  {
      stream << iter->first << " = " << iter->second.Value << separation;
  }//end for
}//end toStream

ostream& operator<<(ostream& stream, const Config& config)
{
    config.toStream(stream);
    return stream;
}

