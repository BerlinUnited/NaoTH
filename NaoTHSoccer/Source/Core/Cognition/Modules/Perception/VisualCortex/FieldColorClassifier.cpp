/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 *
 * Created on 15. März 2011, 15:56
 */

#include "FieldColorClassifier.h"

#include <vector>

#include "Tools/Math/Vector3.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"


FieldColorClassifier::FieldColorClassifier()
:
  justStarted(true),  
  sampleCount(0),
  maxSampleCount(20)
{
  //DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:calibrate", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:set_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:reset", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:saveCalibratedParams", " ", false);

  GT_TRACE("before FieldColorClassifier constructor memset");
  memset(&weightedHistV, 0, sizeof(weightedHistV));
  GT_TRACE("after FieldColorClassifier constructor memset");
}

void FieldColorClassifier::execute()
{
  //DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:calibrate",
  //  calibrate();
  //  return;
  //);

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:saveCalibratedParams",
    fieldParams.MaxBrightnessChannelValue = getFieldColorPercept().maxY2Set;
    fieldParams.MaxCromaBlueChannelValue = getFieldColorPercept().maxU2Set;
    fieldParams.CromaRedChannelDistance = getFieldColorPercept().distV2Set;
    fieldParams.saveToConfig();
    fieldParams.syncWithConfig();
  );

  classify();
}

void FieldColorClassifier::classify()
{
  getFieldColorPercept().maxY = fieldParams.MaxBrightnessChannelValue;
  getFieldColorPercept().maxU = fieldParams.MaxCromaBlueChannelValue;
  getFieldColorPercept().distV = fieldParams.CromaRedChannelDistance;

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:reset",
    justStarted = true;
    sampleCount = 0;
  );

  PixelT<double> diff;

  bool valid = false;

  if(sampleCount >= maxSampleCount)
  {
    justStarted = false;
    diff.v = abs(getFieldColorPercept().indexV - maxWeightedV);
    valid = valid && diff.v < 1;
  }

  if(valid || justStarted)
  {
    sampleCount++;
    getFieldColorPercept().indexV = indexV;
  }


  if(!getHistogram().colorChannelIsUptodate)
  {
    return;
  }
 
  // init values
  maxWeightedV = 0.0;
  indexV = 0;

  STOPWATCH_START("FieldColorClassifier:Cr_filtering");
  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCr = max<int>(0,  128 - i);
    double wCr = mCr / 128.0;
    
    weightedHistV[i] = getHistogram().colorChannelHistogramField[i];
    weightedHistV[i] *= wCr;

    // remember the maximal value
    if(weightedHistV[i] > maxWeightedV)
    {
      maxWeightedV = weightedHistV[i];
      indexV = i;
    }
  }//end for
  STOPWATCH_STOP("FieldColorClassifier:Cr_filtering");

  getFieldColorPercept().set();
  getFieldColorPercept().lastUpdated = getFrameInfo();
  runDebugRequests();
}

void FieldColorClassifier::calibrate()
{
  double maxWeightedCr = 0.0;
  int indexCr = 0;

  unsigned int gridPointNum = getColoredGrid().uniformGrid.numberOfGridPoints;

  memset(&weightedHistV, 0, sizeof(weightedHistV));

  for(unsigned int i = gridPointNum / 2; i < gridPointNum; i++)
  {
    Pixel pixel;
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    getImage().getCorrected(point.x, point.y, pixel);

    colorChannelHistogram[pixel.v]++;
  }

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCr = max<int>(0,  128 - i);
    double wCr = mCr / 128.0;
    
    weightedHistV[i] = colorChannelHistogram[i];
    weightedHistV[i] *= wCr;

    // remember the maximal value
    if(weightedHistV[i] > maxWeightedV)
    {
      maxWeightedCr = weightedHistV[i];
      indexCr = i;
    }
  }//end for

  int meanRegionBeginIndexCr = 0;
  int meanRegionEndIndexCr = 0;

  // find the left Cr-limit
  // (move from maxWeightedIndexCr to 0)
  int idx = indexCr;
  while (idx >= 0 && meanRegionBeginIndexCr == 0)
  {
    if(weightedHistV[idx] <= 0.5)
    {
      meanRegionBeginIndexCr = idx;
    }
    idx--;
  }

  // find the right Cr-limit
  // (move from maxWeightedIndexCr to COLOR_CHANNEL_VALUE_COUNT)
  idx = indexCr;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCr == 0)
  {
    if(weightedHistV[idx] <= 0.5)
    {
      meanRegionEndIndexCr = idx;
    }
    idx++;
  }
  
  distCalib = distCalib / 2 + abs((int) meanRegionBeginIndexCr - (int) meanRegionEndIndexCr) / 4;
  getFieldColorPercept().distV = distCalib;

  runDebugRequests();
}

void FieldColorClassifier::runDebugRequests()
{
  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:set_in_image",
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
}


double FieldColorClassifier::smoothRungeKutta4(const int& idx, double* valueArray) const
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

