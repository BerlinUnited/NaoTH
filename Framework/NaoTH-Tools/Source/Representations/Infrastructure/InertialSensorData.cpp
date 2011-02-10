#include "Representations/Infrastructure/InertialSensorData.h"
//#include "Representations/Infrastructure/ConfigPathInfo.h"
#include "PlatformInterface/Platform.h"
#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

double InertialSensorData::offset[InertialSensorData::numOfInertialSensor];

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
//  configFileName = Platform::getInstance().theConfigPathInfo.inertialsensor_parameter + "/" + Platform::getInstance().theHardwareIdentity + ".prm";
//  Config cfg = ConfigLoader::loadConfig(configFileName.c_str());
//  cfg.get("offset[X]", offset[X]);
//  cfg.get("offset[Y]", offset[Y]);
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

//  stream << "calibration offset:\n";
//  for (int i = 0; i < numOfInertialSensor; i++)
//  {
//    stream << getInertialSensorName((InertialSensorID) i) << " = " << offset[i] << "\n";
//  }

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
