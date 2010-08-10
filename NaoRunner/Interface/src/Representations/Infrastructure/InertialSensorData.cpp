#include "Representations/Infrastructure/InertialSensorData.h"
//#include "Representations/Infrastructure/ConfigPathInfo.h"
#include "PlatformInterface/Platform.h"

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
