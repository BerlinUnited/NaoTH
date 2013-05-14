/*
 * File:   SimpleBallColorClassifier.cpp
 * Author: claas
 * 
 * Created on 15. März 2011, 15:56
 */

#include "SimpleBallColorClassifier.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

using namespace std;

SimpleBallColorClassifier::SimpleBallColorClassifier()
{
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:mark_orange", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:enable_plots", "", false);
}

void SimpleBallColorClassifier::execute()
{
  PixelT<int> max = getFieldColorPercept().range.getMax();
  
  getSimpleBallColorPercept().minV = (int) ballParams.dist2yellow.v + getSimpleGoalColorPercept().minV + getSimpleGoalColorPercept().maxDistV;
  getSimpleBallColorPercept().maxY = (int) ballParams.ballColorMax.y;
  getSimpleBallColorPercept().maxU = (int) ballParams.ballColorMax.u;

  getSimpleBallColorPercept().lastUpdated = getFrameInfo();;

  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      if(i > ballParams.dist2yellow.v + getSimpleGoalColorPercept().minV + getSimpleGoalColorPercept().maxDistV )
      {
        histV[i] = getHistograms().colorChannelHistogramCr[i];
      }
      else
      {
        histV[i] = 0.0;
      }
      PLOT_GENERIC("SimpleBallColorClassifier:histVorg", i, getHistograms().colorChannelHistogramCr[i]);
      PLOT_GENERIC("SimpleBallColorClassifier:histV", i, histV[i]);
    }
  );

  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:mark_orange",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if
        (
          getSimpleBallColorPercept().isInside(pixel)
        )
          POINT_PX(ColorClasses::red, x, y);
      }
    }
  );
}//end execute