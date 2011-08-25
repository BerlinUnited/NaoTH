/**
* @file MotionRequest.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "MotionRequest.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <Messages/Representations.pb.h>

using namespace naoth;

void Serializer<MotionRequest>::serialize(const MotionRequest& representation, std::ostream& stream)
{
  naothmessages::MotionRequest message;
  message.set_id(representation.id);
  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<MotionRequest>::deserialize(std::istream& stream, MotionRequest& representation)
{
  naothmessages::MotionRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.id = static_cast<MotionRequest::MotionID>(message.id());
}
