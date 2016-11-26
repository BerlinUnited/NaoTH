#include "BatteryData.h"

using namespace naoth;

BatteryData::BatteryData():
  charge(0.0),
  current(0.0),
  temperature(0.0),
  headTemperature(-1.0)
{
}

void BatteryData::print(std::ostream& stream) const
{
  // tmp hack:
  float f = (float)headTemperature;
  int* tmp = (int*)(&f);
  int v = *tmp;

  stream << "charge: " << charge << std::endl;
  stream << "current: " << current << std::endl;
  stream << "temperature: " << temperature << std::endl;
  stream << "head temperature: " << v << std::endl;
}

BatteryData::~BatteryData()
{ 
}

