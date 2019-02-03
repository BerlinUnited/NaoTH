//------------------------------------------------------------------------------
//
// @file USBJoypadData.cpp
// Gamepad/Joypad data representation class
//
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
//------------------------------------------------------------------------------
//
//==============================================================================
//------------------------------------------------------------------------------
#include "USBJoypadData.h"
//------------------------------------------------------------------------------
using namespace naoth;
//------------------------------------------------------------------------------
void USBJoypadData::print(std::ostream& stream) const
{
  std::vector<unsigned char>::const_iterator Vi;
  stream << "Joypad: ";
  for (Vi=vJoypadInputData.begin(); Vi < vJoypadInputData.end(); Vi++)
  {
    stream << *Vi;
  }
  stream << std::endl;
}
//------------------------------------------------------------------------------
USBJoypadData::USBJoypadData()
//: vJoypadInputData(0x00, 0x00)
{ // initialize the default vector???
}
//------------------------------------------------------------------------------
USBJoypadData::~USBJoypadData()
{ // something???
}
//------------------------------------------------------------------------------
//==============================================================================
