#include "Representations/Infrastructure/InertialSensorData.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;
using namespace std;

void InertialSensorData::print(ostream& stream) const
{
  stream << "values (deg):" << std::endl; 
  stream << "x = " << Math::toDegrees(data.x) << std::endl; 
  stream << "y = " << Math::toDegrees(data.y) << std::endl;
  stream << "z = " << Math::toDegrees(data.z) << std::endl;
}

void Serializer<InertialSensorData>::serialize(const InertialSensorData& representation, std::ostream& stream)
{
  naothmessages::InertialSensorData msg;
  
  naoth::DataConversion::toMessage(representation.data, *msg.mutable_data());

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<InertialSensorData>::deserialize(std::istream& stream, InertialSensorData& representation)
{
  naothmessages::InertialSensorData msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  // allows to parse old log files
  if(msg.legacypackeddata_size() == 2) {
    representation.data.x = msg.legacypackeddata(0);
    representation.data.y = msg.legacypackeddata(1);
    representation.data.z = 0;
  }

  // legacy: read old 2D data from logs
  if (msg.has_data2d()) {
    representation.data.x = msg.data2d().x();
    representation.data.y = msg.data2d().y();
    representation.data.z = 0;
  }

  if(msg.has_data()) {
    naoth::DataConversion::fromMessage(msg.data(), representation.data);
  }
}
