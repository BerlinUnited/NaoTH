// 
// File:   DebugModify.cpp
// Author: Heinrich Mellmann
//
// Created on 19. march 2008, 21:52
//

#include "DebugModify.h"
#include <DebugCommunication/DebugCommandManager.h>

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;


void DebugModify::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream)
{
  if(command == "modify:list")
  {
    std::map<std::string, ModifyValue>::iterator iter = valueMap.begin();
    
    while(iter != valueMap.end())
    {
      outstream << std::noboolalpha << iter->second.modify << ";" << iter->first << "=" << iter->second.value << std::endl;
      ++iter;
    }
  }
  else if(command == "modify:set")
  {
    std::map<std::string, std::string>::const_iterator iter;

    for (iter = arguments.begin(); iter != arguments.end(); ++iter) {
      
      if(valueMap.find(iter->first) != valueMap.end())
      {
        double value = 0.0;
        if(DataConversion::strTo(iter->second,value))
        {
          valueMap.find(iter->first)->second.modify = true;
          valueMap.find(iter->first)->second.value = value;
          outstream << 1 << ";" << iter->first << "=" << value << std::endl;
        }
        else
        {
          outstream << "double value expected " << iter->first << std::endl;
        }
      }
      else
      {
        outstream << "unknown value " << iter->first << std::endl;
      }
    }//end for
  }
  else if(command == "modify:release")
  {
    std::map<std::string, std::string>::const_iterator iter;

    for (iter = arguments.begin(); iter != arguments.end(); ++iter) {
      if(valueMap.find(iter->first) != valueMap.end())
      {
        valueMap.find(iter->first)->second.modify = false;
        outstream << iter->first << " released" << std::endl;
      }
      else
      {
        outstream << "unknown value " << iter->first << std::endl;
      }
    }//end for
  }
}//end executeDebugCommand


void Serializer<DebugModify>::serialize(const DebugModify& representation, std::ostream& stream)
{
  naothmessages::DebugModify m;

  for (const auto& e: representation.getEntries()) {
    naothmessages::DebugModify_ModifyValue* v = m.add_valuemap();
    v->set_name(e.first);
    v->set_modify(e.second.modify);
    v->set_value(e.second.value);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  m.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<DebugModify>::deserialize(std::istream& stream, DebugModify& representation)
{
  naothmessages::DebugModify m;
  google::protobuf::io::IstreamInputStream buf(&stream);
  m.ParseFromZeroCopyStream(&buf);

  for(int i = 0; i < m.valuemap_size(); ++i) {
    const naothmessages::DebugModify_ModifyValue& v = m.valuemap(i);
    DebugModify::ModifyValue& r = representation.getValueReference(v.name());
    if(v.has_modify()) {
      r.modify = v.modify();
    }
    if(v.has_value()) {
      r.value = v.value();
    }
  }

}//end deserialize
