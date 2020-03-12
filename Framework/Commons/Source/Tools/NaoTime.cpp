/**
* @file NaoTime.cpp
*
* Class NaoTime provides methods for time information and calculation
*
* @author Oliver Welter
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
// EVIL HACK: used to determine the version of the robot
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
    LARGE_INTEGER highPerformanceTick;
    LARGE_INTEGER freq;
    if(QueryPerformanceCounter(&highPerformanceTick) && QueryPerformanceFrequency(&freq)) {
      double inSeconds = ((double) highPerformanceTick.LowPart) / ((double) freq.LowPart);
      return static_cast<unsigned long long>(inSeconds * 1000000.0);
    } else {
      return 0;
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

// NOTE: this is experimenal and uses c++ starndard routines
std::uint32_t NaoTime::getSystemTimeSinceMidnight() {
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	// the time relative to the start of the current day
	return static_cast<std::uint32_t>(time % (24 * 60 * 60 * 1000));
}