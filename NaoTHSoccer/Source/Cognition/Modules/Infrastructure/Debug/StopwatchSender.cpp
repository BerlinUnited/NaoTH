/*
 * File:   ParameterListDebugLoader.cpp
 * Author: thomas
 *
 */
#include "StopwatchSender.h"

#include <Tools/Debug/DebugBufferedOutput.h>
#include <DebugCommunication/DebugCommandManager.h>
#include <Tools/Debug/Stopwatch.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

StopwatchSender::StopwatchSender()
{
  REGISTER_DEBUG_COMMAND("stopwatch", "get values of all the stopwatches in microseconds", this);

  DEBUG_REQUEST_REGISTER("Plot:Stopwatch", "Plot Stopwatches in ms (defined in Stopwatch.cpp)", false);
}

StopwatchSender::~StopwatchSender()
{
}

void StopwatchSender::execute()
{
  DEBUG_REQUEST("Plot:Stopwatch",
    const StopwatchManager::StopwatchMap& stopwatches = StopwatchManager::getInstance().getStopwatches();
    StopwatchManager::StopwatchMap::const_iterator it = stopwatches.begin();
    for (; it != stopwatches.end(); it++)
    {
      const Stopwatch& item = it->second;
      
      std::stringstream s;
      s << "SW:" << it->first;
      PLOT_GENERIC(s.str(), getFrameInfo().getTime(), item.lastValue);
    }//end for
  );
}//end execute

void StopwatchSender::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& /*arguments*/, std::ostream& outstream)
{
  if ("stopwatch" == command)
  {
    static naothmessages::Stopwatches all;
    all.Clear();
    //all.mutable_stopwatches()->Reserve(stopwatches.size());

    const StopwatchManager::StopwatchMap& stopwatches = StopwatchManager::getInstance().getStopwatches();
    StopwatchManager::StopwatchMap::const_iterator it = stopwatches.begin();

    for (; it != stopwatches.end(); ++it)
    {
      naothmessages::StopwatchItem* s = all.add_stopwatches();
      s->set_name(it->first);
      s->set_time(it->second.lastValue);
    }
    
    google::protobuf::io::OstreamOutputStream buf(&outstream);
    all.SerializeToZeroCopyStream(&buf);
  }
}//end executeDebugCommand

