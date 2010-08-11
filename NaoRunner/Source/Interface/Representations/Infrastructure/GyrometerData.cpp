#include "naorunner/Representations/Infrastructure/GyrometerData.h"

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
