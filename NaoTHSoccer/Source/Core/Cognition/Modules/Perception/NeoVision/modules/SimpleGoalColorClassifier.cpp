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

  // experimental (Claas)
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:TopCam:enable_plots", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleGoalColorClassifier:BottomCam:enable_plots", "", false);
}

void SimpleGoalColorClassifier::execute()
{
  PixelT<int> fieldMaxTop = getFieldColorPerceptTop().range.getMax();
  PixelT<int> fieldMinTop = getFieldColorPerceptTop().range.getMin();
  PixelT<int> fieldMax = getFieldColorPercept().range.getMax();
  PixelT<int> fieldMin = getFieldColorPercept().range.getMin();

  getSimpleGoalColorPercept().minY = (int) goalParams.goalColorMin.y;
  getSimpleGoalColorPercept().maxY = (int) goalParams.goalColorMax.y;
  getSimpleGoalColorPercept().maxU = fieldMin.u; // the U value of yellow has to be below green
  getSimpleGoalColorPercept().minV = (int) goalParams.dist2green.v + fieldMax.v;
  getSimpleGoalColorPercept().maxDistV = (int) goalParams.goalColorWidth.v;
  
  getSimpleGoalColorPerceptTop().minY = (int) goalParams.goalColorMin.y;
  getSimpleGoalColorPerceptTop().maxY = (int) goalParams.goalColorMax.y;
  getSimpleGoalColorPerceptTop().maxU = fieldMinTop.u; // the U value of yellow has to be below green
  getSimpleGoalColorPerceptTop().minV = (int) goalParams.dist2green.v + fieldMaxTop.v;
  getSimpleGoalColorPerceptTop().maxDistV = (int) goalParams.goalColorWidth.v;
  
  getSimpleGoalColorPercept().lastUpdated = getFrameInfo();
  getSimpleGoalColorPerceptTop().lastUpdated = getFrameInfo();


  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:TopCam:mark_yellow",
    for(unsigned int x = 0; x < getImageTop().width(); x++)
    {
      for(unsigned int y = 0; y < getImageTop().height(); y++)
      {
        const Pixel& pixel = getImageTop().get(x, y);        
        if(getSimpleGoalColorPerceptTop().isInside(pixel)) {
          TOP_POINT_PX(ColorClasses::yellow, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:BottomCam:mark_yellow",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);        
        if( getSimpleGoalColorPercept().isInside(pixel) ) {
          POINT_PX(ColorClasses::yellow, x, y);        
        }
      }
    }
  );

}//end execute


void SimpleGoalColorClassifier::histogramExperiments()
{
  PixelT<int> fieldMaxTop = getFieldColorPerceptTop().range.getMax();
  PixelT<int> fieldMinTop = getFieldColorPerceptTop().range.getMin();
  PixelT<int> fieldMax = getFieldColorPercept().range.getMax();
  PixelT<int> fieldMin = getFieldColorPercept().range.getMin();


  for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    if(i < fieldMinTop.u) {
      histTopU.set(i, getHistogramsTop().histogramU.rawData[i]);
    } else {
      histTopU.set(i, 0);
    }
    
    if(i < fieldMin.u) {
      histU.set(i, getHistograms().histogramU.rawData[i]);
    } else {
      histU.set(i, 0);
    }

    if(i > fieldMaxTop.v ) {
      histTopV.set(i, getHistogramsTop().histogramV.rawData[i]);
    } else {
      histV.set(i, 0);
    }

    if(i > fieldMax.v ) {
      histV.set(i, getHistograms().histogramV.rawData[i]);
    } else {
      histV.set(i, 0);
    }
  }
  //histTopU.calculate();
  //histTopV.calculate();
  //histU.calculate();
  //histV.calculate();

  //int diffTop = getBaseColorRegionPerceptTop().spanWidthEnv.y * 1 / 100;
  //int diff = getBaseColorRegionPercept().spanWidthEnv.y * 1 / 100;
  //double maxTopY = getBaseColorRegionPerceptTop().maxEnv.y + (255 - getBaseColorRegionPerceptTop().maxEnv.y) * 0.7 - diffTop;
  //double maxY = getBaseColorRegionPercept().maxEnv.y + (255 - getBaseColorRegionPercept().maxEnv.y) * 0.7 - diff;
  //double minTopY = getBaseColorRegionPerceptTop().maxEnv.y + (255 - getBaseColorRegionPerceptTop().maxEnv.y) * 0.3 - diffTop;
  //double minY = getBaseColorRegionPercept().maxEnv.y + (255 - getBaseColorRegionPercept().maxEnv.y) * 0.3 - diff;

  //getSimpleGoalColorPerceptTop().minY = (int) minTopY;//goalParams.goalColorMin.y;
  //getSimpleGoalColorPerceptTop().maxY = (int) maxTopY;//goalParams.goalColorMax.y;
  //getSimpleGoalColorPerceptTop().maxU = (int) (histTopU.mean + 1.5 * goalParams.strength * histTopU.sigma);
  //getSimpleGoalColorPerceptTop().minV = (int) (histTopV.mean - goalParams.strength * histTopV.sigma);
  //getSimpleGoalColorPerceptTop().maxDistV = (int) (histTopV.sigma * 2 * goalParams.strength);

  //getSimpleGoalColorPercept().minY = (int) minY;//goalParams.goalColorMin.y;
  //getSimpleGoalColorPercept().maxY = (int) maxY;//goalParams.goalColorMax.y;
  //getSimpleGoalColorPercept().maxU = (int) (histU.mean + 1.5 * goalParams.strength * histU.sigma);
  //getSimpleGoalColorPercept().minV = (int) (histV.mean - goalParams.strength * histV.sigma);
  //getSimpleGoalColorPercept().maxDistV = (int) (histV.sigma * 2 * goalParams.strength);


  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:TopCam:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      getHistogramsTop().histogramU.plotRaw("SimpleGoalColorClassifier:TopCam:histUorg");
      getHistogramsTop().histogramV.plotRaw("SimpleGoalColorClassifier:TopCam:histVorg");
      PLOT_GENERIC("SimpleGoalColorClassifier:TopCam:histU", i, histTopU.rawData[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:TopCam:histV", i, histTopV.rawData[i]);
    }
  );
  DEBUG_REQUEST("NeoVision:SimpleGoalColorClassifier:BottomCam:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      getHistograms().histogramU.plotRaw("SimpleGoalColorClassifier:BottomCam:histUorg");
      getHistograms().histogramV.plotRaw("SimpleGoalColorClassifier:BottomCam:histVorg");
      PLOT_GENERIC("SimpleGoalColorClassifier:BottomCam:histU", i, histU.rawData[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:BottomCam:histV", i, histV.rawData[i]);
    }
  );
}//end histogramExperiments
