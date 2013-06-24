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
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:TopCam:mark_orange", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:BottomCam:mark_orange", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:TopCam:enable_plots", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:BottomCam:enable_plots", "", false);

  DEBUG_REQUEST_REGISTER("NeoVision:SimpleBallColorClassifier:TopCam:mark_max_ball_color", "", false);
}

void SimpleBallColorClassifier::execute(const CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  //PixelT<int> max = getFieldColorPercept().range.getMax();
  //PixelT<int> maxTop = getFieldColorPerceptTop().range.getMax();
  
  getSimpleBallColorPercept().minV = (int) ballParams.dist2yellow.v + getSimpleGoalColorPercept().minV + getSimpleGoalColorPercept().maxDistV;
  getSimpleBallColorPercept().maxY = (int) ballParams.ballColorMax.y;
  getSimpleBallColorPercept().maxU = (int) ballParams.ballColorMax.u;
  getSimpleBallColorPerceptTop().minV = (int) ballParams.dist2yellow.v + getSimpleGoalColorPerceptTop().minV + getSimpleGoalColorPerceptTop().maxDistV;
  getSimpleBallColorPerceptTop().maxY = (int) ballParams.ballColorMax.y;
  getSimpleBallColorPerceptTop().maxU = (int) ballParams.ballColorMax.u;

  getSimpleBallColorPercept().lastUpdated = getFrameInfo();;
  getSimpleBallColorPerceptTop().lastUpdated = getFrameInfo();;

  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:TopCam:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      if(i > ballParams.dist2yellow.v + getSimpleGoalColorPerceptTop().minV + getSimpleGoalColorPerceptTop().maxDistV )
      {
        histV[i] = getHistogramsTop().histogramV.rawData[i];
      }
      else
      {
        histV[i] = 0.0;
      }
      getHistogramsTop().histogramV.plotRaw("SimpleBallColorClassifier:TopCam:histVorg");
      PLOT_GENERIC("SimpleBallColorClassifier:TopCam:histV", i, histV[i]);
    }
  );
  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:BottomCam:enable_plots",
    for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
    {
      if(i > ballParams.dist2yellow.v + getSimpleGoalColorPercept().minV + getSimpleGoalColorPercept().maxDistV )
      {
        histV[i] = getHistograms().histogramV.rawData[i];
      }
      else
      {
        histV[i] = 0.0;
      }
      getHistograms().histogramV.plotRaw("SimpleBallColorClassifier:BottomCam:histVorg");
      PLOT_GENERIC("SimpleBallColorClassifier:BottomCam:histV", i, histV[i]);
    }
  );

  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:TopCam:mark_orange",
    for(unsigned int x = 0; x < getImageTop().width(); x++)
    {
      for(unsigned int y = 0; y < getImageTop().height(); y++)
      {
        const Pixel& pixel = getImageTop().get(x, y);
        if
        (
          getSimpleBallColorPerceptTop().isInside(pixel)
        )
          TOP_POINT_PX(ColorClasses::red, x, y);
      }
    }
  );
  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:BottomCam:mark_orange",
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


  // Heinrich test
  DEBUG_REQUEST("NeoVision:SimpleBallColorClassifier:TopCam:mark_max_ball_color",
    if(cameraID == CameraInfo::Top) {
      Vector2<int> maxPos;
      int maxCr = 0;
      for(unsigned int x = 10; x < getImageTop().width()-10; x++) {
        for(unsigned int y = 10; y < getImageTop().height()-10; y++) {
          const Pixel& pixel = getImageTop().get(x, y);

          if(pixel.v > maxCr) {
            maxCr = pixel.v;
            maxPos.x = x; maxPos.y = y;
          }
        }
      }//end for x
      CIRCLE_PX(ColorClasses::orange, maxPos.x, maxPos.y, 10);
    }
  );
}//end execute
