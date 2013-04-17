/*
 * File:   FieldColorClassifierFull.cpp
 * Author: claas
 *
 * Created on 15. März 2011, 15:56
 */

#include "FieldColorClassifierFull.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"

FieldColorClassifierFull::FieldColorClassifierFull()
:
  justStarted(true)
{
  //DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:show_Y_range", " ", false);
  //DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:show_Cb_range", " ", false);
  //DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:show_Cr_range", " ", false);
  //DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:show_weighted_Y_histogram", " ", false);
  //DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:show_weighted_Cb_histogram", " ", false);
  //DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:show_weighted_Cr_histogram", " ", false);
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:enable_plots", " ", false);
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:set_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:write_log", " ", false);  

  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:fast_adapt", " ", false);  

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    weightedSmoothedHistY[i] = 0.0;
    weightedSmoothedHistCb[i] = 0.0;
    weightedHistY[i] = 0.0;
    weightedHistCb[i] = 0.0;
    weightedHistCr[i] = 0.0;
  }
  lastFrame = getFrameInfo();   

  outfile.open ("fcc_full.log"); 
}

void FieldColorClassifierFull::execute()
{
  STOPWATCH_START("FieldColorClassifierFull:Cb_Cr_filtering");
  int halfChannelWidth = COLOR_CHANNEL_VALUE_COUNT / 2;

  double distY = getFieldColorPercept().distY;
  double distCb = getFieldColorPercept().distU;
  double distCr = getFieldColorPercept().distV;

  unsigned int maxDistY = fieldParams.maxDistY;
  unsigned int maxDistCb = fieldParams.maxDistCb;
  unsigned int maxDistCr = fieldParams.maxDistCr;

  unsigned int idx;

  double modifyDist = maxDistY;
  MODIFY("FieldColorClassifierFull:maxDistY", modifyDist);
  maxDistY = (unsigned int) modifyDist;

  modifyDist = maxDistCb;
  MODIFY("FieldColorClassifierFull:maxDistCb", modifyDist);
  maxDistCb = (unsigned int) modifyDist;

  modifyDist = maxDistCr;
  MODIFY("FieldColorClassifierFull:maxDistCr", modifyDist);
  maxDistCr = (unsigned int) modifyDist;

  double maxWeightedY = 0.85 * getFieldColorPercept().maxRateY;
  unsigned int maxWeightedIndexY = getFieldColorPercept().indexY;

//  double meanY = 0;
  double weightedMeanY = 0;

  double maxWeightedCb = 0.85 * getFieldColorPercept().maxRateU;
  unsigned int maxWeightedIndexCb = getFieldColorPercept().indexU;

  double maxWeightedCr = 0.85 * getFieldColorPercept().maxRateV;
  unsigned int maxWeightedIndexCr = getFieldColorPercept().indexV;

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    weightedHistY[i] = 0.0;
    weightedHistCb[i] = 0.0;

    double mCr = max(0.0, (double) halfChannelWidth - (double) i);
    double wCr = mCr / (double) halfChannelWidth;
    weightedHistCr[i] = wCr * (double) getHistogram().colorChannelHistogramCr[i];//colorChannelHistogramField[i];
    //search for max Cr channel value with weight w
    if(weightedHistCr[i] > maxWeightedCr)
    {
      maxWeightedCr = weightedHistCr[i];
      maxWeightedIndexCr = i;
    }
  }

  unsigned int meanRegionBeginIndexCr = 0;
  unsigned int meanRegionEndIndexCr = 0;

  idx = maxWeightedIndexCr;
  while (idx > 0 && meanRegionBeginIndexCr == 0)
  {
    if(weightedHistCr[idx] <= log(maxWeightedCr) / log(2.0) || maxWeightedIndexCr - idx > maxDistCr)
    {
      meanRegionBeginIndexCr = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexCr;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCr == 0)
  {
    if(weightedHistCr[idx] <= log(maxWeightedCr) / log(2.0) || idx - maxWeightedIndexCr > maxDistCr)
    {
      meanRegionEndIndexCr = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexCr != 0 && meanRegionBeginIndexCr != 0)
  {
    maxWeightedIndexCr = (meanRegionBeginIndexCr + meanRegionEndIndexCr + getFieldColorPercept().indexV) / 3;
    distCr = (meanRegionEndIndexCr - meanRegionBeginIndexCr + maxDistCr + getFieldColorPercept().distV) / 4;
  }

  STOPWATCH_STOP("FieldColorClassifierFull:Cb_Cr_filtering");


  STOPWATCH_START("FieldColorClassifierFull:GridWalk");
  double meanFieldY = 0.0;
  double meanFieldCountY = 1.0;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    const Pixel& pixel = getImage().get(point.x,point.y);

    if
    (
      abs((int)(pixel.v  - distCr ) < (int) maxWeightedIndexCr)
    )
    {
      weightedHistY[pixel.y]++;
      meanFieldY += pixel.y;
      meanFieldCountY++;

      weightedHistCb[pixel.u]++;
    }
  }
  STOPWATCH_STOP("FieldColorClassifierFull:GridWalk");

  STOPWATCH_START("FieldColorClassifierFull:Y_filtering");
  meanFieldY /= meanFieldCountY;
  double factorDistY =  ((double) halfChannelWidth - fabs((double) halfChannelWidth - meanFieldY) ) / (double) halfChannelWidth;

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCb = COLOR_CHANNEL_VALUE_COUNT - i;
    double wCb = mCb / (double) COLOR_CHANNEL_VALUE_COUNT;
    weightedHistCb[i] *= wCb;//= wCb * (double) getHistogram().colorChannelHistogram[1][i];
    double smoothWeightedCb = smoothRungeKutta4(i, weightedHistCb);
    weightedSmoothedHistCb[i] = smoothWeightedCb;
    if(smoothWeightedCb > maxWeightedCb)
    {
      maxWeightedCb = smoothWeightedCb;
      maxWeightedIndexCb = i;
    }

    if(meanFieldY != 0.0)
    {
      double mY = max(0.0,meanFieldY - fabs(meanFieldY - (double) i));
      double wY = mY / meanFieldY ;
      weightedHistY[i] *= wY;
      double smoothWeightedY = smoothRungeKutta4(i, weightedHistY);
      weightedSmoothedHistY[i] = smoothWeightedY;
      //search for max Y channel value with weight w
      if(weightedSmoothedHistY[i] > maxWeightedY)
      {
        maxWeightedY = weightedSmoothedHistY[i];
        maxWeightedIndexY = i;
      }
    }
  }

  unsigned int meanRegionBeginIndexY = 0;
  unsigned int meanRegionEndIndexY = 0;

  idx = maxWeightedIndexY;
  while (idx > 0 && meanRegionBeginIndexY == 0)
  {
    if(weightedSmoothedHistY[idx] <= log(maxWeightedY) / log(2.0) || maxWeightedIndexY - idx >  factorDistY * maxDistY)
    {
      meanRegionBeginIndexY = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexY;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexY == 0)
  {
    if(weightedSmoothedHistY[idx] <= log(maxWeightedY) / log(2.0) || idx - maxWeightedIndexY > factorDistY * maxDistY)
    {
      meanRegionEndIndexY = idx;
    }
    idx++;
  }

  unsigned int meanRegionBeginIndexCb = 0;
  unsigned int meanRegionEndIndexCb = 0;

  idx = maxWeightedIndexCb;
  while (idx > 0 && meanRegionBeginIndexCb == 0)
  {
    if(weightedHistCb[idx] <= log(maxWeightedCb) / log(2.0) || maxWeightedIndexCb - idx > maxDistCb)
    {
      meanRegionBeginIndexCb = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexCb;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCb == 0)
  {
    if(weightedHistCb[idx] <= log(maxWeightedCb) / log(2.0) || idx - maxWeightedIndexCb > maxDistCb)
    {
      meanRegionEndIndexCb = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexCb != 0 && meanRegionBeginIndexCb != 0)
  {
    maxWeightedIndexCb = (meanRegionBeginIndexCb + meanRegionEndIndexCb + getFieldColorPercept().indexU) / 3;
    distCb = (meanRegionEndIndexCb - meanRegionBeginIndexCb + maxDistCb + getFieldColorPercept().distU) / 4;
  }

  double fY = (double) meanFieldY / 128;

  distY = (unsigned int) (distY * fY) + fieldParams.minDistY;
  double fCb = log((double) maxWeightedCb) / log(200.0);
  distCb = (unsigned int) (distCb * fCb) + fieldParams.minDistCb;
  double fCr = log((double) maxWeightedCr) / log(200.0);
  distCr = (unsigned int) (distCr * fCr) + fieldParams.minDistCr;

  STOPWATCH_STOP("FieldColorClassifierFull:Y_filtering");

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    weightedHistY[i] = weightedSmoothedHistY[i];
    weightedHistCb[i] = weightedSmoothedHistCb[i];
  }

  if(meanRegionEndIndexY != 0 && meanRegionBeginIndexY != 0)
  {
    maxWeightedIndexY = (meanRegionBeginIndexY + meanRegionEndIndexY + getFieldColorPercept().indexY) / 3;
    distY =(meanRegionEndIndexY - meanRegionBeginIndexY + maxDistY + getFieldColorPercept().distY) / 4;
  }

  unsigned int minCrRate = fieldParams.minCrRate;
  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:fast_adapt",
    minCrRate = 0;
  );

  if(maxWeightedCr >= minCrRate)
  {
    if(distY > fieldParams.maxDistY)
    {
      distY = fieldParams.maxDistY;
    }
    if(distY > 0)
    {
      getFieldColorPercept().distY = distY;
    }

    if(distCb > fieldParams.maxDistCb)
    {
      distCb = fieldParams.maxDistCb;
    }
    if(distCb > 0)
    {
      getFieldColorPercept().distU = distCb;
    }

    if(distCr > fieldParams.maxDistCr)
    {
      distCr = fieldParams.maxDistCr;
    }
    if(distCr > 0)
    {
      getFieldColorPercept().distV = distCr;
    }

    if(maxWeightedY > 0)
    {
      getFieldColorPercept().maxRateY = maxWeightedY;
      getFieldColorPercept().indexY = maxWeightedIndexY;
    }

    bool cbChanged = IndexCbBuffer.getNumberOfEntries() < IndexCbBuffer.getMaxEntries() 
          || abs(IndexCbBuffer.getAverage() - (int) maxWeightedIndexCb) < getBaseColorRegionPercept().meanEnv.y / 10
          || lastFrame.getFrameNumber() + 30 <= getFrameInfo().getFrameNumber();

    DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:fast_adapt",
      cbChanged = true;
    );

    if(cbChanged)
    {
      if(maxWeightedCb > 0)
      {
        IndexCbBuffer.add((int) maxWeightedIndexCb);
        getFieldColorPercept().indexU = IndexCbBuffer.getAverage();
      }
      else
      {
        getFieldColorPercept().indexU = maxWeightedIndexCb;
      }
      getFieldColorPercept().maxRateU = maxWeightedCb;
    }

    bool crChanged = IndexCrBuffer.getNumberOfEntries() < IndexCrBuffer.getMaxEntries() 
          || abs(IndexCrBuffer.getAverage() - (int) maxWeightedIndexCr) < getBaseColorRegionPercept().meanEnv.y / 20
          || lastFrame.getFrameNumber() + 30 <= getFrameInfo().getFrameNumber();

    DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:fast_adapt",
      crChanged = true;
    );

    if(crChanged)
    {
      lastFrame = getFrameInfo();
      if(maxWeightedCb > 0)
      {
        IndexCrBuffer.add((int) maxWeightedIndexCr);
        getFieldColorPercept().indexV = IndexCrBuffer.getAverage();
      }
      else
      {
        getFieldColorPercept().indexV = maxWeightedIndexCr;
      }
      getFieldColorPercept().maxRateV = maxWeightedCr;
    }

  }
  getFieldColorPercept().range.set(
    0,//getFieldColorPercept().indexY - (int) getFieldColorPercept().distY,
    getFieldColorPercept().indexU - (int) getFieldColorPercept().distU,
    getFieldColorPercept().indexV - (int) getFieldColorPercept().distV,

    255,//getFieldColorPercept().indexY + (int) getFieldColorPercept().distY,
    getFieldColorPercept().indexU + (int) getFieldColorPercept().distU,
    getFieldColorPercept().indexV + (int) getFieldColorPercept().distV
  ); 

  getFieldColorPercept().lastUpdated = getFrameInfo();

  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:write_log",
    outfile << getFrameInfo().getFrameNumber() << " " << getBaseColorRegionPercept().meanImg.y << " " << getBaseColorRegionPercept().meanImg.u << " " << getBaseColorRegionPercept().meanImg.v;
    outfile << " " << getBaseColorRegionPercept().meanEnv.y << " " << getBaseColorRegionPercept().meanEnv.u << " " << getBaseColorRegionPercept().meanEnv.v;
    outfile << " " << getFieldColorPercept().indexY << " " <<  getFieldColorPercept().indexU << " " <<  getFieldColorPercept().indexV;
    outfile << " " << getFieldColorPercept().distY << " " <<  getFieldColorPercept().distU << " " <<  getFieldColorPercept().distV;
    outfile << " " << getFieldColorPercept().maxRateY << " " <<  getFieldColorPercept().maxRateU << " " <<  getFieldColorPercept().maxRateV;

    outfile << endl;
  );

  runDebugRequests((int) weightedMeanY, (int) meanFieldY);
}

