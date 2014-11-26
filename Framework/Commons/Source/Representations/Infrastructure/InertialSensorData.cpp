#include "Representations/Infrastructure/InertialSensorData.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;
using namespace std;

void InertialSensorData::print(ostream& stream) const
{
  stream << "values (deg):" << std::endl; 
  stream << "x = " << Math::toDegrees(data.x) << std::endl; 
  stream << "y = " << Math::toDegrees(data.y) << std::endl;
}

void Serializer<InertialSensorData>::serialize(const InertialSensorData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  msg.add_v(representation.data.x);
  msg.add_v(representation.data.y);
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<InertialSensorData>::deserialize(std::istream& stream, InertialSensorData& representation)
{
  naothmessages::DoubleVector msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  representation.data.x = msg.v(0);
  representation.data.y = msg.v(1);
}
