#include "Representations/Infrastructure/GyrometerData.h"
#include "Messages/CommonTypes.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

GyrometerData::GyrometerData()
{
  for(int i=0; i < numOfGyrometer; i++)
  {
    data[i] = 0.0;
    rawData[i] = 0.0;
    rawZero[i] = 0.0;
  }
  rawData[numOfGyrometer] = 0.0;
}

GyrometerData::~GyrometerData()
{
}

void GyrometerData::calibrate()
{
  rawZero[0] = rawData[0];
  data[0] = 0.0;

  rawZero[1] = rawData[1];
  data[1] = 0.0;
}

string GyrometerData::getGyrometerName(GyrometerID gyro)
{
  switch(gyro)
  {
    case X: return "X";
    case Y: return "Y";
    default: return "Unknown GyrometerID";
  }//end switch
}//end getGyrometerName


void GyrometerData::print(ostream& stream) const
{
  for(int i = 0; i < numOfGyrometer; i++)
  {
    stream << getGyrometerName((GyrometerID)i) << " = " << data[i]  << endl;
  }//end for
}//end print

void Serializer<GyrometerData>::serialize(const GyrometerData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i< GyrometerData::numOfGyrometer; i++)
  {
    msg.add_v(representation.data[i]);
    msg.add_v(representation.rawData[i]);
  }
  msg.add_v(representation.rawData[GyrometerData::numOfGyrometer]);

  ::google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<GyrometerData>::deserialize(std::istream& stream, GyrometerData& representation)
{
  naothmessages::DoubleVector msg;

  ::google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  for(int i=0; i<GyrometerData::numOfGyrometer; i++)
  {
    representation.data[i] = msg.v(i*2);
    representation.rawData[i] = msg.v(i*2+1);
  }
  representation.rawData[GyrometerData::numOfGyrometer] = msg.v(msg.v_size()-1);  
}