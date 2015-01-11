// 
// File:   DebugPlot.cpp
// Author: heinrich
//
// Created on 19. march 2008, 21:52
//

#include "DebugPlot.h"

#include <DebugCommunication/DebugCommandManager.h>

#include <Messages/Messages.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

void naoth::Serializer<DebugPlot>::serialize(const DebugPlot& r, std::ostream& stream)
{
  naothmessages::Plots plotsMsg;

  // serialize all the plots
  DebugPlot::PlotStrokesMap::const_iterator iter;
  for(iter = r.getPlots().begin(); iter != r.getPlots().end(); ++iter)
  {
    naothmessages::PlotStroke2D* plotMsg = plotsMsg.add_plotstrokes();
    plotMsg->set_name(iter->first);

    const DebugPlot::PlotStrokeBuffer& plotBuffer = iter->second;

    if(plotBuffer.size() > 0) {
      for(int i = plotBuffer.size()-1; i >= 0; i--)
      {
        naothmessages::DoubleVector2* point = plotMsg->add_points();
        point->set_x(plotBuffer[i].x);
        point->set_y(plotBuffer[i].y);
      }
    }
  }//end for

  google::protobuf::io::OstreamOutputStream buf(&stream);
  plotsMsg.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<DebugPlot>::deserialize(std::istream& /*stream*/, DebugPlot& /*object*/)
{
}

void DebugPlot::executeDebugCommand(
  const std::string& command, const ArgumentMap& /*arguments*/,
  std::ostream &outstream)
{
  if ( command == "DebugPlot:get" )
  {
    naoth::Serializer<DebugPlot>::serialize(*this, outstream);
    clear();
  } 
}
