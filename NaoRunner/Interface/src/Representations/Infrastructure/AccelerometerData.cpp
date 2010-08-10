#include <Representations/Infrastructure/AccelerometerData.h>
#include "Messages/LiteStreams.h"

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

void AccelerometerData::toDataStream(ostream& stream) const
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i<numOfAccelerometer; i++)
  {
    msg.add_v(data[i]);
    msg.add_v(rawData[i]);
  }
  google::protobuf::io::OstreamOutputStreamLite buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void AccelerometerData::fromDataStream(istream& stream)
{
  naothmessages::DoubleVector msg;

  google::protobuf::io::IstreamInputStreamLite buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  for(int i=0; i<numOfAccelerometer; i++)
  {
    data[i] = msg.v(i*2);
    rawData[i] = msg.v(i*2+1);
  }
}

void AccelerometerData::print(ostream& stream) const
{
  for(int i = 0; i < numOfAccelerometer; i++)
  {
    stream << getAccelerometerName((AccelerometerID)i) << " = " << data[i]  << endl;
  }//end for
  stream << getAcceleration() <<endl;
}//end print

