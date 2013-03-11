/* 
 * File:   Stopwatch.cpp
 * Author: thomas
 *
 * Created on 18. April 2008, 17:59
 */

#include "Stopwatch.h"

#include <sstream>

#include "Tools/SynchronizedFileWriter.h"
#include "Tools/NaoTime.h"

Stopwatch::Stopwatch() 
  : 
  begin(0), 
  end(0),
  isValid(false),
  lastValue(0),
  mean(0.0f),
  n(0.0f),
  min(0),
  max(0.0f)
{
}

void Stopwatch::start()
{
  isValid = false;
 
  begin = naoth::NaoTime::getSystemTimeInMicroSeconds();
}//end start


void Stopwatch::stop()
{
  end = naoth::NaoTime::getSystemTimeInMicroSeconds();
  
  // we assume, that difference doesn't exeed the size of int
  lastValue = static_cast<unsigned int>(end - begin);

  // update the statistics of the item
  n++;
  double value = static_cast<double>(lastValue) * 0.001; // in ms
  min = std::min(min, value);
  max = std::max(max, value);
  // update the mean iteratively
  // c(n) = c(n-1) + (x_n - c(n-1))/n
  mean += (value - mean)/n;

  isValid = true;
}//end stop



StopwatchManager::StopwatchManager()
{
}

StopwatchManager::~StopwatchManager()
{ 
  // TODO: there is for sure a better  place for it
  //Stopwatch::getInstance().dump(); 
}


void StopwatchManager::notifyStart(const std::string& name)
{
  stopwatches[name].start();
}//end notifyStart


void StopwatchManager::notifyStop(const std::string& name)
{
  stopwatches[name].stop();
}//end notifyStop



Stopwatch& StopwatchManager::getStopwatchReference(const std::string& name)
{
  // add value if not existing
  return stopwatches[name];
}//end getStopwatchReference


void StopwatchManager::dump(std::string name) const
{
  std::stringstream outputStream;
  
  StopwatchMap::const_iterator it = stopwatches.begin();
  for (; it != stopwatches.end(); it++)
  {
    const std::string& name = it->first;
    const Stopwatch& item = it->second;

    if (item.isValid) 
    {
      outputStream  << name << ';' 
                    << item.min << ';' 
                    << item.mean << ';' 
                    << item.max << ';' 
                    << std::endl;
    }
  }//end for

  // write to file
  std::stringstream s;
  if(name != "")
  {
    s << name << ".";
  }
  s << "stopwatch.dump";

  SynchronizedFileWriter::saveStreamToFile(outputStream, s.str());
}//end dump
