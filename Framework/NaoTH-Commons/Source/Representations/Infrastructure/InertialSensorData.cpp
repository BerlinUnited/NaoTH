#include "Representations/Infrastructure/InertialSensorData.h"
#include "PlatformInterface/Platform.h"
#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

Vector2<double> InertialSensorData::offset;
int InertialSensorData::calibrateNum = 0;
string InertialSensorData::configGroup = "inertialsensor";

InertialSensorData::InertialSensorData()
{
}

InertialSensorData::~InertialSensorData()
{
  
}

void InertialSensorData::init()
{
  const naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
  if ( config.hasKey(configGroup, "offset.x") )
    offset.x = config.getDouble(configGroup, "offset.x");
  if ( config.hasKey(configGroup, "offset.y") )
    offset.y = config.getDouble(configGroup, "offset.y");
}

void InertialSensorData::calibrate()
{
  offset = (offset*calibrateNum -data) / (calibrateNum+1);
  calibrateNum++;
}

void InertialSensorData::stopCalibrating()
{
  naoth::Configuration& config =  naoth::Platform::getInstance().theConfiguration;
  
  cout<< "[InertialSensorData] : save to configure configuration group " << configGroup;
  calibrateNum = 0;
  
  config.setDouble(configGroup, "offset.x", offset.x);
  config.setDouble(configGroup, "offset.y", offset.y);
  config.save(naoth::Platform::getInstance().theConfigDirectory);
}

void InertialSensorData::print(ostream& stream) const
{
  stream << "values:" << get() << "\n";
  stream << "calibration offset: " << offset << "\n";
}//end print

void Serializer<InertialSensorData>::serialize(const InertialSensorData& representation, std::ostream& stream)
{
  naothmessages::DoubleVector msg;
  msg.add_v(representation.data.x);
  msg.add_v(representation.data.y);
  msg.add_v(representation.offset.x);
  msg.add_v(representation.offset.y);
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
  representation.offset.x = msg.v(2);
  representation.offset.y = msg.v(3);
}
