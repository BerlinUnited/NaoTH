/**
* @file WalkRequest.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "WalkRequest.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

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
  msg->set_stopwithstand(representation.stopWithStand);
  msg->mutable_pose()->mutable_translation()->set_x(representation.translation.x);
  msg->mutable_pose()->mutable_translation()->set_y(representation.translation.y);
  msg->mutable_pose()->set_rotation(representation.rotation);
}

void Serializer<WalkRequest>::deserialize(std::istream& stream, WalkRequest& representation)
{
  naothmessages::WalkRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  deserialize(&message, representation);
}

void Serializer<WalkRequest>::deserialize(naothmessages::WalkRequest* msg, WalkRequest& representation)
{
  representation.coordinate = static_cast<WalkRequest::Coordinate>(msg->coordinate());
  representation.stopWithStand = msg->stopwithstand();
  representation.translation.x = msg->pose().translation().x();
  representation.translation.y = msg->pose().translation().y();
  representation.rotation = msg->pose().rotation();
}
