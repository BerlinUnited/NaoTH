#include "TeamMessage.h"

#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<TeamMessage>::serialize(const TeamMessage& representation, std::ostream& stream)
{
  naothmessages::TeamMessage msg;

  // TODO: fill message

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<TeamMessage>::deserialize(std::istream& stream, TeamMessage& representation)
{
  naothmessages::TeamMessage msg;

  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  // TODO: fill representation
}
