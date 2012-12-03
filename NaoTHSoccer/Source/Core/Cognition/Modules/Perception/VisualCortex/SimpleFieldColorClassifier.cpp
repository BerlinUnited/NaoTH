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

SimpleFieldColorClassifier::SimpleFieldColorClassifier()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:weightedHistCr", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:weightedHistY", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:weightedHistCb", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:markCrClassification", "", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:SimpleFieldColorClassifier:mark_green", "", false);
}

void SimpleFieldColorClassifier::execute()
{

  // check if a fresh histogram is avaliable
  if(!getHistogram().colorChannelIsUptodate)
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

  // the histogram is weighted with the function 
  // max(0,128-i)/128, i.e., we are interested only in the first half of it
  for(int i = 0; i < halfChannelWidth; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCr = 1.0 - i*histogramDoubleStep;
    double weightedCr = wCr * (double) getHistogram().colorChannelHistogramCr[i];

    // search for max Cr channel value with weight w
    if(weightedCr > maxWeightedCr)
    {
      maxWeightedCr = weightedCr;
      maxWeightedIndexCr = i;
    }
  }//end for
  

  // no green candidates found
  if(maxWeightedIndexCr < 0)
  {
    return;
  }


  DEBUG_REQUEST("ImageProcessor:SimpleFieldColorClassifier:markCrClassification",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if( abs((int)pixel.v-(int)maxWeightedIndexCr) < (int)fieldParams.fieldColorMax.v)
        {
          POINT_PX(ColorClasses::red, x, y);
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
    double weightedCb = wCb * (double) getHistogram().colorChannelHistogramCb[i];

    // calculate the Cb maximum
    if(weightedCb > maxWeightedCb)
    {
      maxWeightedCb = weightedCb;
      maxWeightedIndexCb = i;
    }

    meanFieldY += getHistogram().colorChannelHistogramY[i]*i;
    numberOfFieldY += getHistogram().colorChannelHistogramY[i];
  }//end for


  if(numberOfFieldY > 0)
  {
    meanFieldY /= numberOfFieldY;
  }

  //check how it works in other conditions
  int maxWeightedIndexY = (int)Math::clamp(meanFieldY,0.0, 255.0);


  PLOT("SimpleFieldColorClassifier:maxWeightedIndexCr", maxWeightedIndexCr);
  PLOT("SimpleFieldColorClassifier:maxWeightedIndexCb", maxWeightedIndexCb);
  PLOT("SimpleFieldColorClassifier:meanFieldY", meanFieldY);

  double maxY = 100;
  MODIFY("SimpleFieldColorClassifier:maxY", maxY);
  double minY = 5;
  MODIFY("SimpleFieldColorClassifier:minY", minY);

  DEBUG_REQUEST("ImageProcessor:SimpleFieldColorClassifier:mark_green",
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
  getFieldColorPercept().set();
  getFieldColorPercept().lastUpdated = getFrameInfo();
}//end execute