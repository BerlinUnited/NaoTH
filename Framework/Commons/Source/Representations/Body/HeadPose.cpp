/**
* @file HeadPose.cpp
*
* @author <a href="mailto:mellmanninformatik.hu-berlin.de">Heinrich Mellmann</a>
*
*/

#include "HeadPose.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<HeadPose>::serialize(const HeadPose& representation, std::ostream& stream)
{
  naothmessages::HeadPose message;

  message.set_timestamp(representation.timestamp);
  DataConversion::toMessage(representation.pose, *message.mutable_pose());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<HeadPose>::deserialize(std::istream& stream, HeadPose& representation)
{
  naothmessages::HeadPose message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(message.ParseFromZeroCopyStream(&buf))
  {
    representation.timestamp = message.timestamp();
    DataConversion::fromMessage(message.pose(), representation.pose);

    // hack: convert from meters to mm
    representation.pose.translation *= 1000.0;
  }
  else
  {
    THROW("Serializer<HeadPose>::deserialize failed");
  }
}
