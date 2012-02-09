#include "Representations/Infrastructure/AccelerometerData.h"
#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

AccelerometerData::AccelerometerData()
{
}

AccelerometerData::~AccelerometerData()
{
}

Vector3<double> AccelerometerData::getAcceleration() const
{
    Vector3<double> acc(data);
    acc.z -= 9.81;
    return acc;
}//end getAcceleration

void AccelerometerData::print(ostream& stream) const
{
  stream << "X = " << data.x << endl;
  stream << "Y = " << data.x << endl;
  stream << "Z = " << data.x << endl;
  stream << getAcceleration() <<endl;
}//end print

void Serializer<AccelerometerData>::serialize(const AccelerometerData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i<3; i++)
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

