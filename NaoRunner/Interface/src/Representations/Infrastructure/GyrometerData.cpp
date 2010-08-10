#include <Representations/Infrastructure/GyrometerData.h>

#include "Messages/LiteStreams.h"

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

void GyrometerData::toDataStream(ostream& stream) const
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i<numOfGyrometer; i++)
  {
    msg.add_v(data[i]);
    msg.add_v(rawData[i]);
  }
  msg.add_v(rawData[numOfGyrometer]);

  google::protobuf::io::OstreamOutputStreamLite buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}//end toDataStream

void GyrometerData::fromDataStream(istream& stream)
{
  naothmessages::DoubleVector msg;

  google::protobuf::io::IstreamInputStreamLite buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  for(int i=0; i<numOfGyrometer; i++)
  {
    data[i] = msg.v(i*2);
    rawData[i] = msg.v(i*2+1);
  }
  rawData[numOfGyrometer] = msg.v(msg.v_size()-1);
}//end fromDataStream

void GyrometerData::print(ostream& stream) const
{
  for(int i = 0; i < numOfGyrometer; i++)
  {
    stream << getGyrometerName((GyrometerID)i) << " = " << data[i]  << endl;
  }//end for
}//end print
