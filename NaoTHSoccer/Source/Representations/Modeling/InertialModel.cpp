

#include "InertialModel.h"
#include "Tools/DataConversion.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<InertialModel>::serialize(const InertialModel& representation, std::ostream& stream)
{
  naothmessages::InertialModel msg;
  DataConversion::toMessage(representation.orientation, *(msg.mutable_orientation()));
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<InertialModel>::deserialize(std::istream& stream, InertialModel& representation)
{
  naothmessages::InertialModel msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  DataConversion::fromMessage(msg.orientation(),representation.orientation);
}
