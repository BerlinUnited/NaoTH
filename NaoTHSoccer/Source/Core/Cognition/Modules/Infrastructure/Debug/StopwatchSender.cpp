/*
 * File:   ParameterListDebugLoader.cpp
 * Author: thomas
 *
 */
#include "StopwatchSender.h"

#include "Core/Cognition/CognitionDebugServer.h"
#include <Tools/Debug/Stopwatch.h>
#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

StopwatchSender::StopwatchSender()
{
  REGISTER_DEBUG_COMMAND("stopwatch",
    "get values of all the stopwatches in microseconds", this);

  //DEBUG_REQUEST_REGISTER("DataPlot:Stopwatch","Plot Stopwatches in ms (defined in Stopwatch.cpp)", false);
}

void StopwatchSender::execute()
{
  
}

void StopwatchSender::executeDebugCommand(const std::string& command, const std::map<std::string,std::string>& arguments, std::stringstream& outstream)
{
  if ("stopwatch" == command)
  {
    std::map<std::string, StopwatchItem>& stopwatches = Stopwatch::getInstance().stopwatches;
    naothmessages::Stopwatches all;
    // collect all values
    std::map<std::string, StopwatchItem>::const_iterator it = stopwatches.begin();
    while (it != stopwatches.end())
    {
      const std::string& name = it->first;
      const StopwatchItem& item = it->second;

      if (item.isValid)
      {
        naothmessages::StopwatchItem* s = all.add_stopwatches();
        s->set_name(name);
        s->set_time(item.stop - item.start);
      }

      it++;
    }//end while
    
    google::protobuf::io::OstreamOutputStream buf(&outstream);
    all.SerializeToZeroCopyStream(&buf);
    
  }
}

StopwatchSender::~StopwatchSender()
{
}
