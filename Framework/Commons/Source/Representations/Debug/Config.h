/* 
 * File:   Config.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 * This is a wrapper class providing access to the Configuration
 *   naoth::Platform::getInstance().theConfiguration;
 */

#ifndef _Config_H_
#define _Config_H_

#include <PlatformInterface/Platform.h>
#include <string>

#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Printable.h"

class Config: public naoth::Printable
{
public:
  virtual void print(std::ostream& stream) const
  {
    naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
    std::set<std::string> groups = config.getGroups();
    for(const std::string& group: groups) 
    {
      stream << "[" << group << "]" << std::endl;
      std::set<std::string> keys = config.getKeys(group);
      for(const std::string& key: keys) {
        stream << "    " << key << "=" << config.getRawValue(group, key) << std::endl;
      }
    }
  }
};

namespace naoth
{
template<>
class Serializer<Config>
{
  public:
  static void serialize(const Config& object, std::ostream& stream);

  // we don't need that
  static void deserialize(std::istream& stream, Config& object);
};
}

#endif  /* _Config_H_ */

