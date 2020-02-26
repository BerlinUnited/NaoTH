/**
* @file NaoTime.h
*
* Class NaoTime provides information and calculation for time
*
* @author Oliver Welter
*/

#ifndef _NAOTIME_H
#define _NAOTIME_H

#include <cstdint>

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
  static unsigned long long getSystemTimeInMicroSeconds();

  /**
   *
   */
  static unsigned long long getSystemTimeInMilliSeconds() {
    return getSystemTimeInMicroSeconds() / 1000l;
  }

  /*
   * return the time since the start of the controller
   */
  static unsigned int getNaoTimeInMilliSeconds() {
    return static_cast<unsigned int>(getSystemTimeInMilliSeconds() - startingTimeInMilliSeconds);
  }

  /**
   * This value is set once in the beginning of the programm to the system time
   * at this point in time
   */
  static const unsigned long long startingTimeInMilliSeconds;

	/**
	 * Returns the milliseconds since midnight
	 */
	static std::uint32_t getSystemTimeSinceMidnight();

  static const unsigned long long long_thousand = 1000;
  static const unsigned long long long_million = 1000000;
};
}// end namespace naoth
#endif  /* _NAOTIME_H */

