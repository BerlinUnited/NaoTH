/* 
 * File:   Stopwatch.h
 * Author: thomas
 *
 * Created on 18. April 2008, 18:00
 */

#ifndef _STOPWATCH_H
#define	_STOPWATCH_H

#include <map>
#include <cstring>
#include <string>

#include "Tools/DataStructures/Singleton.h"

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
    start(0), 
    stop(0), 
    isValid(false), 
    mean(0.0f),
    n(0.0f),
    min(10000.0f), // start with 10 s :)
    max(0.0f),
    lastValue(0)
  {}
  
  /** name of the stopwatch (the same as used as key in te map)*/
  std::string name;
  /** starting time in micro-seconds */
  unsigned long long start;
  /** stopping time in micro-seconds */
  unsigned long long stop;
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
};

class Stopwatch : public naoth::Singleton<Stopwatch>
{
protected:
  friend class naoth::Singleton<Stopwatch>;
  Stopwatch();

public:  
  virtual ~Stopwatch();
  
  //@Depricated and shouldn't be used
  //since access using this method is very slow
  void notifyStart(const std::string& stopWatchName);

  //@Depricated and shouldn't be used
  //since access using this method is very slow
  void notifyStop(const std::string& stopWatchName);

  /** */
  void notifyStart(StopwatchItem& stopwatchItem);
  
  /** */
  void notifyStop(StopwatchItem& stopwatchItem);


  /** */
  StopwatchItem& getStopwatchReference(const std::string& stopWatchName);

  /** */
  void dump(std::string name = "");

  std::map<std::string, StopwatchItem> stopwatches;
};

// MACROS //


//#ifdef DEBUG
#define STOPWATCH_START(name) { static StopwatchItem& _debug_stopwatch_item_ = Stopwatch::getInstance().getStopwatchReference(name); Stopwatch::getInstance().notifyStart(_debug_stopwatch_item_); }
#define STOPWATCH_STOP(name) { static StopwatchItem& _debug_stopwatch_item_ = Stopwatch::getInstance().getStopwatchReference(name); Stopwatch::getInstance().notifyStop(_debug_stopwatch_item_); }

// this is slow and should be used for loops (only if necessary)
#define STOPWATCH_START_GENERIC(name) Stopwatch::getInstance().notifyStart(name);
#define STOPWATCH_STOP_GENERIC(name) Stopwatch::getInstance().notifyStop(name);

//#else
//#define STOPWATCH_START(name) /* */
//#define STOPWATCH_STOP(name) /* */
//#define STOPWATCH_START_GENERIC(name) /* */
//#define STOPWATCH_STOP_GENERIC(name) /* */
//#endif // DEBUG

#endif	/* _STOPWATCH_H */

