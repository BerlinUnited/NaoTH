#include <Representations/Infrastructure/InertialSensorData.h>
#include "ConfigPathInfo.h"
#include "PlatformInterface/Platform.h"
#include "Messages/LiteStreams.h"

double InertialSensorData::offset[InertialSensorData::numOfInertialSensor];
int InertialSensorData::calibrateNum = 0;
string InertialSensorData::configFileName = "InertialSensorData.cfg";

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
  configFileName = Platform::getInstance().theConfigPathInfo.inertialsensor_parameter + "/" + Platform::getInstance().theHardwareIdentity + ".prm";
  Config cfg = ConfigLoader::loadConfig(configFileName.c_str());
  cfg.get("offset[X]", offset[X]);
  cfg.get("offset[Y]", offset[Y]);
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
  cout<< "[InertialSensorData] : save to configure file " << configFileName;
  calibrateNum = 0;
  Config cfg;
  cfg.set("offset[X]",offset[X]);
  cfg.set("offset[Y]",offset[Y]);
  ofstream of(configFileName.c_str());
  if (of)
  {
    of << cfg;
    cout << " ok"<<endl;
  } else
  {
    cout << " failed" << endl;
  }
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

void InertialSensorData::toDataStream(ostream& stream) const
{
  naothmessages::DoubleVector msg;
  for(size_t i=0; i<numOfInertialSensor; i++)
  {
    msg.add_v(data[i]);
    msg.add_v(offset[i]);
  }
  google::protobuf::io::OstreamOutputStreamLite buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void InertialSensorData::fromDataStream(istream& stream)
{
  naothmessages::DoubleVector msg;
  google::protobuf::io::IstreamInputStreamLite buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);
  for(int i=0; i<numOfInertialSensor; i++)
  {
    data[i] = msg.v(i*2);
    offset[i] = msg.v(i*2+1);
  }
}

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
