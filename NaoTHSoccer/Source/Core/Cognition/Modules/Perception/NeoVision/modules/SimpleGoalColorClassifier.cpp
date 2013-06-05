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
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:TopCam:mark_yellow", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:BottomCam:mark_yellow", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:TopCam:enable_plots", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:BottomCam:enable_plots", "", false);
}

void SimpleGoalColorClassifier::execute()
{
  PixelT<int> max = getFieldColorPercept().range.getMax();
  PixelT<int> min = getFieldColorPercept().range.getMin();
  PixelT<int> maxTop = getFieldColorPerceptTop().range.getMax();
  PixelT<int> minTop = getFieldColorPerceptTop().range.getMin();

  //getSimpleGoalColorPercept().minFieldV = min.v;
  //getSimpleGoalColorPercept().maxFieldV = max.v;
  //getSimpleGoalColorPercept().maxFieldU = max.u;

  getSimpleGoalColorPercept().minY = (int) goalParams.goalColorMin.y;
  getSimpleGoalColorPercept().maxY = (int) goalParams.goalColorMax.y;
  getSimpleGoalColorPercept().maxU = min.u;
  getSimpleGoalColorPercept().minV = (int) goalParams.dist2green.v + max.v;
  getSimpleGoalColorPercept().maxDistV = (int) goalParams.goalColorWidth.v;
  getSimpleGoalColorPerceptTop().minY = (int) goalParams.goalColorMin.y;
  getSimpleGoalColorPerceptTop().maxY = (int) goalParams.goalColorMax.y;
  getSimpleGoalColorPerceptTop().maxU = minTop.u;
  getSimpleGoalColorPerceptTop().minV = (int) goalParams.dist2green.v + maxTop.v;
  getSimpleGoalColorPerceptTop().maxDistV = (int) goalParams.goalColorWidth.v;

  getSimpleGoalColorPercept().lastUpdated = getFrameInfo();
  getSimpleGoalColorPerceptTop().lastUpdated = getFrameInfo();

  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:TopCam:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      if(i < min.u)
      {
        histU[i] = getHistogramsTop().histogramU.rawData[i];
      }
      else
      {
        histU[i] = 0.0;
      }

      if(i > goalParams.dist2green.v + max.v )
      {
         histV[i] = getHistogramsTop().histogramV.rawData[i];
      }
      else
      {
        histV[i] = 0.0;
      }
      getHistogramsTop().histogramU.plotRaw("SimpleGoalColorClassifier:TopCam:histUorg");
      getHistogramsTop().histogramV.plotRaw("SimpleGoalColorClassifier:TopCam:histVorg");
      PLOT_GENERIC("SimpleGoalColorClassifier:TopCam:histU", i, histU[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:TopCam:histV", i, histV[i]);
    }
  );
  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:BottomCam:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      if(i < min.u)
      {
        histU[i] = getHistograms().histogramU.rawData[i];
      }
      else
      {
        histU[i] = 0.0;
      }

      if(i > goalParams.dist2green.v + max.v )
      {
         histV[i] = getHistograms().histogramV.rawData[i];
      }
      else
      {
        histV[i] = 0.0;
      }
      getHistograms().histogramU.plotRaw("SimpleGoalColorClassifier:BottomCam:histUorg");
      getHistograms().histogramV.plotRaw("SimpleGoalColorClassifier:BottomCam:histVorg");
      PLOT_GENERIC("SimpleGoalColorClassifier:BottomCam:histU", i, histU[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:BottomCam:histV", i, histV[i]);
    }
  );

  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:TopCam:mark_yellow",
    for(unsigned int x = 0; x < getImageTop().width(); x++)
    {
      for(unsigned int y = 0; y < getImageTop().height(); y++)
      {
        const Pixel& pixel = getImageTop().get(x, y);        
        if
        (
          getSimpleGoalColorPerceptTop().isInside(pixel)
        )
          POINT_PX(ColorClasses::yellow, x, y);        
      }
    }
  );

  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:BottomCam:mark_yellow",
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