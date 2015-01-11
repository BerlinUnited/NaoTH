// 
// File:   DebugPlot.h
// Author: thomas
//
// Created on 19. march 2008, 21:51
//

#ifndef _DebugPlot_H
#define _DebugPlot_H

#include <cstring>

#include <Tools/DataStructures/RingBuffer.h>
#include "Tools/NaoTime.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/DataStructures/Serializer.h"
#include <DebugCommunication/DebugCommandExecutor.h>

class DebugPlot: public DebugCommandExecutor
{
public:
  static const int maxPlotBufferSize = 1024;
  typedef RingBuffer<Vector2d, maxPlotBufferSize> PlotStrokeBuffer;
  typedef std::map<std::string, PlotStrokeBuffer> PlotStrokesMap;

  void addPlot(const std::string& name, double x, double y) {
    plotStrokes[name].add(Vector2d(x,y));
  }

  const PlotStrokesMap& getPlots() const {
    return plotStrokes;
  }

  void clear() {
    for(PlotStrokesMap::iterator iter = plotStrokes.begin(); iter != plotStrokes.end(); ++iter) {
      iter->second.clear();
    }
  }
  
  virtual void executeDebugCommand(
      const std::string& command, const ArgumentMap& arguments,
      std::ostream &outstream);

private:
  PlotStrokesMap plotStrokes;
};

namespace naoth
{
template<>
class Serializer<DebugPlot>
{
  public:
  static void serialize(const DebugPlot& object, std::ostream& stream);
  static void deserialize(std::istream& stream, DebugPlot& object);
};
}

#ifdef DEBUG
/** Debug output stream, usage like "DOUT("ball_pos:" << x << ", " << y */
#define PLOT_GENERIC(id,x,y) { ASSERT(!Math::isInf(x)&&!Math::isNan(x)); ASSERT(!Math::isInf(y)&&!Math::isNan(y)); getDebugPlot().addPlot(id,x,y); } ((void)0)
#define PLOT(id,value) DEBUG_REQUEST_SLOPPY(std::string("Plot:")+std::string(id), PLOT_GENERIC(id, getFrameInfo().getTime() ,value); )
#else
/* ((void)0) - that's a do-nothing statement */
#define PLOT_GENERIC(id,x,y) ((void)0)
#define PLOT(id,value) ((void)0)
#endif //DEBUG

#endif  /* _DebugBufferedOutput_H */

