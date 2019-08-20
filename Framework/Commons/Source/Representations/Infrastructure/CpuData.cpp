#include "CpuData.h"

using namespace naoth;

CpuData::CpuData():
  temperature(-1.0)
{
}

void CpuData::print(std::ostream& stream) const
{
  stream << "head temperature: " << temperature << "°C" << std::endl;
}

CpuData::~CpuData()
{
}
