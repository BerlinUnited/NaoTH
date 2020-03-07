/**
* @file NaoTime.h
*
* Class NaoTime provides information and calculation for time
*
* @author Oliver Welter
*/

#ifndef NAOTIME_H
#define NAOTIME_H

#include <cstdint>

#ifdef WIN32
  #include <windows.h>
#else
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
  static inline unsigned long long getSystemTimeInMilliSeconds() {
    return getSystemTimeInMicroSeconds() / 1000l;
  }

  /*
   * return the time since the start of the controller
   */
  static inline unsigned int getNaoTimeInMilliSeconds() {
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
};
}// end namespace naoth
#endif  /* NAOTIME_H */

