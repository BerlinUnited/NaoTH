#include "SPLStandardMessage.h"
#include <Messages/Representations.pb.h>

ExtendedSPLStandardMessage::ExtendedSPLStandardMessage()
{

}

virtual ExtendedSPLStandardMessage::ExtendedSPLStandardMessage()
{

}

void Serializer<ExtendedSPLStandardMessage>::serialize(
    const ExtendedSPLStandardMessage& representation, std::ostream& stream)
{
  naothmessages::ExtendedSPLStandardMessage proto;

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

