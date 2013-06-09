/*
 * File:   SimpleFieldColorClassifier.cpp
 * Author: claas
 * 
 * Created on 15. März 2011, 15:56
 */

#include "SimpleFieldColorClassifier.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

using namespace std;

SimpleFieldColorClassifier::SimpleFieldColorClassifier()
  : cameraID(CameraInfo::Bottom)
{
  //DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:weightedHistCr", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:weightedHistY", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:weightedHistCb", " ", false);

  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:TopCam:markCrClassification", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:BottomCam:markCrClassification", "", false);

  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:TopCam:mark_green", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:BottomCam:mark_green", "", false);

  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:TopCam:enable_plots", "", false);
  DEBUG_REQUEST_REGISTER("NeoVision:SimpleFieldColorClassifier:BottomCam:enable_plots", "", false);
}

void SimpleFieldColorClassifier::execute(const CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  // check if a fresh histogram is avaliable
  if(!getHistograms().colorChannelIsUptodate)
  {
    return;
  }

  // some usefull precalculations (to speedup the process)
  // the cente of the histogram
  static const int halfChannelWidth = COLOR_CHANNEL_VALUE_COUNT / 2;
  static const double histogramStep = 1.0/(double) COLOR_CHANNEL_VALUE_COUNT;
  static const double histogramDoubleStep = 2.0*histogramStep;
  
  STOPWATCH_START("SimpleFieldColorClassifier:Cr_filtering");

  // STEP 1: search for the maximal value in the weighted Cr histogramm 
  // CAUTION: the histogram created by the grid provider in the last frame is used
  double maxWeightedCr = 0.0;
  int maxWeightedIndexCr = -1;
  double maxWeightedCrTop = 0.0;
  int maxWeightedIndexCrTop = -1;

  // the histogram is weighted with the function 
  // max(0,128-i)/128, i.e., we are interested only in the first half of it
  for(int i = 0; i < halfChannelWidth; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCr = 1.0 - i*histogramDoubleStep;
    double weightedCr = wCr * (double) getHistograms().histogramV.rawData[i];

    // search for max Cr channel value with weight w
    if(weightedCr > maxWeightedCr)
    {
      maxWeightedCr = weightedCr;
      maxWeightedIndexCr = i;
    }
  }//end for
  

  // no green candidates found
  if(maxWeightedIndexCr < 0) {
    return;
  }


  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:BottomCam:markCrClassification",
    if(cameraID == CameraInfo::Bottom) {
      for(unsigned int x = 0; x < getImage().width(); x++) {
        for(unsigned int y = 0; y < getImage().height(); y++) 
        {
          const Pixel& pixel = getImage().get(x, y);
          if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)fieldParams.fieldColorMax.v)
          {
            POINT_PX(ColorClasses::red, x, y);
          }
        }
      }
    }
  );
  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:TopCam:markCrClassification",
    if(cameraID == CameraInfo::Top) {
      for(unsigned int x = 0; x < getImage().width(); x++) {
        for(unsigned int y = 0; y < getImage().height(); y++) 
        {
          const Pixel& pixel = getImage().get(x, y);
          if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)fieldParams.fieldColorMax.v)
          {
            POINT_PX(ColorClasses::red, x, y);
          }
        }
      }
    }
  );


  STOPWATCH_STOP("SimpleFieldColorClassifier:Cr_filtering");
  

  
  /*
  // Step 3: calculate the Y and Cb histograms based on the points 
  //         which satisfy the "green"-condition based on Cr histogram
  STOPWATCH_START("SimpleFieldColorClassifier:GridWalk");
  double meanFieldY = 0.0;
  double meanFieldCountY = 0.0;
  Pixel pixel;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    getImage().get(point.x, point.y, pixel);
    
    if( abs((int)pixel.v  - (int) maxWeightedIndexCr) < (int) maxDistCr )
    {
      weightedHistY[pixel.y]++;
      weightedHistCb[pixel.u]++;

      meanFieldY += pixel.y;
      meanFieldCountY++;
    }
  }//end for
  
  // estimate the mean Y of the green candidates
  if(meanFieldCountY <= 0.0)
  {
    return;
  }

  meanFieldY /= meanFieldCountY;

  STOPWATCH_STOP("SimpleFieldColorClassifier:GridWalk");
  */

  STOPWATCH_START("SimpleFieldColorClassifier:Y_filtering");
  
  double maxWeightedCb = 0;
  int maxWeightedIndexCb = -1;

  double meanFieldY = 0;
  double numberOfFieldY = 0;

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    // weight based on the mean value  (255 - i)/255
    double wCb = 1.0 - i*histogramStep;
    double weightedCb = wCb * (double) getHistograms().histogramU.rawData[i];

    // calculate the Cb maximum
    if(weightedCb > maxWeightedCb)
    {
      maxWeightedCb = weightedCb;
      maxWeightedIndexCb = i;
    }

    meanFieldY += getHistograms().histogramY.rawData[i]*i;
    numberOfFieldY += getHistograms().histogramY.rawData[i];
  }//end for

  if(numberOfFieldY > 0) {
    meanFieldY /= numberOfFieldY;
  }

  //check how it works in other conditions
  int maxWeightedIndexY = (int)Math::clamp(meanFieldY,0.0, 255.0);

  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:BottomCam:enable_plots",
    if(cameraID == CameraInfo::Bottom) {
      PLOT("SimpleFieldColorClassifier:BottomCam:maxWeightedIndexCr", maxWeightedIndexCr);
      PLOT("SimpleFieldColorClassifier:BottomCam:maxWeightedIndexCb", maxWeightedIndexCb);
      PLOT("SimpleFieldColorClassifier:BottomCam:meanFieldY", meanFieldY);
    }
  );
  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:TopCam:enable_plots",
    if(cameraID == CameraInfo::Top) {
      PLOT("SimpleFieldColorClassifier:BottomCam:maxWeightedIndexCr", maxWeightedIndexCr);
      PLOT("SimpleFieldColorClassifier:BottomCam:maxWeightedIndexCb", maxWeightedIndexCb);
      PLOT("SimpleFieldColorClassifier:BottomCam:meanFieldY", meanFieldY);
    }
  );

  double maxY = 255;
  MODIFY("SimpleFieldColorClassifier:maxY", maxY);
  double minY = 0;
  MODIFY("SimpleFieldColorClassifier:minY", minY);

  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:BottomCam:mark_green",
    if(cameraID == CameraInfo::Bottom) {
      for(unsigned int x = 0; x < getImage().width(); x++)
      {
        for(unsigned int y = 0; y < getImage().height(); y++)
        {
          const Pixel& pixel = getImage().get(x, y);

          if
          (
            min((int)(maxWeightedIndexY + fieldParams.fieldColorMax.y),(int)maxY) > (int)pixel.y
            &&
            max((int)(maxWeightedIndexY - fieldParams.fieldColorMin.y),(int)minY) < (int)pixel.y
            &&
            abs((int)pixel.u -(int)maxWeightedIndexCb) < (int)fieldParams.fieldColorMax.u
            &&
            abs((int)pixel.v -(int)maxWeightedIndexCr) < (int)fieldParams.fieldColorMax.v
          )
            POINT_PX(ColorClasses::green, x, y);
        }
      } 
    }
  );
  DEBUG_REQUEST("NeoVision:SimpleFieldColorClassifier:TopCam:mark_green",
    if(cameraID == CameraInfo::Top) {
      for(unsigned int x = 0; x < getImage().width(); x++)
      {
        for(unsigned int y = 0; y < getImage().height(); y++)
        {
          const Pixel& pixel = getImage().get(x, y);

          if
          (
            min((int)(maxWeightedIndexY + fieldParams.fieldColorMax.y),(int)maxY) > (int)pixel.y
            &&
            max((int)(maxWeightedIndexY - fieldParams.fieldColorMin.y),(int)minY) < (int)pixel.y
            &&
            abs((int)pixel.u -(int)maxWeightedIndexCb) < (int)fieldParams.fieldColorMax.u
            &&
            abs((int)pixel.v -(int)maxWeightedIndexCr) < (int)fieldParams.fieldColorMax.v
          )
            POINT_PX(ColorClasses::green, x, y);
        }
      } 
    }
  );

  /*
  // 
  getFieldColorPercept().distY = fieldParams.fieldColorMax.y;
  getFieldColorPercept().distCb = fieldParams.fieldColorMax.u;
  getFieldColorPercept().distCr = fieldParams.fieldColorMax.v;


  //getFieldColorPercept().maxWeightedY = maxWeightedY;
  getFieldColorPercept().maxWeightedIndexY = maxWeightedIndexY;
  getFieldColorPercept().maxWeightedCb = maxWeightedCb;
  getFieldColorPercept().maxWeightedIndexCb = maxWeightedIndexCb;
  getFieldColorPercept().maxWeightedCr = maxWeightedCr;
  getFieldColorPercept().maxWeightedIndexCr = maxWeightedIndexCr;
  */

  getFieldColorPercept().range.set(
    maxWeightedIndexY - (int)fieldParams.fieldColorMin.y,
    (int)maxWeightedIndexCb - (int)fieldParams.fieldColorMax.u,
    (int)maxWeightedIndexCr - (int)fieldParams.fieldColorMax.v,

    maxWeightedIndexY + (int)fieldParams.fieldColorMax.y,
    (int)maxWeightedIndexCb + (int)fieldParams.fieldColorMax.u,
    (int)maxWeightedIndexCr + (int)fieldParams.fieldColorMax.v
    );

  //getFieldColorPercept().set();
  getFieldColorPercept().lastUpdated = getFrameInfo();
}//end execute