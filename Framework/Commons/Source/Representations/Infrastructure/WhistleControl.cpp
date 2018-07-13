#include "WhistleControl.h"

using namespace naoth;

WhistleControl::WhistleControl():
  onOffSwitch(0),
  whistleListFile("whistles.lst"),
  activeChannels("1010"),
  threshold(0.4),
  checkAllWhistles(true),
  saveRawAudio(false)
{}

void WhistleControl::print(std::ostream& stream) const
{
  stream << "switch: " << (onOffSwitch == 1 ? "on" : "off") << std::endl;
  stream << "whistle list file: " << whistleListFile << std::endl;
  stream << "active channels(Left|Front|Right|Rear): " << activeChannels << std::endl;
}

WhistleControl::~WhistleControl()
{}

