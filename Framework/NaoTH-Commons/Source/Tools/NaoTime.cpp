/**
* @file NaoTime.cpp
*
* Class NaoTime provides methods for time information and calculation
*
* @author Oliver Welter
*/
#include "NaoTime.h"
#include "Tools/Debug/Trace.h"
#include "Tools/Debug/NaoTHAssert.h"
#include <iostream>

using namespace naoth;

const unsigned long long NaoTime::startingTimeInMilliSeconds = getSystemTimeInMilliSeconds();

unsigned long long NaoTime::getSystemTimeInMilliSeconds()
{
  #ifdef WIN32
    LARGE_INTEGER highPerformanceTick;
    LARGE_INTEGER freq;
    if(QueryPerformanceCounter(&highPerformanceTick) && QueryPerformanceFrequency(&freq))
    {
      double inSeconds = ((double) highPerformanceTick.LowPart) / ((double) freq.LowPart);
      return (unsigned long long) (inSeconds * 1000.0);
    }
  #else
  #ifdef NAO
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC ,&t);
    return ((unsigned long long)t.tv_sec) * long_thousand + ((unsigned long long)t.tv_nsec) / long_million;
  #else
    struct timeval t;
    int returnval = gettimeofday(&t, NULL);
    if(returnval == 0)
    {
      return ((unsigned long long)t.tv_sec) * long_thousand + ((unsigned long long)t.tv_usec) / long_thousand;
    }
  #endif
  #endif

  return 0;
}//end getSystemTimeInMicroSeconds


unsigned int NaoTime::getNaoTimeInMilliSeconds()
{
  return (unsigned int) (getSystemTimeInMilliSeconds() - startingTimeInMilliSeconds);
}//end getNaoTimeInMilliSeconds
