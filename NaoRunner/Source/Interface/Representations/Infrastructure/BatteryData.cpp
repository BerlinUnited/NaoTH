#include "Interface/Representations/Infrastructure/BatteryData.h"

using namespace naorunner;

BatteryData::BatteryData():
  charge(0.0)
{
}

void BatteryData::print(ostream& stream) const
{
  stream << "charge: " << charge << endl;
}

BatteryData::~BatteryData()
{ 
}
