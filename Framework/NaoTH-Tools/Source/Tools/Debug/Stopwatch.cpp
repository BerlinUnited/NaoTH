/* 
 * File:   Stopwatch.cpp
 * Author: thomas
 *
 * Created on 18. April 2008, 17:59
 */

#include "Stopwatch.h"

#include <fstream>

using namespace std;

Stopwatch::Stopwatch() : stopwatches()
{
}

Stopwatch::~Stopwatch()
{ 
  // TODO: there is for sure a better  place for it
  //Stopwatch::getInstance().dump(); 
}


void Stopwatch::notifyStart(const std::string stopWatchName)
{
  stopwatches[stopWatchName].name = stopWatchName;
  notifyStart(stopwatches.find(stopWatchName)->second);
}//end notifyStart

void Stopwatch::notifyStart(StopwatchItem& stopwatchItem) 
{
  stopwatchItem.isValid = false;
 
  #ifdef WIN32
  LARGE_INTEGER highPerformanceTick;
  LARGE_INTEGER freq;
  if(QueryPerformanceCounter(&highPerformanceTick) && QueryPerformanceFrequency(&freq))
  {
    double inSeconds = ((double) highPerformanceTick.LowPart) / ((double) freq.LowPart);
    unsigned long timeInMicro = (unsigned long) (inSeconds * 1000000.0);
    stopwatchItem.start = timeInMicro;
    stopwatchItem.stop = timeInMicro;
  }
  #else
  struct timeval t;
  if(gettimeofday(&t, NULL) == 0)
  {
    unsigned int timeInMicro = t.tv_sec * 1000000 + t.tv_usec;
    stopwatchItem.start = timeInMicro;
    stopwatchItem.stop = timeInMicro;
  }
  #endif // WIN32
}//end notifyStart


void Stopwatch::notifyStop(const std::string stopWatchName)
{
  notifyStop(stopwatches[stopWatchName]);
}//end notifyStop

void Stopwatch::notifyStop(StopwatchItem& stopwatchItem)
{
  #ifdef WIN32
  LARGE_INTEGER highPerformanceTick;
  LARGE_INTEGER freq;
  if(QueryPerformanceCounter(&highPerformanceTick) && QueryPerformanceFrequency(&freq))
  {
    double inSeconds = ((double) highPerformanceTick.LowPart) / ((double) freq.LowPart);
    unsigned long timeInMicro = (unsigned long) (inSeconds * 1000000.0);
    stopwatchItem.stop = timeInMicro;
    stopwatchItem.isValid = true;
  }
  
  #else
  struct timeval t;
  if(gettimeofday(&t, NULL) == 0)
  {
    unsigned int timeInMicro = t.tv_sec * 1000000 + t.tv_usec;
    stopwatchItem.stop = timeInMicro;
    stopwatchItem.isValid = true;
  }
  #endif //WIN32


  // update the statistics of the item
  stopwatchItem.n++;
  float value = static_cast<float>(stopwatchItem.stop - stopwatchItem.start) * 0.001f; // in ms
  stopwatchItem.min = min(stopwatchItem.min, value);
  stopwatchItem.max = max(stopwatchItem.max, value);
  // update the mean iteratively
  // c(n) = c(n-1) + (x_n - c(n-1))/n
  stopwatchItem.mean += (value - stopwatchItem.mean)/stopwatchItem.n;

}//end notifyStop



StopwatchItem& Stopwatch::getStopwatchReference(const std::string stopWatchName)
{
  // add value if not existing
  StopwatchItem& stopwatchItem = stopwatches[stopWatchName];
  stopwatchItem.name = stopWatchName;
    
  return stopwatchItem;
}//end getStopwatchReference


void Stopwatch::dump()
{
  ofstream outputFileStream ( "Config/naoth_stopwatch.dump" , ofstream::out );
  
  std::map<std::string, StopwatchItem>::const_iterator it = stopwatches.begin();
  while (it != stopwatches.end())
  {
//    const std::string& name = it->first;
    const StopwatchItem& item = it->second;

    if (item.isValid) 
    {
      outputFileStream << item.name <<
        ';' << item.min << ';' << item.mean << ';' << item.max << ';' << endl;
    }

    it++;
  }//end while

  outputFileStream.close();
}//end dump
