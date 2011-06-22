// 
// File:   DebugBufferedOutput.cpp
// Author: heinrich
//
// Created on 19. März 2008, 21:52
//

#include "DebugBufferedOutput.h"

#include "Cognition/CognitionDebugServer.h"

DebugBufferedOutput::DebugBufferedOutput()
  :
  framesSinceLastDOUTSend(0),
  framesSinceLastPlotDataSend(0)
{
  plotsMutex = g_mutex_new();

  REGISTER_DEBUG_COMMAND("latest_output_messages", 
    "return the output messages which where collected in the internal buffer", this);

  REGISTER_DEBUG_COMMAND("plot_values", 
    "return the values to plot which where collected in the internal buffer", this);
}

DebugBufferedOutput::~DebugBufferedOutput()
{
  g_mutex_free(plotsMutex);
}


void DebugBufferedOutput::executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& /*arguments*/,
    std::ostream &outstream)
{

  if(command == "latest_output_messages")
  {    
    framesSinceLastDOUTSend = 0;
    std::string buf = doutOut.str();
    if(buf != "")
    {
      outstream << buf;
      doutOut.str("");
      doutOut.clear();
    }
  }
  else if(command == "plot_values")
  {    
    framesSinceLastPlotDataSend = 0;
    g_mutex_lock(plotsMutex);
    outstream << plots.SerializeAsString();
    plots.clear_plots();
    g_mutex_unlock(plotsMutex);
  }
}//end executeDebugCommand


void DebugBufferedOutput::update()
{
  framesSinceLastDOUTSend++;
  framesSinceLastPlotDataSend++;
  
  if(framesSinceLastDOUTSend > 10)
  {
    // reset output
    doutOut.str("");
    doutOut.clear();

    framesSinceLastDOUTSend = 0;
  }//end if
  
  if(framesSinceLastPlotDataSend > 50)
  {
    g_mutex_lock(plotsMutex);
    plots.clear_plots();
    g_mutex_unlock(plotsMutex);

    framesSinceLastPlotDataSend = 0;
  }//end if
}//end update
