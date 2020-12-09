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
