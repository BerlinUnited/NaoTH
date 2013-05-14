/*
 * File:   SimpleGoalColorClassifier.cpp
 * Author: claas
 * 
 */

#include "SimpleGoalColorClassifier.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

using namespace std;

SimpleGoalColorClassifier::SimpleGoalColorClassifier()
{
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:mark_yellow", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:enable_plots", "", false);
}

void SimpleGoalColorClassifier::execute()
{
  PixelT<int> max = getFieldColorPercept().range.getMax();
  PixelT<int> min = getFieldColorPercept().range.getMin();

  //getSimpleGoalColorPercept().minFieldV = min.v;
  //getSimpleGoalColorPercept().maxFieldV = max.v;
  //getSimpleGoalColorPercept().maxFieldU = max.u;

  getSimpleGoalColorPercept().minY = (int) goalParams.goalColorMin.y;
  getSimpleGoalColorPercept().maxY = (int) goalParams.goalColorMax.y;
  getSimpleGoalColorPercept().maxU = min.u;
  getSimpleGoalColorPercept().minV = (int) goalParams.dist2green.v + max.v;
  getSimpleGoalColorPercept().maxDistV = (int) goalParams.goalColorWidth.v;

  getSimpleGoalColorPercept().lastUpdated = getFrameInfo();

  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      if(i < min.u)
      {
        histU[i] = getHistograms().colorChannelHistogramCb[i];
      }
      else
      {
        histU[i] = 0.0;
      }

      if(i > goalParams.dist2green.v + max.v )
      {
        histV[i] = getHistograms().colorChannelHistogramCr[i];
      }
      else
      {
        histV[i] = 0.0;
      }
      PLOT_GENERIC("SimpleGoalColorClassifier:histUorg", i, getHistograms().colorChannelHistogramCb[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:histVorg", i, getHistograms().colorChannelHistogramCr[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:histU", i, histU[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:histV", i, histV[i]);
    }
  );

  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:mark_yellow",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);        
        if
        (
          getSimpleGoalColorPercept().isInside(pixel)
        )
          POINT_PX(ColorClasses::yellow, x, y);        
      }
    }
  );

}//end execute