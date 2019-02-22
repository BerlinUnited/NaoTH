/* 
 * File:   Config.cpp
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include "Config.h"


#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

void naoth::Serializer<Config>::serialize(const Config& object, std::ostream& stream)
{
  naothmessages::Config msg;
  
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  
  std::set<std::string> groups = config.getGroups();
  for(const std::string& group: groups) 
  {
    naothmessages::Config::Group* msg_group = msg.add_groups();
    msg_group->set_name(group);

    std::set<std::string> keys = config.getKeys(group);
    for(const std::string& key: keys) {
      naothmessages::Config::Group::Entry* msg_entry = msg_group->add_entries();
      msg_entry->set_name(key);
      msg_entry->set_value(config.getRawValue(group, key));
    }
  }
  
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<Config>::deserialize(std::istream& stream, Config& object)
{
  naothmessages::Config msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;

  for(int i = 0; i < msg.groups_size(); ++i) 
  {
    const naothmessages::Config::Group& msg_group = msg.groups(i);
    for(int j = 0; j < msg_group.entries_size(); ++j) 
    {
      const naothmessages::Config::Group::Entry& msg_entry = msg_group.entries(j);

      config.setRawValue(msg_group.name(), msg_entry.name(), msg_entry.value());
    }
  }

  config.save();
}
