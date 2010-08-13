/**
* @file NaoTime.cpp
*
* Class NaoTime provides methods for time information and calculation
*
* @author Oliver Welter
*/
#include "naorunner/Tools/NaoTime.h"

using namespace naorunner;

unsigned long long NaoTime::getSystemTimeInMicroSeconds()
{
  #ifdef WIN32
    LARGE_INTEGER highPerformanceTick;
    LARGE_INTEGER freq;
    if(QueryPerformanceCounter(&highPerformanceTick) && QueryPerformanceFrequency(&freq))
    {
      double inSeconds = ((double) highPerformanceTick.LowPart) / ((double) freq.LowPart);
      return (unsigned long long) (inSeconds * 1000000.0);
    }
  #else
  #ifdef NAO
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC ,&t);
    return t.tv_sec * 1000000 + t.tv_nsec/1000;
  #else
    struct timeval t;
    if(gettimeofday(&t, NULL) == 0)
    {
      return t.tv_sec * 1000000 + t.tv_usec;
    }
  #endif
  #endif
  return 0;
}//end getSystemTimeInMicroSeconds


unsigned int NaoTime::getNaoTimeInMilliSeconds()
{
  static const unsigned long long startingTimeInMicroSeconds = getSystemTimeInMicroSeconds();
  return (unsigned int) ((getSystemTimeInMicroSeconds() - startingTimeInMicroSeconds) / 1000 );
}//end getNaoTimeInMilliSeconds
