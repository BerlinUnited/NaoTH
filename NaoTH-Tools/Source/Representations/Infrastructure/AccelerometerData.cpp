#include "Representations/Infrastructure/AccelerometerData.h"
#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

AccelerometerData::AccelerometerData()
{
  for (int i = 0; i < numOfAccelerometer; i++)
  {
    data[i] = 0.0;
    rawData[i] = 0.0;
  }
}

AccelerometerData::~AccelerometerData()
{
}

Vector3<double> AccelerometerData::getAcceleration() const
{
    Vector3<double> acc;
    acc.x = data[X];
    acc.y = data[Y];
    acc.z = data[Z] - 9.81;
    return acc;
}

string AccelerometerData::getAccelerometerName(AccelerometerID acc)
{
  switch(acc)
  {
    case X:
      return "X";
    case Y:
      return "Y";
    case Z:
      return "Z";
    default:
      return "Unknown AccelerometerID";
  }//end switch
}//end getAccelerometerName


void AccelerometerData::print(ostream& stream) const
{
  for(int i = 0; i < numOfAccelerometer; i++)
  {
    stream << getAccelerometerName((AccelerometerID)i) << " = " << data[i]  << endl;
  }//end for
  stream << getAcceleration() <<endl;
}//end print

void Serializer<AccelerometerData>::serialize(const AccelerometerData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i< AccelerometerData::numOfAccelerometer; i++)
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
  for(int i=0; i<AccelerometerData::numOfAccelerometer; i++)
  {
    representation.data[i] = msg.v(i*2);
    representation.rawData[i] = msg.v(i*2+1);
  }
}

