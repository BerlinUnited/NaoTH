
#include "LineGraphPercept.h" 

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<LineGraphPercept>::serialize(const LineGraphPercept& representation, std::ostream& stream)
{
  naothmessages::LineGraphPercept msg;

  for (const Edgel& edgel : representation.edgels)
  {
    naothmessages::Edgel& edgel_msg = *(msg.add_edgelsonfield());

    DataConversion::toMessage(edgel.point, *edgel_msg.mutable_point());
    DataConversion::toMessage(edgel.direction, *edgel_msg.mutable_direction());
  }

  // serialize the message
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<LineGraphPercept>::deserialize(std::istream& stream, LineGraphPercept& representation)
{
  representation.reset();

  // deserialize the message
  naothmessages::LineGraphPercept percept_msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  percept_msg.ParseFromZeroCopyStream(&buf);

  
  representation.edgels.resize(percept_msg.edgelsonfield_size());
  for (int i = 0; i < percept_msg.edgelsonfield_size(); i++)
  {
    Edgel& edgel = representation.edgels[i];
    const naothmessages::Edgel& edgel_msg = percept_msg.edgelsonfield(i);

    DataConversion::fromMessage(edgel_msg.point(), edgel.point);
    DataConversion::fromMessage(edgel_msg.direction(), edgel.direction);
  }

}//end deserialize