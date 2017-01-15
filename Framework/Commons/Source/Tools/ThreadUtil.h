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

};

}

#endif // THREADUTIL_H
