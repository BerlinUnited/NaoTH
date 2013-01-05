/* 
 * File:   Stopwatch.h
 * Author: thomas
 *
 * Created on 18. April 2008, 18:00
 */

#ifndef _STOPWATCH_H
#define _STOPWATCH_H

#include <map>
#include <cstring>
#include <string>

#include "Tools/DataStructures/Singleton.h"
#include "Trace.h"
#ifdef WIN32
  #include <windows.h>
#else
  #include <sys/time.h>
  #include <time.h>
#endif //WIN32

class StopwatchItem
{
public:
  
  StopwatchItem() 
    : 
    begin(0), 
    end(0), 
    isValid(false), 
    mean(0.0f),
    n(0.0f),
    min(10000.0f), // start with 10 s :)
    max(0.0f),
    lastValue(0)
  {}
  
  /** name of the stopwatch (the same as used as key in te map)*/
  //std::string name;
  /** starting time in micro-seconds */
  unsigned long long begin;
  /** stopping time in micro-seconds */
  unsigned long long end;
  /** true if the data (stop - start) is valid */
  bool isValid;

  // some statistics
  /** the mean time */
  float mean;
  /** count */
  float n;
  /** the mean time */
  float min;
  /** the mean time */
  float max;
  
  /** The last valid value */
  unsigned int lastValue;

public:
  /** */
  void start();
  
  /** */
  void stop();
};//end class StopwatchItem


class StopwatchManager : public naoth::Singleton<StopwatchManager>
{
protected:
  friend class naoth::Singleton<StopwatchManager>;
  StopwatchManager();

public:  
  virtual ~StopwatchManager();
  
  //@Depricated and shouldn't be used
  //since access using this method is very slow
  void notifyStart(const std::string& stopWatchName);

  //@Depricated and shouldn't be used
  //since access using this method is very slow
  void notifyStop(const std::string& stopWatchName);

  /** */
  StopwatchItem& getStopwatchReference(const std::string& stopWatchName);

  /** */
  void dump(std::string name = "");

  std::map<std::string, StopwatchItem> stopwatches;
};//end class StopwatchManager

// MACROS //


//#ifdef DEBUG
#define STOPWATCH_START(name) { GT_TRACE(name); static StopwatchItem& _debug_stopwatch_item_ = StopwatchManager::getInstance().getStopwatchReference(name); _debug_stopwatch_item_.start(); }
#define STOPWATCH_STOP(name) { GT_TRACE(name); static StopwatchItem& _debug_stopwatch_item_ = StopwatchManager::getInstance().getStopwatchReference(name); _debug_stopwatch_item_.stop(); }

// this is slow and should be used for loops (only if necessary)
#define STOPWATCH_START_GENERIC(name) StopwatchManager::getInstance().notifyStart(name);
#define STOPWATCH_STOP_GENERIC(name) StopwatchManager::getInstance().notifyStop(name);

//#else
//#define STOPWATCH_START(name) /* */
//#define STOPWATCH_STOP(name) /* */
//#define STOPWATCH_START_GENERIC(name) /* */
//#define STOPWATCH_STOP_GENERIC(name) /* */
//#endif // DEBUG

#endif  /* _STOPWATCH_H */

