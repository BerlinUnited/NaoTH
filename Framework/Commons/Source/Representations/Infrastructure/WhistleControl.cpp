#include "WhistleControl.h"

using namespace naoth;

WhistleControl::WhistleControl():
  onOffSwitch(0)
{
}

void WhistleControl::print(std::ostream& stream) const
{
  stream << "switch: " << (onOffSwitch == 1 ? "on" : "off") << std::endl;
}

WhistleControl::~WhistleControl()
{ 
}

