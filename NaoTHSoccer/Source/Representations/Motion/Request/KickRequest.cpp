/**
* @file KickRequest.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#include "KickRequest.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<KickRequest>::serialize(const KickRequest& representation, std::ostream& stream)
{
  naothmessages::KickRequest message;
  serialize(representation, &message);
  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<KickRequest>::serialize(const KickRequest& representation, naothmessages::KickRequest* msg)
{
  DataConversion::toMessage(representation.kickPoint, *(msg->mutable_kickpoint()));
  msg->set_kickdirection(representation.kickDirection);
  msg->set_kickfoot(representation.kickFoot);
  msg->set_finishkick(representation.finishKick);
}

void Serializer<KickRequest>::deserialize(std::istream& stream, KickRequest& representation)
{
  naothmessages::KickRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  deserialize(&message, representation);
}

void Serializer<KickRequest>::deserialize(const naothmessages::KickRequest* msg, KickRequest& representation)
{
  DataConversion::fromMessage(msg->kickpoint(), representation.kickPoint);
  representation.kickDirection = msg->kickdirection();
  representation.kickFoot = static_cast<KickRequest::KickFootID>(msg->kickfoot());
  representation.finishKick = msg->finishkick();
}
