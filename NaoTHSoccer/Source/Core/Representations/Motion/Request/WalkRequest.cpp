/**
* @file WalkRequest.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "WalkRequest.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<WalkRequest>::serialize(const WalkRequest& representation, std::ostream& stream)
{
  naothmessages::WalkRequest message;
  serialize(representation, &message);
  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<WalkRequest>::serialize(const WalkRequest& representation, naothmessages::WalkRequest* msg)
{
  msg->set_coordinate(representation.coordinate);
  DataConversion::toMessage(representation.target, *(msg->mutable_target()));
}

void Serializer<WalkRequest>::deserialize(std::istream& stream, WalkRequest& representation)
{
  naothmessages::WalkRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  deserialize(&message, representation);
}

void Serializer<WalkRequest>::deserialize(const naothmessages::WalkRequest* msg, WalkRequest& representation)
{
  representation.coordinate = static_cast<WalkRequest::Coordinate>(msg->coordinate());
  DataConversion::fromMessage(msg->target(), representation.target);
}
