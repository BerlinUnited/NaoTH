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
#include "Nao/Framework/Platforms/Source/DCM/NaoRobot/Joypad.h"
//--------------------------------------------------------------------------------------------------
using namespace std;
using namespace naoth;
//--------
int main()
{
  Joypad usbJoypad;
  JoypadData data;
  timespec ts;

//  ts.tv_sec = 1;
//  ts.tv_nsec = 0;
//  nanosleep(&ts, NULL);

  ts.tv_sec = 0;
  ts.tv_nsec = 100000000;
  int moreData = 500;
  while (moreData > 0)
  {  // get as much as you want
    std::fprintf(stderr, "%03i: \n", moreData);
    usbJoypad.get(data);

    fprintf(stderr, "Valid = %i: ", data.controls.isValid);
    fprintf(stderr, "%i, ", data.controls.button.select);
    fprintf(stderr, "%i, ", data.controls.button.start);
    fprintf(stderr, "%i, ", data.controls.button.X);
    fprintf(stderr, "%i, ", data.controls.button.Y);
    fprintf(stderr, "%i, ", data.controls.button.A);
    fprintf(stderr, "%i, ", data.controls.button.B);
    fprintf(stderr, "%i, ", data.controls.button.LB);
    fprintf(stderr, "%i, ", data.controls.button.RB);
    fprintf(stderr, "%i, ", data.controls.button.LT);
    fprintf(stderr, "%i, ", data.controls.button.RT);
    fprintf(stderr, "%02i, ", data.controls.dpad);
    fprintf(stderr, "%-03.6f, ", data.controls.stick.L.x);
    fprintf(stderr, "%-03.6f, ", data.controls.stick.L.y);
    fprintf(stderr, "%-03.6f, ", data.controls.stick.R.x);
    fprintf(stderr, "%-03.6f", data.controls.stick.R.y);

//    std::cerr << data;
    std::cerr << std::endl;
    moreData--;
    nanosleep(&ts, NULL);
  }

  return 0;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
