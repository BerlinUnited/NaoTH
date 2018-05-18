
#include "LineGraphPercept.h" 

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>
#include <Tools/SerialzationHelpers.h>

using namespace naoth;

void Serializer<LineGraphPercept>::serialize(const LineGraphPercept& representation, std::ostream& stream)
{
  naothmessages::LineGraphPercept msg;

  SerialzationHelpers::toMessage(representation.edgelsOnField, *msg.mutable_edgelsonfield());
  SerialzationHelpers::toMessage(representation.edgelsInImage, *msg.mutable_edgelsinimage());
  SerialzationHelpers::toMessage(representation.edgelsInImageTop, *msg.mutable_edgelsinimagetop());

  // serialize the message
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<LineGraphPercept>::deserialize(std::istream& stream, LineGraphPercept& representation)
{
  representation.reset();

  // deserialize the message
  naothmessages::LineGraphPercept msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  SerialzationHelpers::fromMessage(msg.edgelsonfield(), representation.edgelsOnField);
  SerialzationHelpers::fromMessage(msg.edgelsinimage(), representation.edgelsInImage);
  SerialzationHelpers::fromMessage(msg.edgelsinimagetop(), representation.edgelsInImageTop);
  
}//end deserialize