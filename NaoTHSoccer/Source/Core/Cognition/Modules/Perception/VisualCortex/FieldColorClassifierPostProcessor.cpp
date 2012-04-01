/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 *
 * Created on 15. März 2011, 15:56
 */

#include "FieldColorClassifierPostProcessor.h"

#include <vector>

#include "Tools/Math/Vector3.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"


FieldColorClassifierPostProcessor::FieldColorClassifierPostProcessor()
:
  justStarted(true),  
  sampleCount(0),
  maxSampleCount(20)
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:set_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:reset", " ", false);

  GT_TRACE("before FieldColorClassifierPostProcessor constructor memset");
  memset(&weightedHistU, 0, sizeof(weightedHistU));
  GT_TRACE("after FieldColorClassifierPostProcessor constructor memset");
}

void FieldColorClassifierPostProcessor::execute()
{

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:reset",
    justStarted = true;
    sampleCount = 0;
  );

  PixelT<double> diff;

  bool valid = false;

  if(sampleCount >= maxSampleCount)
  {
    justStarted = false;
    diff.v = abs(getFieldColorPercept().indexU - maxWeightedU);
    valid = valid && diff.v < 1;
  }

  if(valid || justStarted)
  {
    sampleCount++;
    getFieldColorPercept().indexU = indexU;
    getFieldColorPercept().distU = fieldParams.CromaRedChannelDistance;
  }

  getFieldColorPercept().lastUpdated = getFrameInfo();

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:set_in_image",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if(getFieldColorPercept().isFieldColor(pixel))
        {
          POINT_PX(ColorClasses::red, x, y);
        }
      }
    }
  );

  if(!getHistogram().colorChannelIsUptodate)
  {
    return;
  }
 
  // init values
  maxWeightedU = 0.0;
  indexU = 0;

  STOPWATCH_START("FieldColorClassifier:Cr_filtering");
  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCr = max<int>(0,  128 - i);
    double wCr = mCr / 128.0;
    
    weightedHistU[i] = getHistogram().colorChannelHistogramField[i];
    weightedHistU[i] *= wCr;

    // remember the maximal value
    if(weightedHistU[i] > maxWeightedU)
    {
      maxWeightedU = weightedHistU[i];
      indexU = i;
    }
  }//end for
  STOPWATCH_STOP("FieldColorClassifier:Cr_filtering");

  getFieldColorPercept().lastUpdated = getFrameInfo();

}

double FieldColorClassifierPostProcessor::smoothRungeKutta4(const int& idx, double* valueArray) const
{
  if(idx < 0) return 0.0;

  switch(idx)
  {
    case 0:
      return valueArray[0];

    case 1:
      return valueArray[1];

    case 2:
      return (valueArray[0] + valueArray[1]) / 2;

    case 3:
      return(valueArray[0] +  4 * valueArray[1] + valueArray[2]) / 6;
  }

  return (valueArray[idx - 1] +  2 * (valueArray[idx - 2] + valueArray[idx - 3]) + valueArray[idx - 4]) / 6;
}

