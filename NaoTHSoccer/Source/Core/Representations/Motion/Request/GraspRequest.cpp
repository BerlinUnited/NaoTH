/**
* @file GraspRequest.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#include "GraspRequest.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<GraspRequest>::serialize(const GraspRequest& representation, std::ostream& stream)
{
  naothmessages::GraspRequest message;
  serialize(representation, &message);
  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<GraspRequest>::serialize(const GraspRequest& representation, naothmessages::GraspRequest* msg)
{
  DataConversion::toMessage(representation.graspingPoint, *(msg->mutable_graspingpoint()));
  msg->set_graspingstate(representation.graspingState);
  msg->set_graspdiststate(representation.graspDistState);
  msg->set_graspstiffstate(representation.graspStiffState);
}

void Serializer<GraspRequest>::deserialize(std::istream& stream, GraspRequest& representation)
{
  naothmessages::GraspRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  deserialize(&message, representation);
}

void Serializer<GraspRequest>::deserialize(const naothmessages::GraspRequest* msg, GraspRequest& representation)
{
  DataConversion::fromMessage(msg->graspingpoint(), representation.graspingPoint);
  representation.graspingState = static_cast<GraspRequest::GraspingState>(msg->graspingstate());
  representation.graspDistState = static_cast<GraspRequest::GraspDistState>(msg->graspdiststate());
  representation.graspStiffState = static_cast<GraspRequest::GraspStiffState>(msg->graspstiffstate());
}
