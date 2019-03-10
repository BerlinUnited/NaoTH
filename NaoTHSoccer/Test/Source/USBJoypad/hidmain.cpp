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
#include "Nao/Framework/Platforms/Source/DCM/NaoRobot/USBJoypad.h"
//--------------------------------------------------------------------------------------------------
using namespace std;
using namespace naoth;
//--------
int main()
{
  USBJoypadData data;
  USBJoypad usbJoypad;
  int moreData;

  timespec ts;
  
  ts.tv_sec=0;
  ts.tv_nsec=250000000;
  moreData=50;
  while (moreData > 0)
  {  // get as much as you want
    std::fprintf(stderr, "%03i: ", moreData);
    usbJoypad.get(data);
//    std::cerr << data;
    std::cerr << std::endl;
    moreData--;
    nanosleep(&ts, NULL);
  }

  return 0;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
