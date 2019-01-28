/**
* @file USBJoypadData.h
* Gamepad/Joypad data declarations
*
* @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
*/
//------------------------------------------------------------------------------
#ifndef _USB_JOYPAD_DATA_H
#define _USB_JOYPAD_DATA_H
//==============================================================================
//------------------------------------------------------------------------------
#ifdef _WIN32
  #include <Windows.h>
#else
  // anything?
#endif

#include "Tools/DataStructures/Printable.h"
#include <vector>
#include "types.h"
//------------------------------------------------------------------------------
namespace naoth
{
  class clJoypadData : public Printable
  {
  public:
    std::vector<byte> vJoypadInputData;
    virtual void print(std::ostream& stream) const;
    clJoypadData();
    virtual ~clJoypadData();
  };
}
//------------------------------------------------------------------------------
//==============================================================================
#endif  // _USB_JOYPAD_DATA_H
