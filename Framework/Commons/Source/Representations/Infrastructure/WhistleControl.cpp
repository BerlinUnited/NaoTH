#include "WhistleControl.h"

using namespace naoth;

WhistleControl::WhistleControl():
  onOffSwitch(1),
  activeChannels("1010"),
  numChannels(2),
  sampleRate(8000),
  onlySoundInSet(true),
  buffer_size(1024)
{}

void WhistleControl::print(std::ostream& stream) const
{
  stream << "switch: " << (onOffSwitch == 1 ? "on" : "off") << std::endl;
  stream << "active channels(Left|Front|Right|Rear): " << activeChannels << std::endl;
}

WhistleControl::~WhistleControl()
{}

