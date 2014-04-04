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
: 
  cameraID(CameraInfo::Bottom),
  uniformGrid(getImage().width(), getImage().height(), 60, 40)
{
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleFieldColorClassifier:TopCam:markCrClassification", "", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleFieldColorClassifier:BottomCam:markCrClassification", "", false);

  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleFieldColorClassifier:TopCam:mark_green", "", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleFieldColorClassifier:BottomCam:mark_green", "", false);
}

void SimpleFieldColorClassifier::execute(const CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  // check if a fresh histogram is avaliable
  if(!getColorChannelHistograms().colorChannelIsUptodate) {
    return;
  }

  // some usefull precalculations (to speedup the process)
  // the cente of the histogram
  static const int halfChannelWidth = ColorChannelHistograms::VALUE_COUNT / 2;
  static const double histogramStep = 1.0/(double) ColorChannelHistograms::VALUE_COUNT;
  static const double histogramDoubleStep = 2.0*histogramStep;
  
  STOPWATCH_START("SimpleFieldColorClassifier:Cr_filtering");

  // STEP 1: search for the maximal value in the weighted Cr histogramm 
  // CAUTION: the histogram created by the grid provider in the last frame is used
  double maxWeightedCr = 0.0;
  int maxWeightedIndexCr = -1;

  // the histogram is weighted with the function 
  // max^2(0,128-i)/128, i.e., we are interested only in the first half of it
  for(int i = 0; i < halfChannelWidth; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCr = 1.0 - i*histogramDoubleStep;
    double weightedCr = wCr * wCr * (double) getColorChannelHistograms().histogramV.rawData[i];

    // search for maximum in the wighted Cr channel
    if(weightedCr > maxWeightedCr)
    {
      maxWeightedCr = weightedCr;
      maxWeightedIndexCr = i;
    }
  }

  // no green candidates found
  if(maxWeightedIndexCr < 0) {
    return;
  }


  DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:BottomCam:markCrClassification",
    if(cameraID == CameraInfo::Bottom) {
      for(unsigned int x = 0; x < getImage().width(); x++) {
        for(unsigned int y = 0; y < getImage().height(); y++) {
          const Pixel& pixel = getImage().get(x, y);
          if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)getParameters().fieldColorMax.v) {
            POINT_PX(ColorClasses::red, x, y);
          }
        }
      }
    }
  );
  DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:TopCam:markCrClassification",
    if(cameraID == CameraInfo::Top) {
      for(unsigned int x = 0; x < getImage().width(); x++) {
        for(unsigned int y = 0; y < getImage().height(); y++) {
          const Pixel& pixel = getImage().get(x, y);
          if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)getParameters().fieldColorMax.v) {
            POINT_PX(ColorClasses::red, x, y);
          }
        }
      }
    }
  );


  STOPWATCH_STOP("SimpleFieldColorClassifier:Cr_filtering");
  

  // Step 3: calculate the Y and Cb histograms based on the points 
  //         which satisfy the "green"-condition based on Cr histogram
  STOPWATCH_START("SimpleFieldColorClassifier:GridWalk");

  Pixel pixel;
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    getImage().get(point.x, point.y, pixel);
    
    if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)getParameters().fieldColorMax.v )
    {
      filteredHistogramY.add(pixel.y);
      filteredHistogramU.add(pixel.u);
    }
  }

  STOPWATCH_STOP("SimpleFieldColorClassifier:GridWalk");


  STOPWATCH_START("SimpleFieldColorClassifier:Y_filtering");
  
  double maxWeightedCb = 0;
  int maxWeightedIndexCb = -1;

  double meanFieldY = 0;
  double numberOfFieldY = 0;

  for(int i = 0; i < ColorChannelHistograms::VALUE_COUNT; i++)
  {
    // weight based on the mean value  (255 - i)/255
    double wCb = 1.0 - i*histogramStep;
    double weightedCb = wCb * (double) filteredHistogramU.rawData[i];

    // calculate the Cb maximum
    if(weightedCb > maxWeightedCb)
    {
      maxWeightedCb = weightedCb;
      maxWeightedIndexCb = i;
    }

    meanFieldY += filteredHistogramY.rawData[i]*i;
    numberOfFieldY += filteredHistogramY.rawData[i];
  }

  // estimate the mean Y of the green candidates
  if(numberOfFieldY > 0) {
    meanFieldY /= numberOfFieldY;
  }
  
  //TODO: check how it works in other conditions
  int maxWeightedIndexY = (int)Math::clamp(meanFieldY,0.0, 255.0);

  STOPWATCH_STOP("SimpleFieldColorClassifier:Y_filtering");

  PLOT("SimpleFieldColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":maxWeightedIndexCr", maxWeightedIndexCr);
  PLOT("SimpleFieldColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":maxWeightedIndexCb", maxWeightedIndexCb);
  PLOT("SimpleFieldColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":meanFieldY", meanFieldY);

  getFieldColorPercept().range.set(
    maxWeightedIndexY - (int)getParameters().fieldColorMin.y,
    (int)maxWeightedIndexCb - (int)getParameters().fieldColorMax.u,
    (int)maxWeightedIndexCr - (int)getParameters().fieldColorMax.v,

    maxWeightedIndexY + (int)getParameters().fieldColorMax.y,
    (int)maxWeightedIndexCb + (int)getParameters().fieldColorMax.u,
    (int)maxWeightedIndexCr + (int)getParameters().fieldColorMax.v
    );

  getFieldColorPercept().lastUpdated = getFrameInfo();

  if(parameters.classifyInBothImages < 1)
  {
    getFieldColorPerceptTop().range.set(
      maxWeightedIndexY - (int)getParameters().fieldColorMin.y,
      (int)maxWeightedIndexCb - (int)getParameters().fieldColorMax.u,
      (int)maxWeightedIndexCr - (int)getParameters().fieldColorMax.v,

      maxWeightedIndexY + (int)getParameters().fieldColorMax.y,
      (int)maxWeightedIndexCb + (int)getParameters().fieldColorMax.u,
      (int)maxWeightedIndexCr + (int)getParameters().fieldColorMax.v
      );

    getFieldColorPerceptTop().lastUpdated = getFrameInfo();
  }
  

}//end execute
