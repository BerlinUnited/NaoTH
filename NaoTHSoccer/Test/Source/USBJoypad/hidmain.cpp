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
  int gotEnough;

  timespec sleepTime;
  timespec remainingTime;

  sleepTime.tv_sec=0;
  sleepTime.tv_nsec=250000000;
  gotEnough=50;
  while (gotEnough > 0)
  {  // get as much as you want
    printf("%03i: ", gotEnough);
    usbJoypad.get(data);
    std::cout << data << std::endl;
    gotEnough--;
    nanosleep(&sleepTime, &remainingTime);
  }

  return 0;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
