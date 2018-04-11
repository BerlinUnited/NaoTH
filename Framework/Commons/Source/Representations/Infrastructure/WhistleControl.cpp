#include "WhistleControl.h"

using namespace naoth;

WhistleControl::WhistleControl():
  onOffSwitch(0),
  whistleListFile("whistles.lst"),
  activeChannels("1010")

{}

void WhistleControl::print(std::ostream& stream) const
{
  stream << "switch: " << (onOffSwitch == 1 ? "on" : "off") << std::endl;
  stream << "whistle list file: " << whistleListFile << std::endl;
  stream << "active channels(Left|Front|Right|Rear): " << activeChannels << std::endl;
}

WhistleControl::~WhistleControl()
{}

