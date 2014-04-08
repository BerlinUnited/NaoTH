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
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleFieldColorClassifier:TopCam:markYClassification", "", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassifiers:SimpleFieldColorClassifier:BottomCam:markYClassification", "", false);
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

  const Statistics::HistogramX& histY = getColorChannelHistograms().histogramY;
  double start = histY.min;
  double halfSpan = histY.spanWidth / 2.0;
  double quadSpan = histY.spanWidth / 4.0;

  for(int i = 0; i < getColorChannelHistograms().histogramY.size; i++)
  {
    double f = 1.0;
    f =  gauss(parameters.filterFactorY * quadSpan, start + halfSpan, i);

    int val = (int) Math::round(getColorChannelHistograms().histogramY.rawData[i] * f);
    filteredHistogramY.add(i, val);
  }
  filteredHistogramY.calculate();

  double lowBorderY = filteredHistogramY.median - parameters.filterFactorY * filteredHistogramY.sigma;
  double highBorderY = filteredHistogramY.median + parameters.filterFactorY * filteredHistogramY.sigma;
  Pixel pixel;

  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);

    getImage().get(point.x, point.y, pixel);

    if(!getBodyContour().isOccupied(point) && lowBorderY < pixel.y && pixel.y < highBorderY)
    {
      filteredHistogramV.add(pixel.v);
    }
  }//end for


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
    double weightedCr = wCr * wCr * (double) filteredHistogramV.rawData[i];

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
  STOPWATCH_STOP("SimpleFieldColorClassifier:Cr_filtering");
  

  // Step 3: calculate the Y and Cb histograms based on the points 
  //         which satisfy the "green"-condition based on Cr histogram
  STOPWATCH_START("SimpleFieldColorClassifier:GridWalk");
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    getImage().get(point.x, point.y, pixel);
    
    if(!getBodyContour().isOccupied(point) && abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)getParameters().fieldColorMax.v)
    {
      filteredHistogramU.add(pixel.u);
    }
  }
  STOPWATCH_STOP("SimpleFieldColorClassifier:GridWalk");


  STOPWATCH_START("SimpleFieldColorClassifier:Y_filtering");  
  double maxWeightedCb = 0;
  int maxWeightedIndexCb = -1;

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
  }
  STOPWATCH_STOP("SimpleFieldColorClassifier:Y_filtering");

  PLOT("SimpleFieldColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":maxWeightedIndexCr", maxWeightedIndexCr);
  PLOT("SimpleFieldColorClassifier:" + getImage().cameraInfo.getCameraIDName(cameraID) + ":maxWeightedIndexCb", maxWeightedIndexCb);

  getFieldColorPercept().range.set(
    (int)lowBorderY,
    (int)maxWeightedIndexCb - (int)getParameters().fieldColorMax.u,
    (int)maxWeightedIndexCr - (int)getParameters().fieldColorMax.v,

    (int)highBorderY,
    (int)maxWeightedIndexCb + (int)getParameters().fieldColorMax.u,
    (int)maxWeightedIndexCr + (int)getParameters().fieldColorMax.v
    );

  getFieldColorPercept().histogramField.y = (unsigned char)Math::clamp(filteredHistogramY.median,0,255);
  getFieldColorPercept().histogramField.u = (unsigned char)Math::clamp(maxWeightedIndexCb,0,255);
  getFieldColorPercept().histogramField.v = (unsigned char)Math::clamp(maxWeightedIndexCr,0,255);

  getFieldColorPercept().lastUpdated = getFrameInfo();

  if(parameters.classifyInBothImages < 1)
  {
    getFieldColorPerceptTop().range.set(
      (int)lowBorderY,
      (int)maxWeightedIndexCb - (int)getParameters().fieldColorMax.u,
      (int)maxWeightedIndexCr - (int)getParameters().fieldColorMax.v,

      (int)highBorderY,
      (int)maxWeightedIndexCb + (int)getParameters().fieldColorMax.u,
      (int)maxWeightedIndexCr + (int)getParameters().fieldColorMax.v
      );

    getFieldColorPerceptTop().lastUpdated = getFrameInfo();
  }
  
  cameraID = CameraInfo::Bottom;
  CANVAS_PX(cameraID);
  DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:BottomCam:markCrClassification",
    for(unsigned int x = 0; x < getImage().width(); x+=2) {
      for(unsigned int y = 0; y < getImage().height(); y+=2) {
        const Pixel& pixel = getImage().get(x, y);
        if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)getParameters().fieldColorMax.v) {
          POINT_PX(ColorClasses::red, x, y);
        }
      }
    }
  );
  cameraID = CameraInfo::Top;
  CANVAS_PX(cameraID);
  DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:TopCam:markCrClassification",
    for(unsigned int x = 0; x < getImage().width(); x+=2) {
      for(unsigned int y = 0; y < getImage().height(); y+=2) {
        const Pixel& pixel = getImage().get(x, y);
        if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)getParameters().fieldColorMax.v) {
          POINT_PX(ColorClasses::red, x, y);
        }
      }
    }
  );

}//end execute

double SimpleFieldColorClassifier::gauss(double sigma, double mean, double x)
{
  double exponent = -((x - mean) * (x - mean)) / (2 * sigma * sigma);
  double ret = exp( exponent );
  return Math::isInf(ret) || Math::isNan(ret) ? 0.0 : ret;
}

