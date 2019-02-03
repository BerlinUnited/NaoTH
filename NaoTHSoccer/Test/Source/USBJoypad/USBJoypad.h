#ifndef _USB_JOYPAD_H
#define _USB_JOYPAD_H
//==============================================================================
//------------------------------------------------------------------------------
#include <mutex>
#include <thread>
#include <fstream>
#include "USBJoypadData.h"
//------------------------------------------------------------------------------
class USBJoypad
{
private:
  bool exiting;
  unsigned int lastGetTime;
  std::mutex dataMutex;
  std::thread readThread;
//  std::ifstream temperatureFile; //???
  naoth::USBJoypadData JoypadInputData;

public:
  void get(naoth::USBJoypadData& rJoypadData);
  void readLoop();
  USBJoypad();
  ~USBJoypad();
};
//------------------------------------------------------------------------------
//==============================================================================
#endif // _USB_JOYPAD_H
