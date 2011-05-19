#include "Representations/Infrastructure/InertialSensorData.h"
#include "PlatformInterface/Platform.h"
#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

double InertialSensorData::offset[InertialSensorData::numOfInertialSensor];
int InertialSensorData::calibrateNum = 0;
string InertialSensorData::configGroup = "inertialsensor";

InertialSensorData::InertialSensorData()
{
  for (int i = 0; i < numOfInertialSensor; i++)
  {
    data[i] = 0.0;
    lastData[i][0] = 0.0;
    lastData[i][1] = 0.0;
    lastData[i][2] = 0.0;
    lastData[i][3] = 0.0;
  }
}

InertialSensorData::~InertialSensorData()
{
  
}

void InertialSensorData::init()
{
  const naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
  offset[X] = config.getDouble(configGroup, "offset[X]");
  offset[Y] = config.getDouble(configGroup, "offset[Y]");
}

void InertialSensorData::calibrate()
{
    for(int i=0; i<numOfInertialSensor; i++){
        offset[i] = (offset[i]*calibrateNum -data[i]) / (calibrateNum+1);
    }
    calibrateNum++;
}

void InertialSensorData::stopCalibrating()
{
  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
  
  cout<< "[InertialSensorData] : save to configure configuration group " << configGroup;
  calibrateNum = 0;
  
  config.setDouble(configGroup, "offset[X]", offset[X]);
  config.setDouble(configGroup, "offset[Y]", offset[Y]);
  config.save(naoth::Platform::getInstance().theConfigDirectory);
}

string InertialSensorData::getInertialSensorName(InertialSensorID angle)
{
  switch(angle)
  {
    case X:
      return "X";
    case Y:
      return "Y";
    default:
      return "Unknown InertialSensorID";

  }
}//end getInertialSensorName

void InertialSensorData::print(ostream& stream) const
{
  stream << "values:\n";
  for (int i = 0; i < numOfInertialSensor; i++) 
  {
    stream << getInertialSensorName((InertialSensorID) i) << " = " << get((InertialSensorID) i) << "\n";
  }

  stream << "calibration offset:\n";
  for (int i = 0; i < numOfInertialSensor; i++) 
  {
    stream << getInertialSensorName((InertialSensorID) i) << " = " << offset[i] << "\n";
  }

  stream << "raw data:\n";
  for (int i = 0; i < numOfInertialSensor; i++) 
  {
    stream << getInertialSensorName((InertialSensorID) i) << " = " << data[i] << "\n";
  }
}//end print

void Serializer<InertialSensorData>::serialize(const InertialSensorData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i< InertialSensorData::numOfInertialSensor; i++)
  {
    msg.add_v(representation.data[i]);
    msg.add_v(representation.offset[i]);
  }
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<InertialSensorData>::deserialize(std::istream& stream, InertialSensorData& representation)
{
  naothmessages::DoubleVector msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  for(int i=0; i<InertialSensorData::numOfInertialSensor; i++)
  {
    representation.data[i] = msg.v(i*2);
    representation.offset[i] = msg.v(i*2+1);
  }
}
