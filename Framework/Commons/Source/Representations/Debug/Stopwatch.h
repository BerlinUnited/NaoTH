/* 
 * File:   Stopwatch.h
 * Author: thomas
 *
 * Created on 18. April 2008, 18:00
 */

#ifndef _Stopwatch_H_
#define _Stopwatch_H_

#include <map>
#include <cstring>
#include <string>

#include "Tools/DataStructures/Serializer.h"

class Stopwatch
{
public:
  
  Stopwatch();
  
  /** starting time in microseconds */
  unsigned long long begin;
  /** stopping time in microseconds */
  unsigned long long end;
  
  /** 
  * Indicates that the stopwatch is in a consistent state.
  * true if the stopwatch was running at least once 
  * and is currently stopped, i.e., there is a valid 
  * measurement.
  */
  bool isValid;
  
  /** the last valid value (end - begin) */
  unsigned int lastValue;

  /** some statistics in milliseconds
  * those values are only usefull if the same Stopwatch 
  * object is used several times. The statistics are calculated 
  * based on the lastValue. 
  */
  /** the mean time */
  double mean;
  /** number of measurements */
  double n;
  /** minimal measured time */
  double min;
  /** maximal measured time */
  double max;
  
public:
  /** */
  void start();
  
  /** */
  void stop();
};//end class Stopwatch


class StopwatchManager // : public naoth::Singleton<StopwatchManager>
{
public:
  StopwatchManager() {}
  virtual ~StopwatchManager() {}
  
  typedef std::map<std::string, Stopwatch> StopwatchMap;

  void notifyStart(const std::string& name) {
    stopwatches[name].start();
  }

  void notifyStop(const std::string& name) {
    stopwatches[name].stop();
  }

  Stopwatch& getStopwatch(const std::string& name) {
    return stopwatches[name];
  }

  const StopwatchMap& getStopwatches() const { 
    return stopwatches; 
  }

  /** */
  void dump(std::string name = "") const;

private:
  StopwatchMap stopwatches;

};//end class StopwatchManager

namespace naoth
{
template<>
class Serializer<StopwatchManager>
{
  public:
  static void serialize(const StopwatchManager& object, std::ostream& stream);

  // we don't need that
  static void deserialize(std::istream& stream, StopwatchManager& object);
};
}

// MACROS for convenient usage.
// NOTE: those macros only work inside a MODULE whith PROVIDE(StopwatchManager)

#define USE_DEBUG_STOPWATCH

#ifdef USE_DEBUG_STOPWATCH

#define STOPWATCH_START(name) \
{ \
  static Stopwatch& _debug_stopwatch_item_ = getStopwatchManager().getStopwatch(name); \
  _debug_stopwatch_item_.start(); \
} ((void)0)

#define STOPWATCH_STOP(name) \
{ \
  static Stopwatch& _debug_stopwatch_item_ = getStopwatchManager().getStopwatch(name); \
  _debug_stopwatch_item_.stop(); \
} ((void)0)

// this is slow and should be used for loops (only if necessary)
#define STOPWATCH_START_GENERIC(name) getStopwatchManager().notifyStart(name);
#define STOPWATCH_STOP_GENERIC(name) getStopwatchManager().notifyStop(name);

#else
#define STOPWATCH_START(name) ((void)0)
#define STOPWATCH_STOP(name) ((void)0)
#define STOPWATCH_START_GENERIC(name) ((void)0)
#define STOPWATCH_STOP_GENERIC(name) ((void)0)
#endif // USE_DEBUG_STOPWATCH

#endif  /* _Stopwatch_H_ */

