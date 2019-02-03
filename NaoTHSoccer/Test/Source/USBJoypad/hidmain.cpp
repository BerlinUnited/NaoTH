//--------------------------------------------------------------------------------------------------
//
// @file hidmain.cpp
//
// This is only a temporary main file for test purposes
// It does, what NAO has to do:
// - initialize an object of the HID / Joypad class
// - request data via get
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <iostream>
#include "USBJoypad.h"
//--------------------------------------------------------------------------------------------------
using namespace std;
using namespace naoth;
//--------
int main()
{
  USBJoypadData data;
  USBJoypad usbJoypad;
  bool gotEnough;
  int keyPressed;

  gotEnough=false;
  while (!gotEnough)
  {  // get as much as you want
    keyPressed=getchar();
    if (keyPressed=='e')
      gotEnough=true;
    else
    {
      usbJoypad.get(data);
      std::cout << data;
    }
  }

  return 0;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
