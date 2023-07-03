#ifndef THREADUTIL_H
#define THREADUTIL_H

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
  #ifdef _POSIX_THREADS
    #include <pthread.h>
  #endif // _POSIX_THREADS
#endif // _WIN32

#include <thread>
#include <chrono>

#include <cstring>

namespace naoth {

class ThreadUtil
{

public:

  enum class Priority {lowest, highest};

  static bool setPriority(std::thread& t, Priority prio)
  {
  #ifdef _POSIX_THREADS
     int policy;
     int errCode;
     struct sched_param param;

     // query the policy we use
     errCode = pthread_getschedparam(t.native_handle(), &policy, &param);
     if(errCode == 0)
     {
       switch(prio)
       {
       case Priority::lowest:
         param.sched_priority = sched_get_priority_min(policy);
         break;
       case Priority::highest:
         param.sched_priority = sched_get_priority_max(policy);
         break;
       }
       errCode = pthread_setschedparam(t.native_handle(), policy, &param);
       if(errCode == 0)
       {
         return true;
       }
     }
  #elif _WIN32
  #endif // _POSIX_THREADS

    return false;
  }

  static bool setName(std::thread& t, const std::string& name)
  {
  #ifdef _POSIX_THREADS
    // The thread name is a meaningful C language string, whose 
    // length is restricted to 16 characters,
    // including the terminating null byte ('\0')
    pthread_setname_np(t.native_handle(), name.substr(0,15).c_str());
    
  #elif _WIN32
    // do nothing in windows
  #endif // _POSIX_THREADS

    return false;
  }


  // sleep for a number of milliseconds
  static void sleep(unsigned int ms) 
  {
    // NOTE: sleep_for() is quite imprecise starting Windows 10. 
    // Approach set the resolution for the timing services higher. 
    // This can increase computational load.
    // It's here for the future use in case higher precision is required.
    // https://learn.microsoft.com/en-us/windows/win32/api/timeapi/nf-timeapi-timebeginperiod
    // requires winmm.lib to be linked in VS
    //timeBeginPeriod(1);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(ms)); 
    
    //timeEndPeriod(1);
    
    
    // NOTE: this is the legacy way of a portable sleep function
    /*
    #ifdef WIN32
      Sleep(t);
    #else
      usleep(t * 1000);
    #endif
    */
  }

};

}

#endif // THREADUTIL_H
