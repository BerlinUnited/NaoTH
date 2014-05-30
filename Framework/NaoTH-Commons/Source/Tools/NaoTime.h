/**
* @file NaoTime.h
*
* Class NaoTime provides information and calculation for time
*
* @author Oliver Welter
*/

#ifndef _NAOTIME_H
#define _NAOTIME_H

#ifdef WIN32
#include <windows.h>
#else
  #include <sys/time.h>
  #include <cstdlib>
  #include <ctime>
#endif

namespace naoth
{
class NaoTime 
{
public:
  /**
   *
   */
  static unsigned long long getSystemTimeInMilliSeconds();

  /**
   *
   */
  static unsigned long long getSystemTimeInMicroSeconds();


  /*
   * return the time sinse the start of the controller
   */
  static unsigned int getNaoTimeInMilliSeconds();

  /**
   * This value is set once in the beginning of the programm to the system time
   * at this point in time
   */
  static const unsigned long long startingTimeInMilliSeconds;

  static const unsigned long long long_thousand = 1000;
  static const unsigned long long long_million = 1000000;
};
}// end namespace naoth
#endif  /* _NAOTIME_H */

