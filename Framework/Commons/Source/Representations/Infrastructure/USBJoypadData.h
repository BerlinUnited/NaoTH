//--------------------------------------------------------------------------------------------------
//
// @file USBJoypadData.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Gamepad/Joypad data declarations
//
//--------------------------------------------------------------------------------------------------
//
#ifndef _USB_JOYPAD_DATA_H
#define _USB_JOYPAD_DATA_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Tools/DataStructures/Printable.h"
//--------------------------------------------------------------------------------------------------
namespace naoth
{
  class USBJoypadData : public Printable
  {
  public:
    std::vector<unsigned char> vJoypadInputData;
    virtual void print(std::ostream& stream) const;
    USBJoypadData();
    virtual ~USBJoypadData();
  };
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif  // _USB_JOYPAD_DATA_H
