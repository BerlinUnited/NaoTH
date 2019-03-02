// 
// File:   DebugRequest.cpp
// Author: thomas
//
// Created on 19. march 2008, 21:52
//

#include "DebugRequest.h"

#include <Tools/Debug/NaoTHAssert.h>

#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>


const bool& DebugRequest::registerRequest(const std::string& name, const std::string& description, bool defaultValue = false)
{
  // only add if not known already
  std::map<std::string, Request>::iterator iter = requestMap.lower_bound(name);
  if(iter == requestMap.end() || iter->first != name) {
    iter = requestMap.insert(iter, std::make_pair(name, Request(defaultValue, description)));
  } else {
    // TODO:
    //some places use the fact that a debug request may be registered several times
    //eg, when several instances of ImageProcessing/BlobFinder.cpp are created
    //should we prohibit it?
    //THROW( "[ERROR] there is already a request with the name \"" + name + "\"");
    std:: cout << "[DebugRequest] WARNING: multiple registration for the debug request: " << name << std::endl;
  }

  return iter->second.value;
}//end registerRequest


bool DebugRequest::isActive(const std::string& name) const
{
  std::map<std::string, Request>::const_iterator iter = requestMap.find(name);
  return iter != requestMap.end() && iter->second.value;
}

const bool& DebugRequest::getValueReference(const std::string& name) const
{
  std::map<std::string, Request>::const_iterator iter = requestMap.find(name);
  ASSERT_MSG(iter != requestMap.end(), "[ERROR] Could not find reference for debug request \"" + name + "\"");
  return iter->second.value;
}

void naoth::Serializer<DebugRequest>::serialize(const DebugRequest& r, std::ostream& stream)
{
  naothmessages::DebugRequest msg;

  DebugRequest::RequestMap::const_iterator iter = r.getRequestMap().begin();
  for(;iter != r.getRequestMap().end(); ++iter)
  {
    naothmessages::DebugRequest_Item* item = msg.add_requests();
    item->set_name(iter->first);
    item->set_description(iter->second.description);
    item->set_value(iter->second.value);
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<DebugRequest>::deserialize(std::istream& stream, DebugRequest& object)
{
  naothmessages::DebugRequest msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  for(int i = 0; i < msg.requests_size(); ++i) {
    const naothmessages::DebugRequest_Item& v = msg.requests(i);
    object.setValue(v.name(), v.value());
  }
}
