/* 
 * File:   Stopwatch.cpp
 * Author: thomas
 *
 * Created on 18. April 2008, 17:59
 */

#include "Stopwatch.h"

#include <glib.h>
#include <sstream>

#include "Tools/SynchronizedFileWriter.h"
#include "Tools/NaoTime.h"

using namespace std;

Stopwatch::Stopwatch() : stopwatches()
{
}

Stopwatch::~Stopwatch()
{ 
  // TODO: there is for sure a better  place for it
  //Stopwatch::getInstance().dump(); 
}


void Stopwatch::notifyStart(const std::string& stopWatchName)
{
  stopwatches[stopWatchName].name = stopWatchName;
  notifyStart(stopwatches.find(stopWatchName)->second);
}//end notifyStart


void Stopwatch::notifyStop(const std::string& stopWatchName)
{
  notifyStop(stopwatches[stopWatchName]);
}//end notifyStop

void StopwatchItem::_start()
{
  isValid = false;
 
  unsigned long long timeInMicro = naoth::NaoTime::getSystemTimeInMicroSeconds();
  start = timeInMicro;
  stop = timeInMicro;
}//end _start

void Stopwatch::notifyStart(StopwatchItem& stopwatchItem) 
{
  //g_debug("stopwatch START %s", stopwatchItem.name.c_str());
  stopwatchItem._start();
}//end notifyStart

void StopwatchItem::_stop()
{
  unsigned long long timeInMicro = naoth::NaoTime::getSystemTimeInMicroSeconds();
  this->stop = timeInMicro;
  this->isValid = true;

  // update the statistics of the item
  this->n++;
  float value = static_cast<float>(this->stop - this->start) * 0.001f; // in ms
  this->min = std::min(this->min, value);
  this->max = std::max(this->max, value);
  // update the mean iteratively
  // c(n) = c(n-1) + (x_n - c(n-1))/n
  this->mean += (value - this->mean)/this->n;
  //g_debug("stopwatch STOP %s", stopwatchItem.name.c_str());
  this->lastValue = (unsigned int)(this->stop - this->start);
}//end stop_


void Stopwatch::notifyStop(StopwatchItem& stopwatchItem)
{
  stopwatchItem._stop();
}//end notifyStop



StopwatchItem& Stopwatch::getStopwatchReference(const std::string& stopWatchName)
{
  // add value if not existing
  StopwatchItem& stopwatchItem = stopwatches[stopWatchName];
  stopwatchItem.name = stopWatchName;
    
  return stopwatchItem;
}//end getStopwatchReference


void Stopwatch::dump(std::string name)
{
  stringstream outputStream;
  
  std::map<std::string, StopwatchItem>::const_iterator it = stopwatches.begin();
  while (it != stopwatches.end())
  {
//    const std::string& name = it->first;
    const StopwatchItem& item = it->second;

    if (item.isValid) 
    {
      outputStream << item.name <<
        ';' << item.min << ';' << item.mean << ';' << item.max << ';' << endl;
    }

    it++;
  }//end while

  // write to file
  std::stringstream s;
  if(name != "")
  {
    s << name << ".";
  }
  s << "stopwatch.dump";
  SynchronizedFileWriter::saveStreamToFile(outputStream, s.str());
}//end dump
