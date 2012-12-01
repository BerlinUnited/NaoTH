#include "BatteryData.h"

using namespace naoth;

BatteryData::BatteryData():
  charge(0.0)
{
}

void BatteryData::print(std::ostream& stream) const
{
  stream << "charge: " << charge << std::endl;
}

BatteryData::~BatteryData()
{ 
}

