/**
* @file USBJoypadData.cpp
* Gamepad/Joypad class
*
* @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
*/
//==============================================================================
//------------------------------------------------------------------------------
#include "USBJoypadData.h"
//------------------------------------------------------------------------------
using namespace naoth;

void clJoypadData::print(std::ostream& stream) const
{
  std::vector<byte>::const_iterator Vi;
  stream << "Joystick: ";
  for (Vi=vJoypadInputData.begin(); Vi < vJoypadInputData.end(); Vi++)
  {
    stream << *Vi;
  }
  stream << std::endl;
}
//------------------------------------------------------------------------------
clJoypadData::clJoypadData() 
  // : temperature(-1.0)
{ // initialize the default vector???
}
//------------------------------------------------------------------------------
clJoypadData::~clJoypadData()
{ // something???
}
//------------------------------------------------------------------------------
//==============================================================================
