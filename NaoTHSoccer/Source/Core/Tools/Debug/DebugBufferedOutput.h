// 
// File:   DebugBufferedOutput.h
// Author: thomas
//
// Created on 19. März 2008, 21:51
//

#ifndef _DebugBufferedOutput_H
#define	_DebugBufferedOutput_H

#include <cstring>
#include <sstream>
#include <glib.h>

#include <Tools/DataStructures/Singleton.h>
#include "Tools/NaoTime.h"
#include "Tools/Math/Common.h"

#include <DebugCommunication/DebugCommandExecutor.h>
#include <Messages/Messages.pb.h>

using namespace std;

class DebugBufferedOutput : public naoth::Singleton<DebugBufferedOutput>, public DebugCommandExecutor
{
protected:
  friend class naoth::Singleton<DebugBufferedOutput>;
  DebugBufferedOutput();
  ~DebugBufferedOutput();

public:

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);
  
  void update();

  void addPlot(std::string name, double value)
  {
    if(g_mutex_trylock(plotsMutex) == 0)
    {
      naothmessages::PlotItem* item = plots.add_plots();
      g_mutex_unlock(plotsMutex);

      item->set_type(naothmessages::PlotItem_PlotType_Default);
      item->set_name(name);
      item->set_x(naoth::NaoTime::getNaoTimeInMilliSeconds());
      item->set_y(value);
    }
  }//end addPlot

  void addPlot2D(std::string name, double x, double y)
  {
    if(g_mutex_trylock(plotsMutex) == 0)
    {
      naothmessages::PlotItem* item = plots.add_plots();
      g_mutex_unlock(plotsMutex);

      item->set_type(naothmessages::PlotItem_PlotType_Plot2D);
      item->set_name(name);
      item->set_x(x);
      item->set_y(y);
    }
  }//end addPlot2D

  void addPlotOrigin2D(std::string name, double x, double y, double rotation)
  {
    if(g_mutex_trylock(plotsMutex) == 0)
    {
      naothmessages::PlotItem* item = plots.add_plots();
      g_mutex_unlock(plotsMutex);

      item->set_type(naothmessages::PlotItem_PlotType_Origin2D);
      item->set_name(name);
      item->set_x(x);
      item->set_y(y);
      item->set_rotation(rotation);
    }
  }

  stringstream doutOut;

private:

  unsigned int framesSinceLastDOUTSend;
  unsigned int framesSinceLastPlotDataSend;

  naothmessages::Plots plots;

  GMutex* plotsMutex;
};


#ifdef DEBUG
/** Debug output stream, usage like "DOUT("ball_pos:" << x << ", " << y */
#define DOUT(arg) DebugBufferedOutput::getInstance().doutOut << arg
#define PLOT(id,value) ASSERT(!Math::isInf(value)&&!Math::isNan(value)); DebugBufferedOutput::getInstance().addPlot(id,value)
#define PLOT2D(id,x,y) ASSERT(!Math::isInf(x)&&!Math::isNan(x)); ASSERT(!Math::isInf(y)&&!Math::isNan(y)); DebugBufferedOutput::getInstance().addPlot2D(id, x, y)
/** set the origin of the 2D plot. It used for visualization of traces on the field etc. */
#define ORIGIN2D(id,x,y,rotation) DebugBufferedOutput::getInstance().addPlotOrigin2D(id, x, y, rotation)
#else
/* ((void)0) - that's a do-nothing statement */
#define DOUT(arg) ((void)0)
#define PLOT(id,value) ((void)0)
#define PLOT2D(id,x,y) ((void)0)
#define ORIGIN2D(id,x,y,rotation) ((void)0)
#endif //DEBUG

#endif	/* _DebugBufferedOutput_H */

