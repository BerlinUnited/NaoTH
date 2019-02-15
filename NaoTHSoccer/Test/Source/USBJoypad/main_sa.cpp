//--------------------------------------------------------------------------------------------------
//
// @file sa_main.cpp
//
// This is only a temporary main file for test purposes
// "true" standalone
//
//--------------------------------------------------------------------------------------------------
//
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <unistd.h>
#include "udevIf.h"
//--------------------------------------------------------------------------------------------------
int main()
{
  int sleepTime_s;
  UDevInterface udevInterface;

  std::fprintf(stderr, "Hello from standalone main.\n");
  std::fprintf(stderr, "Starting the hotplug detection\n");
  udevInterface.startHIDPlugDetection();

  sleepTime_s=60;
  std::fprintf(stderr, "Main sleeping for %i seconds...\n", sleepTime_s);
  usleep(1000*1000*sleepTime_s);

  std::fprintf(stderr, "\nStopping the hotplug detection.\n");
  udevInterface.stopHIDPlugDetection();

  sleepTime_s=5;
  std::fprintf(stderr, "Main sleeping for %i seconds...\n", sleepTime_s);
  usleep(1000*1000*sleepTime_s);

  return 0;
}
//--------------------------------------------------------------------------------------------------
//==================================================================================================
