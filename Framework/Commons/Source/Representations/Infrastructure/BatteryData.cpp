#include "BatteryData.h"

using namespace naoth;

BatteryData::BatteryData():
  charge(0.0),
  current(0.0),
  temperature(0.0)
{
}

void BatteryData::print(std::ostream& stream) const
{
  stream << "charge: " << charge << std::endl;
  stream << "current: " << current << std::endl;
  stream << "temperature: " << temperature << std::endl;
}
