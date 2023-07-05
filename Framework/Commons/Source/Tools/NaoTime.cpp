/**
* @file NaoTime.cpp
*
* Class NaoTime provides methods for time information and calculation
*
* @author Oliver Welter
* @author Heinrich Mellmann <mellmann@informatik.hu-berlin.de>
*/

#include "NaoTime.h"
#include <chrono>

using namespace naoth;

#ifdef WIN32
  #include <windows.h>
#else
  #include <ctime>
#endif

// TODO: need a better solution here
// HACK: used to determine the version of the robot (EVIL HACK)
#ifdef NAO
#include <sys/stat.h>
#include <stdio.h>

static inline bool fileExists (const char* filename) {
  struct stat buffer;   
  return (stat(filename, &buffer) == 0); 
}
// determine the clock id to use and print status
// NOTE: CLOCK_MONOTONIC is used on NAO V6, because V4L uses CLOCK_MONOTONIC for the timestamp of the images.
// on NAO V5 and earlier, CLOCK_REALTIME is used by V4L.
static inline clockid_t getClockId() {
  static const bool NAOV6 = fileExists("/usr/bin/lola") || fileExists("/opt/aldebaran/bin/lola");
  if(NAOV6) {
    printf("[NaoTime] use CLOCK_MONOTONIC\n");
    return CLOCK_MONOTONIC;
  } else {
    printf("[NaoTime] use CLOCK_REALTIME\n");
    return CLOCK_REALTIME;
  }
}
#endif


const unsigned long long NaoTime::startingTimeInMilliSeconds = getSystemTimeInMilliSeconds();


unsigned long long NaoTime::getSystemTimeInMicroSeconds()
{
  #ifdef WIN32
    // https://learn.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps
    // Get the frequency once and check if high-resolution performance counter is supported.
    static LARGE_INTEGER s_frequency;
    static BOOL supported = QueryPerformanceFrequency(&s_frequency);
    if (supported) {
      LARGE_INTEGER now;
      QueryPerformanceCounter(&now);
      return (1000000LL * now.QuadPart) / s_frequency.QuadPart;
    } else {
      return GetTickCount64();
    }
  #else // NAO, Linux, MAC
  
  #ifdef NAO
    static const clockid_t ID = getClockId();
  #else
    // use monotonic clock on linux and mac
    static const clockid_t ID = CLOCK_MONOTONIC;
  #endif

    struct timespec t;
    clock_gettime(ID, &t);
    return t.tv_sec * 1000000LL + t.tv_nsec / 1000LL;
  #endif
}
