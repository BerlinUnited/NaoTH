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
#include "Tools/Math/Vector2.h"
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
    
    Vector2d cross;
    Vector2d analogL;
    Vector2d analogR;
    
    struct Button {
      bool A;
      bool B;
      bool X;
      bool Y;  
      bool start;
      bool select;       
    } button;
    
    bool valid;
  };
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif  // _USB_JOYPAD_DATA_H
