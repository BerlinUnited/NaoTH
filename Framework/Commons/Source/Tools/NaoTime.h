/**
* @file NaoTime.h
*
* Class NaoTime provides information and calculation for time
*
* @author Oliver Welter
* @author Heinrich Mellmann <mellmann@informatik.hu-berlin.de>
*/

#ifndef NAOTIME_H
#define NAOTIME_H

typedef unsigned long long NaoTimestamp;

namespace naoth
{
class NaoTime 
{
public:

  /**
  * Returns the system time in microseconds :)
  * NOTE: operation systems have several different ways of tracking time 
  * (several clocks). Posix systems have CLOCK_MONOTONIC and CLOCK_REALTIME
  * among others.
  * 
  * NOTE: this function is used to synchronize images from the camera with other 
  * sensor input on the robot. Thus it is crucial that we use the same clock here
  * as used by the video for linux (V4L) driver for image time stamps: 
  *  - NAO V6 uses          CLOCK_MONOTONIC
  *  - NAO V5 and older use CLOCK_REALTIME
  * For more details see the implementation of the function.
  */
  static unsigned long long getSystemTimeInMicroSeconds();

  /**
  * Returns the system time in milliseconds. Wrapper for getSystemTimeInMicroSeconds.
  */
  static inline unsigned long long getSystemTimeInMilliSeconds() {
    return getSystemTimeInMicroSeconds() / 1000l;
  }

  /*
  * Return the time since the start of the program.
  */
  static inline unsigned int getNaoTimeInMilliSeconds() {
    return static_cast<unsigned int>(getSystemTimeInMilliSeconds() - startingTimeInMilliSeconds);
  }

  /**
  * Time stamp of when the programm was started. This value is set only once at 
  * the start of the programm to the system time at that point in time.
  * Used by getNaoTimeInMilliSeconds to calculate the time since the start of the 
  * program.
  */
  static const unsigned long long startingTimeInMilliSeconds;

};
}// end namespace naoth
#endif  /* NAOTIME_H */

