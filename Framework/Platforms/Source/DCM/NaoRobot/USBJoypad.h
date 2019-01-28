#ifndef _USB_JOYPAD_H
#define _USB_JOYPAD_H
//==============================================================================
//------------------------------------------------------------------------------
#include <mutex>
#include <thread>
#include <fstream>
#include "Representations/Infrastructure/USBJoypadData.h"
//------------------------------------------------------------------------------
class clJoypad
{
private:
  bool exiting;
  unsigned int lastGetTime;
  std::mutex dataMutex;
  std::thread readThread;
//  std::ifstream temperatureFile; //???
  naoth::clJoypadData JoypadInputData;

public:
  void get(naoth::clJoypadData& rJoypadData);
  void readLoop();
  clJoypad();
  ~clJoypad();
};
//------------------------------------------------------------------------------
//==============================================================================
#endif // _USB_JOYPAD_H
