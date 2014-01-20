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
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleGoalColorClassifier:mark_yellow", "", false);

  // experimental (Claas)
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleGoalColorClassifier:enable_plots", "", false);
}

void SimpleGoalColorClassifier::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);
  //PixelT<int> fieldMax = getFieldColorPercept().range.getMax();
  //PixelT<int> fieldMin = getFieldColorPercept().range.getMin();

  //getSimpleGoalColorPercept().minY = (int) goalParams.goalColorMin.y;
  //getSimpleGoalColorPercept().maxY = (int) goalParams.goalColorMax.y;
  //getSimpleGoalColorPercept().maxU = fieldMin.u; // the U value of yellow has to be below green
  //getSimpleGoalColorPercept().minV = (int) goalParams.dist2green.v + fieldMax.v;
  //getSimpleGoalColorPercept().maxDistV = (int) goalParams.goalColorWidth.v;
  getSimpleGoalColorPercept().vu_offset = (int) goalParams.vu_offset;
    
  getSimpleGoalColorPercept().lastUpdated = getFrameInfo();

  DEBUG_REQUEST("Vision:ColorClassifiers:SimpleGoalColorClassifier:mark_yellow",
    for(unsigned int x = 0; x < getImage().width(); x++) {
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
  PixelT<int> fieldMax = getFieldColorPercept().range.getMax();
  PixelT<int> fieldMin = getFieldColorPercept().range.getMin();


  for(int i = 0; i < ColorChannelHistograms::VALUE_COUNT; i++)
  {
    if(i < fieldMin.u) {
      histU.set(i, getColorChannelHistograms().histogramU.rawData[i]);
    } else {
      histU.set(i, 0);
    }

    if(i > fieldMax.v ) {
      histV.set(i, getColorChannelHistograms().histogramV.rawData[i]);
    } else {
      histV.set(i, 0);
    }
  }
  //histU.calculate();
  //histV.calculate();

  //int diff = getBaseColorRegionPercept().spanWidthEnv.y * 1 / 100;
  //double maxY = getBaseColorRegionPercept().maxEnv.y + (255 - getBaseColorRegionPercept().maxEnv.y) * 0.7 - diff;
  //double minY = getBaseColorRegionPercept().maxEnv.y + (255 - getBaseColorRegionPercept().maxEnv.y) * 0.3 - diff;

  //getSimpleGoalColorPercept().minY = (int) minY;//goalParams.goalColorMin.y;
  //getSimpleGoalColorPercept().maxY = (int) maxY;//goalParams.goalColorMax.y;
  //getSimpleGoalColorPercept().maxU = (int) (histU.mean + 1.5 * goalParams.strength * histU.sigma);
  //getSimpleGoalColorPercept().minV = (int) (histV.mean - goalParams.strength * histV.sigma);
  //getSimpleGoalColorPercept().maxDistV = (int) (histV.sigma * 2 * goalParams.strength);


  DEBUG_REQUEST("Vision:ColorClassifiers:SimpleGoalColorClassifier:enable_plots",
    for(int i = 0; i < ColorChannelHistograms::VALUE_COUNT; i++)
    {
      getColorChannelHistogramsTop().histogramU.plotRaw("SimpleGoalColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":histUorg");
      getColorChannelHistogramsTop().histogramV.plotRaw("SimpleGoalColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":histVorg");
      PLOT_GENERIC("SimpleGoalColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":histU", i, histTopU.rawData[i]);
      PLOT_GENERIC("SimpleGoalColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":histV", i, histTopV.rawData[i]);
    }
  );
}//end histogramExperiments
