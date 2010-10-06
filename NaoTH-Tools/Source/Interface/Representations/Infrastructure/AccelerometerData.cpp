#include "Interface/Representations/Infrastructure/AccelerometerData.h"

using namespace naorunner;

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