double FieldColorClassifierFull::smoothRungeKutta4(const unsigned int& idx, double* valueArray)
{
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

void FieldColorClassifierFull::runDebugRequests(int weightedMeanY, int meanY)
{
  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:enable_plots",
    for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i ++)
    {
      PLOT_GENERIC("FieldColorClassifierFull:weighted_Y_histogram", i, weightedHistY[i]);
      PLOT_GENERIC("FieldColorClassifierFull:weighted_Cb_histogram", i, weightedHistCb[i]);
      PLOT_GENERIC("FieldColorClassifierFull:weighted_Cr_histogram", i, weightedHistCr[i]);
    }
  );

  int imageWidth = getImage().cameraInfo.resolutionWidth;
  int imageHeight = getImage().cameraInfo.resolutionHeight;

  //DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:show_Y_range",
  //  LINE_PX
  //  (
  //    ColorClasses::white,
  //    Math::clamp((int) (getFieldColorPercept().indexY - getFieldColorPercept().distY), 0, imageWidth),
  //    0,
  //    Math::clamp((int) (getFieldColorPercept().indexY - getFieldColorPercept().distY), 0, imageWidth),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::gray,
  //    Math::clamp((int) getFieldColorPercept().indexY, 0, imageWidth),
  //    imageHeight,
  //    Math::clamp((int) getFieldColorPercept().indexY, 0, imageWidth),
  //    Math::clamp(imageHeight - (int) getFieldColorPercept().maxRateY, 0, imageHeight)
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::white,
  //    Math::clamp((int) (getFieldColorPercept().indexY + getFieldColorPercept().distY), 0, imageWidth),
  //    0,
  //    Math::clamp((int) (getFieldColorPercept().indexY + getFieldColorPercept().distY), 0, imageWidth),
  //    imageHeight
  //  );
  //);

  //DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:show_Cb_range",
  //  LINE_PX
  //  (
  //    ColorClasses::blue,
  //    Math::clamp((int) (getFieldColorPercept().indexU - getFieldColorPercept().distU), 0, imageWidth),
  //    0,
  //    Math::clamp((int) (getFieldColorPercept().indexU - getFieldColorPercept().distU), 0, imageWidth),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::skyblue,
  //    Math::clamp((int) getFieldColorPercept().indexU, 0, imageWidth),
  //    imageHeight,
  //    Math::clamp((int) getFieldColorPercept().indexU, 0, imageWidth),
  //    Math::clamp(imageHeight - (int) getFieldColorPercept().maxRateU, 0, imageHeight)
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::blue,
  //    Math::clamp((int) (getFieldColorPercept().indexU + getFieldColorPercept().distU), 0, imageWidth),
  //    0,
  //    Math::clamp((int) (getFieldColorPercept().indexU + getFieldColorPercept().distU), 0, imageWidth),
  //    imageHeight
  //  );
  //);

  //DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:show_Cr_range",
  //  LINE_PX
  //  (
  //    ColorClasses::red,
  //    Math::clamp((int) (getFieldColorPercept().indexV - getFieldColorPercept().distV), 0, imageWidth),
  //    0,
  //    Math::clamp((int) (getFieldColorPercept().indexV - getFieldColorPercept().distV), 0, imageWidth),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::orange,
  //    Math::clamp((int) getFieldColorPercept().indexV, 0, imageWidth),
  //    imageHeight,
  //    Math::clamp((int) getFieldColorPercept().indexV, 0, imageWidth),
  //    Math::clamp(imageHeight - (int) getFieldColorPercept().maxRateV, 0, imageHeight)
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::red,
  //    Math::clamp((int) (getFieldColorPercept().indexV + getFieldColorPercept().distV), 0, imageWidth),
  //    0,
  //    Math::clamp((int) (getFieldColorPercept().indexV + getFieldColorPercept().distV), 0, imageWidth),
  //    imageHeight
  //  );
  //);

  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:set_in_image",

    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);
        if
        (
          getFieldColorPercept().isFieldColor(pixel)
        )
        {
          POINT_PX(ColorClasses::green, x, y);
        }
      }
    }
  );
}


