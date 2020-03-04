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
    struct timespec t;
    // NOTE: CLOCK_MONOTONIC is necessary here, because V4L uses CLOCK_MONOTONIC for the timestamp of the images.
    //clock_gettime(CLOCK_MONOTONIC,&t); // NAO V6
    clock_gettime(CLOCK_REALTIME,&t); // NAO V5
    return t.tv_sec * 1000000LL + t.tv_nsec / 1000LL;
  #endif
}


std::uint32_t NaoTime::getSystemTimeSinceMidnight(){
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	// the time relative to the start of the current day
	return static_cast<std::uint32_t>(time % (24 * 60 * 60 * 1000));
}