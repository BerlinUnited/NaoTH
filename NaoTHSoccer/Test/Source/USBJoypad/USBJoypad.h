#ifndef _USB_JOYPAD_H
#define _USB_JOYPAD_H
//==============================================================================
//------------------------------------------------------------------------------
#include <mutex>
#include <thread>
#include <fstream>
#include "USBJoypadData.h"
#include "udevIf.h"
//------------------------------------------------------------------------------
class USBJoypad
{
private:
  bool exiting;
  unsigned int lastGetTime;
  std::mutex dataMutex;
  std::thread readThread;
  naoth::USBJoypadData dataJoypad;
  UDevInterface udevInterface;

public:
  void get(naoth::USBJoypadData& dataJoypad);
  void readLoop();
  USBJoypad();
  ~USBJoypad();
};
//------------------------------------------------------------------------------
//==============================================================================
#endif // _USB_JOYPAD_H
