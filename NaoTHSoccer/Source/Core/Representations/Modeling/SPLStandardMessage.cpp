#include "SPLStandardMessage.h"
#include <Messages/Representations.pb.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

ExtendedSPLStandardMessage::ExtendedSPLStandardMessage()
{

}

ExtendedSPLStandardMessage::~ExtendedSPLStandardMessage()
{

}

void Serializer<ExtendedSPLStandardMessage>::serialize(
    const ExtendedSPLStandardMessage& representation, std::ostream& stream)
{
  naothmessages::ExtendedSPLStandardMessage proto;

  google::protobuf::io::OstreamOutputStream buf(&stream);
  proto.SerializePartialToZeroCopyStream(&buf);
}

void Serializer<ExtendedSPLStandardMessage>::deserialize(
    std::istream& stream,
    ExtendedSPLStandardMessage& representation)
{
  naothmessages::ExtendedSPLStandardMessage proto;
  google::protobuf::io::IstreamInputStream buf(&stream);
  proto.ParseFromZeroCopyStream(&buf);

}

