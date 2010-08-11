/**
* @file NaoTime.h
*
* Class NaoTime provides information and calculation for time
*
* @author Oliver Welter
*/

#ifndef _NAOTIME_H
#define	_NAOTIME_H

#ifdef WIN32
#include <windows.h>
#else
  #include <sys/time.h>
  #include <cstdlib>
  #include <ctime>
#endif

namespace naorunner
{

  class NaoTime
  {
    static unsigned long long getSystemTimeInMicroSeconds();

  public:
    /*
     * return the time sinse the start of the controller
     */
    static unsigned int getNaoTimeInMilliSeconds();
  };
}

#endif	/* _NAOTIME_H */

