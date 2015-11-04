
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;
using namespace std;

Vector3d AccelerometerData::getAcceleration() const
{
  Vector3d acc(data);
  acc.z += Math::g;
  return acc;
}

void AccelerometerData::print(ostream& stream) const
{
  stream << "x = " << data.x << endl;
  stream << "y = " << data.y << endl;
  stream << "z = " << data.z << endl;
  stream << getAcceleration() <<endl;
}

void Serializer<AccelerometerData>::serialize(const AccelerometerData& representation, std::ostream& stream)
{
  naothmessages::AccelerometerData msg;

  naoth::DataConversion::toMessage(representation.data, *msg.mutable_data());
  naoth::DataConversion::toMessage(representation.rawData, *msg.mutable_rawdata());

  ::google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<AccelerometerData>::deserialize(std::istream& stream, AccelerometerData& representation)
{
  naothmessages::AccelerometerData msg;

  ::google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  // allows to parse old log files
  if(msg.legacypackeddata_size() == 6)
  {
    for(int i=0; i<3; i++)
    {
      representation.data[i] = msg.legacypackeddata(i*2);
      representation.rawData[i] = msg.legacypackeddata(i*2+1);
    }
  }

  // always use the non-deprecated fields if possible
  if(msg.has_data()) {
    naoth::DataConversion::fromMessage(msg.data(),representation.data);
  }
  if(msg.has_rawdata()) {
    naoth::DataConversion::fromMessage(msg.rawdata(),representation.rawData);
  }
}

