#include "Representations/Infrastructure/AccelerometerData.h"
#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

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
  naothmessages::DoubleVector msg;
  for(int i=0; i<3; i++)
  {
    msg.add_v(representation.data[i]);
    msg.add_v(representation.rawData[i]);
  }
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<AccelerometerData>::deserialize(std::istream& stream, AccelerometerData& representation)
{
  naothmessages::DoubleVector msg;

  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  for(int i=0; i<3; i++)
  {
    representation.data[i] = msg.v(i*2);
    representation.rawData[i] = msg.v(i*2+1);
  }
}

