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
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:TopCam:enable_plots", " ", false);
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:BottomCam:enable_plots", " ", false);
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:TopCam:set_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:BottomCam:set_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:write_log", " ", false);  
  DEBUG_REQUEST_REGISTER("NeoVision:FieldColorClassifierFull:fast_adapt", " ", false);  

  for(unsigned int i = 0; i < ColorChanelHistograms::VALUE_COUNT; i++)
  {
    memset(&weightedSmoothedHistY, 0, sizeof(weightedSmoothedHistY));
    memset(&weightedSmoothedHistCb, 0, sizeof(weightedSmoothedHistCb));

    memset(&weightedHistY, 0, sizeof(weightedHistY));
    memset(&weightedHistCb, 0, sizeof(weightedHistCb));
    memset(&weightedHistCr, 0, sizeof(weightedHistCr));

    memset(&weightedSmoothedHistTopY, 0, sizeof(weightedSmoothedHistTopY));
    memset(&weightedSmoothedHistTopCb, 0, sizeof(weightedSmoothedHistTopCb));

    memset(&weightedHistTopY, 0, sizeof(weightedHistTopY));
    memset(&weightedHistTopCb, 0, sizeof(weightedHistTopCb));
    memset(&weightedHistTopCr, 0, sizeof(weightedHistTopCr));
  }
  lastFrame = getFrameInfo();   

  outfile.open ("fcc_full.log"); 
}

void FieldColorClassifierFull::execute()
{
  STOPWATCH_START("FieldColorClassifierFull:Cb_Cr_filtering");
  int halfChannelWidth = ColorChanelHistograms::VALUE_COUNT / 2;

  double distY = getFieldColorPercept().distY;
  double distCb = getFieldColorPercept().distU;
  double distCr = getFieldColorPercept().distV;
  double distTopY = getFieldColorPerceptTop().distY;
  double distTopCb = getFieldColorPerceptTop().distU;
  double distTopCr = getFieldColorPerceptTop().distV;

  unsigned int maxDistY = fieldParams.maxDistY;
  unsigned int maxDistCb = fieldParams.maxDistCb;
  unsigned int maxDistCr = fieldParams.maxDistCr;

  unsigned int idx;
  unsigned int idxTop;

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
  double maxWeightedTopY = 0.85 * getFieldColorPerceptTop().maxRateY;
  unsigned int maxWeightedIndexTopY = getFieldColorPerceptTop().indexY;

  double maxWeightedCb = 0.85 * getFieldColorPercept().maxRateU;
  unsigned int maxWeightedIndexCb = getFieldColorPercept().indexU;
  double maxWeightedTopCb = 0.85 * getFieldColorPerceptTop().maxRateU;
  unsigned int maxWeightedIndexTopCb = getFieldColorPerceptTop().indexU;

  double maxWeightedCr = 0.85 * getFieldColorPercept().maxRateV;
  unsigned int maxWeightedIndexCr = getFieldColorPercept().indexV;
  double maxWeightedTopCr = 0.85 * getFieldColorPerceptTop().maxRateV;
  unsigned int maxWeightedIndexTopCr = getFieldColorPerceptTop().indexV;

  for(unsigned int i = 0; i < ColorChanelHistograms::VALUE_COUNT; i++)
  {
    weightedHistY[i] = 0.0;
    weightedHistCb[i] = 0.0;
    weightedHistTopY[i] = 0.0;
    weightedHistTopCb[i] = 0.0;

    double mCr = max(0.0, (double) halfChannelWidth - (double) i);
    double wCr = mCr / (double) halfChannelWidth;
    weightedHistCr[i] = wCr * (double) getColorChanelHistograms().histogramV.rawData[i];
    weightedHistTopCr[i] = wCr * (double) getColorChanelHistogramsTop().histogramV.rawData[i];
    //search for max Cr channel value with weight w
    if(weightedHistCr[i] > maxWeightedCr)
    {
      maxWeightedCr = weightedHistCr[i];
      maxWeightedIndexCr = i;
    }
    if(weightedHistTopCr[i] > maxWeightedTopCr)
    {
      maxWeightedTopCr = weightedHistTopCr[i];
      maxWeightedIndexTopCr = i;
    }
  }

  unsigned int meanRegionBeginIndexCr = 0;
  unsigned int meanRegionEndIndexCr = 0;
  unsigned int meanRegionBeginIndexTopCr = 0;
  unsigned int meanRegionEndIndexTopCr = 0;

  idx = maxWeightedIndexCr;
  while (idx > 0 && meanRegionBeginIndexCr == 0)
  {
    if(weightedHistCr[idx] <= log(maxWeightedCr) / log(2.0) || maxWeightedIndexCr - idx > maxDistCr)
    {
      meanRegionBeginIndexCr = idx;
    }
    idx--;
  }
  idxTop = maxWeightedIndexTopCr;
  while (idxTop > 0 && meanRegionBeginIndexTopCr == 0)
  {
    if(weightedHistTopCr[idxTop] <= log(maxWeightedTopCr) / log(2.0) || maxWeightedIndexTopCr - idxTop > maxDistCr)
    {
      meanRegionBeginIndexTopCr = idxTop;
    }
    idxTop--;
  }

  idx = maxWeightedIndexCr;
  while (idx < ColorChanelHistograms::VALUE_COUNT && meanRegionEndIndexCr == 0)
  {
    if(weightedHistCr[idx] <= log(maxWeightedCr) / log(2.0) || idx - maxWeightedIndexCr > maxDistCr)
    {
      meanRegionEndIndexCr = idx;
    }
    idx++;
  }
  idxTop = maxWeightedIndexTopCr;
  while (idxTop < ColorChanelHistograms::VALUE_COUNT && meanRegionEndIndexTopCr == 0)
  {
    if(weightedHistTopCr[idxTop] <= log(maxWeightedTopCr) / log(2.0) || idxTop - maxWeightedIndexTopCr > maxDistCr)
    {
      meanRegionEndIndexTopCr = idxTop;
    }
    idxTop++;
  }

  if(meanRegionEndIndexCr != 0 && meanRegionBeginIndexCr != 0)
  {
    maxWeightedIndexCr = (meanRegionBeginIndexCr + meanRegionEndIndexCr + getFieldColorPercept().indexV) / 3;
    distCr = (meanRegionEndIndexCr - meanRegionBeginIndexCr + maxDistCr + getFieldColorPercept().distV) / 4;
  }
  if(meanRegionEndIndexTopCr != 0 && meanRegionBeginIndexTopCr != 0)
  {
    maxWeightedIndexTopCr = (meanRegionBeginIndexTopCr + meanRegionEndIndexTopCr + getFieldColorPerceptTop().indexV) / 3;
    distTopCr = (meanRegionEndIndexTopCr - meanRegionBeginIndexTopCr + maxDistCr + getFieldColorPerceptTop().distV) / 4;
  }

  STOPWATCH_STOP("FieldColorClassifierFull:Cb_Cr_filtering");


  STOPWATCH_START("FieldColorClassifierFull:GridWalk");
  double meanFieldY = 0.0;
  double meanFieldCountY = 1.0;
  double meanFieldTopY = 0.0;
  double meanFieldCountTopY = 1.0;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    const Pixel& pixel = getImage().get(point.x,point.y);
    const Vector2<int>& pointTop = getColoredGridTop().uniformGrid.getPoint(i);
    const Pixel& pixelTop = getImageTop().get(pointTop.x,pointTop.y);

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
    if
    (
      abs((int)(pixelTop.v  - distTopCr ) < (int) maxWeightedIndexTopCr)
    )
    {
      weightedHistTopY[pixelTop.y]++;
      meanFieldTopY += pixelTop.y;
      meanFieldCountTopY++;

      weightedHistTopCb[pixelTop.u]++;
    }
  }
  STOPWATCH_STOP("FieldColorClassifierFull:GridWalk");

  STOPWATCH_START("FieldColorClassifierFull:Y_filtering");
  meanFieldY /= meanFieldCountY;
  double factorDistY =  ((double) halfChannelWidth - fabs((double) halfChannelWidth - meanFieldY) ) / (double) halfChannelWidth;
  meanFieldTopY /= meanFieldCountTopY;
  double factorDistTopY =  ((double) halfChannelWidth - fabs((double) halfChannelWidth - meanFieldTopY) ) / (double) halfChannelWidth;

  for(unsigned int i = 0; i < ColorChanelHistograms::VALUE_COUNT; i++)
  {
    double mCb = ColorChanelHistograms::VALUE_COUNT - i;
    double wCb = mCb / (double) ColorChanelHistograms::VALUE_COUNT;
    weightedHistCb[i] *= wCb;//= wCb * (double) getHistograms().histogramU.rawData[i];
    weightedHistTopCb[i] *= wCb;//= wCb * (double) getHistograms().histogramU.rawData[i];
    double smoothWeightedCb = smoothRungeKutta4(i, weightedHistCb);
    weightedSmoothedHistCb[i] = smoothWeightedCb;
    double smoothWeightedTopCb = smoothRungeKutta4(i, weightedHistTopCb);
    weightedSmoothedHistTopCb[i] = smoothWeightedTopCb;
    if(smoothWeightedCb > maxWeightedCb)
    {
      maxWeightedCb = smoothWeightedCb;
      maxWeightedIndexCb = i;
    }
    if(smoothWeightedTopCb > maxWeightedTopCb)
    {
      maxWeightedTopCb = smoothWeightedTopCb;
      maxWeightedIndexTopCb = i;
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
    if(meanFieldTopY != 0.0)
    {
      double mY = max(0.0,meanFieldTopY - fabs(meanFieldTopY - (double) i));
      double wY = mY / meanFieldTopY ;
      weightedHistTopY[i] *= wY;
      double smoothWeightedTopY = smoothRungeKutta4(i, weightedHistTopY);
      weightedSmoothedHistTopY[i] = smoothWeightedTopY;
      //search for max Y channel value with weight w
      if(weightedSmoothedHistTopY[i] > maxWeightedTopY)
      {
        maxWeightedTopY = weightedSmoothedHistTopY[i];
        maxWeightedIndexTopY = i;
      }
    }
  }

  unsigned int meanRegionBeginIndexY = 0;
  unsigned int meanRegionEndIndexY = 0;
  unsigned int meanRegionBeginIndexTopY = 0;
  unsigned int meanRegionEndIndexTopY = 0;

  idx = maxWeightedIndexY;
  while (idx > 0 && meanRegionBeginIndexY == 0)
  {
    if(weightedSmoothedHistY[idx] <= log(maxWeightedY) / log(2.0) || maxWeightedIndexY - idx >  factorDistY * maxDistY)
    {
      meanRegionBeginIndexY = idx;
    }
    idx--;
  }
  idxTop = maxWeightedIndexTopY;
  while (idxTop > 0 && meanRegionBeginIndexTopY == 0)
  {
    if(weightedSmoothedHistTopY[idxTop] <= log(maxWeightedTopY) / log(2.0) || maxWeightedIndexTopY - idxTop >  factorDistTopY * maxDistY)
    {
      meanRegionBeginIndexTopY = idxTop;
    }
    idxTop--;
  }

  idx = maxWeightedIndexY;
  while (idx < ColorChanelHistograms::VALUE_COUNT && meanRegionEndIndexY == 0)
  {
    if(weightedSmoothedHistY[idx] <= log(maxWeightedY) / log(2.0) || idx - maxWeightedIndexY > factorDistY * maxDistY)
    {
      meanRegionEndIndexY = idx;
    }
    idx++;
  }
  idxTop = maxWeightedIndexTopY;
  while (idx < ColorChanelHistograms::VALUE_COUNT && meanRegionEndIndexTopY == 0)
  {
    if(weightedSmoothedHistTopY[idxTop] <= log(maxWeightedTopY) / log(2.0) || idxTop - maxWeightedIndexTopY > factorDistTopY * maxDistY)
    {
      meanRegionEndIndexTopY = idxTop;
    }
    idxTop++;
  }

  unsigned int meanRegionBeginIndexCb = 0;
  unsigned int meanRegionEndIndexCb = 0;
  unsigned int meanRegionBeginIndexTopCb = 0;
  unsigned int meanRegionEndIndexTopCb = 0;

  idx = maxWeightedIndexCb;
  while (idx > 0 && meanRegionBeginIndexCb == 0)
  {
    if(weightedHistCb[idx] <= log(maxWeightedCb) / log(2.0) || maxWeightedIndexCb - idx > maxDistCb)
    {
      meanRegionBeginIndexCb = idx;
    }
    idx--;
  }
  idxTop = maxWeightedIndexTopCb;
  while (idxTop > 0 && meanRegionBeginIndexTopCb == 0)
  {
    if(weightedHistTopCb[idxTop] <= log(maxWeightedTopCb) / log(2.0) || maxWeightedIndexTopCb - idxTop > maxDistCb)
    {
      meanRegionBeginIndexTopCb = idxTop;
    }
    idxTop--;
  }

  idx = maxWeightedIndexCb;
  while (idx < ColorChanelHistograms::VALUE_COUNT && meanRegionEndIndexCb == 0)
  {
    if(weightedHistCb[idx] <= log(maxWeightedCb) / log(2.0) || idx - maxWeightedIndexCb > maxDistCb)
    {
      meanRegionEndIndexCb = idx;
    }
    idx++;
  }
  idxTop = maxWeightedIndexTopCb;
  while (idxTop < ColorChanelHistograms::VALUE_COUNT && meanRegionEndIndexTopCb == 0)
  {
    if(weightedHistTopCb[idx] <= log(maxWeightedTopCb) / log(2.0) || idxTop - maxWeightedIndexTopCb > maxDistCb)
    {
      meanRegionEndIndexTopCb = idxTop;
    }
    idxTop++;
  }

  if(meanRegionEndIndexCb != 0 && meanRegionBeginIndexCb != 0)
  {
    maxWeightedIndexCb = (meanRegionBeginIndexCb + meanRegionEndIndexCb + getFieldColorPercept().indexU) / 3;
    distCb = (meanRegionEndIndexCb - meanRegionBeginIndexCb + maxDistCb + getFieldColorPercept().distU) / 4;
  }
  if(meanRegionEndIndexTopCb != 0 && meanRegionBeginIndexTopCb != 0)
  {
    maxWeightedIndexTopCb = (meanRegionBeginIndexTopCb + meanRegionEndIndexTopCb + getFieldColorPerceptTop().indexU) / 3;
    distTopCb = (meanRegionEndIndexTopCb - meanRegionBeginIndexTopCb + maxDistCb + getFieldColorPerceptTop().distU) / 4;
  }

  double fY = (double) meanFieldY / 128;
  distY = (unsigned int) (distY * fY) + fieldParams.minDistY;
  double fCb = log((double) maxWeightedCb) / log(200.0);
  distCb = (unsigned int) (distCb * fCb) + fieldParams.minDistCb;
  double fCr = log((double) maxWeightedCr) / log(200.0);
  distCr = (unsigned int) (distCr * fCr) + fieldParams.minDistCr;

  double fTopY = (double) meanFieldTopY / 128;
  distTopY = (unsigned int) (distTopY * fTopY) + fieldParams.minDistY;
  double fTopCb = log((double) maxWeightedTopCb) / log(200.0);
  distTopCb = (unsigned int) (distTopCb * fTopCb) + fieldParams.minDistCb;
  double fTopCr = log((double) maxWeightedTopCr) / log(200.0);
  distTopCr = (unsigned int) (distTopCr * fTopCr) + fieldParams.minDistCr;

  STOPWATCH_STOP("FieldColorClassifierFull:Y_filtering");

  for(unsigned int i = 0; i < ColorChanelHistograms::VALUE_COUNT; i++)
  {
    weightedHistY[i] = weightedSmoothedHistY[i];
    weightedHistCb[i] = weightedSmoothedHistCb[i];
    weightedHistTopY[i] = weightedSmoothedHistTopY[i];
    weightedHistTopCb[i] = weightedSmoothedHistTopCb[i];
  }

  if(meanRegionEndIndexY != 0 && meanRegionBeginIndexY != 0)
  {
    maxWeightedIndexY = (meanRegionBeginIndexY + meanRegionEndIndexY + getFieldColorPercept().indexY) / 3;
    distY =(meanRegionEndIndexY - meanRegionBeginIndexY + maxDistY + getFieldColorPercept().distY) / 4;
  }
  if(meanRegionEndIndexTopY != 0 && meanRegionBeginIndexTopY != 0)
  {
    maxWeightedIndexTopY = (meanRegionBeginIndexTopY + meanRegionEndIndexTopY + getFieldColorPerceptTop().indexY) / 3;
    distTopY =(meanRegionEndIndexTopY - meanRegionBeginIndexTopY + maxDistY + getFieldColorPerceptTop().distY) / 4;
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

    bool cbChanged = IndexCbBuffer.size() < IndexCbBuffer.getMaxEntries() 
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

    bool crChanged = IndexCrBuffer.size() < IndexCrBuffer.getMaxEntries() 
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

  if(maxWeightedTopCr >= minCrRate)
  {
    if(distTopY > fieldParams.maxDistY)
    {
      distTopY = fieldParams.maxDistY;
    }
    if(distTopY > 0)
    {
      getFieldColorPerceptTop().distY = distTopY;
    }

    if(distTopCb > fieldParams.maxDistCb)
    {
      distTopCb = fieldParams.maxDistCb;
    }
    if(distTopCb > 0)
    {
      getFieldColorPerceptTop().distU = distTopCb;
    }

    if(distTopCr > fieldParams.maxDistCr)
    {
      distTopCr = fieldParams.maxDistCr;
    }
    if(distTopCr > 0)
    {
      getFieldColorPerceptTop().distV = distTopCr;
    }

    if(maxWeightedTopY > 0)
    {
      getFieldColorPerceptTop().maxRateY = maxWeightedTopY;
      getFieldColorPerceptTop().indexY = maxWeightedIndexTopY;
    }

    bool cbChanged = IndexCbBufferTop.size() < IndexCbBufferTop.getMaxEntries() 
          || abs(IndexCbBufferTop.getAverage() - (int) maxWeightedIndexTopCb) < getBaseColorRegionPerceptTop().meanEnv.y / 10
          || lastFrame.getFrameNumber() + 30 <= getFrameInfo().getFrameNumber();

    DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:fast_adapt",
      cbChanged = true;
    );

    if(cbChanged)
    {
      if(maxWeightedTopCb > 0)
      {
        IndexCbBufferTop.add((int) maxWeightedIndexTopCb);
        getFieldColorPerceptTop().indexU = IndexCbBufferTop.getAverage();
      }
      else
      {
        getFieldColorPerceptTop().indexU = maxWeightedIndexTopCb;
      }
      getFieldColorPerceptTop().maxRateU = maxWeightedTopCb;
    }

    bool crChanged = IndexCrBufferTop.size() < IndexCrBufferTop.getMaxEntries() 
          || abs(IndexCrBufferTop.getAverage() - (int) maxWeightedIndexTopCr) < getBaseColorRegionPerceptTop().meanEnv.y / 20
          || lastFrame.getFrameNumber() + 30 <= getFrameInfo().getFrameNumber();

    DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:fast_adapt",
      crChanged = true;
    );

    if(crChanged)
    {
      lastFrame = getFrameInfo();
      if(maxWeightedTopCb > 0)
      {
        IndexCrBufferTop.add((int) maxWeightedIndexTopCr);
        getFieldColorPerceptTop().indexV = IndexCrBufferTop.getAverage();
      }
      else
      {
        getFieldColorPerceptTop().indexV = maxWeightedIndexTopCr;
      }
      getFieldColorPerceptTop().maxRateV = maxWeightedTopCr;
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
  getFieldColorPerceptTop().range.set(
    0,//getFieldColorPerceptTop().indexY - (int) getFieldColorPerceptTop().distY,
    getFieldColorPerceptTop().indexU - (int) getFieldColorPerceptTop().distU,
    getFieldColorPerceptTop().indexV - (int) getFieldColorPerceptTop().distV,

    255,//getFieldColorPerceptTop().indexY + (int) getFieldColorPerceptTop().distY,
    getFieldColorPerceptTop().indexU + (int) getFieldColorPerceptTop().distU,
    getFieldColorPerceptTop().indexV + (int) getFieldColorPerceptTop().distV
  ); 

  getFieldColorPercept().lastUpdated = getFrameInfo();
  getFieldColorPerceptTop().lastUpdated = getFrameInfo();

  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:write_log",
    outfile << getFrameInfo().getFrameNumber() << " " << getBaseColorRegionPercept().meanImg.y << " " << getBaseColorRegionPercept().meanImg.u << " " << getBaseColorRegionPercept().meanImg.v;
    outfile << " " << getBaseColorRegionPercept().meanEnv.y << " " << getBaseColorRegionPercept().meanEnv.u << " " << getBaseColorRegionPercept().meanEnv.v;
    outfile << " " << getFieldColorPercept().indexY << " " <<  getFieldColorPercept().indexU << " " <<  getFieldColorPercept().indexV;
    outfile << " " << getFieldColorPercept().distY << " " <<  getFieldColorPercept().distU << " " <<  getFieldColorPercept().distV;
    outfile << " " << getFieldColorPercept().maxRateY << " " <<  getFieldColorPercept().maxRateU << " " <<  getFieldColorPercept().maxRateV;

    outfile << endl;
  );

  runDebugRequests();
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

void FieldColorClassifierFull::runDebugRequests()
{
  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:TopCam:enable_plots",
    for(unsigned int i = 0; i < ColorChanelHistograms::VALUE_COUNT; i ++)
    {
      PLOT_GENERIC("FieldColorClassifierFull:TopCam:weighted_Y_histogram", i, weightedHistTopY[i]);
      PLOT_GENERIC("FieldColorClassifierFull:TopCam:weighted_Cb_histogram", i, weightedHistTopCb[i]);
      PLOT_GENERIC("FieldColorClassifierFull:TopCam:weighted_Cr_histogram", i, weightedHistTopCr[i]);
    }
  );
  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:BottomCam:enable_plots",
    for(unsigned int i = 0; i < ColorChanelHistograms::VALUE_COUNT; i ++)
    {
      PLOT_GENERIC("FieldColorClassifierFull:BottomCam:weighted_Y_histogram", i, weightedHistY[i]);
      PLOT_GENERIC("FieldColorClassifierFull:BottomCam:weighted_Cb_histogram", i, weightedHistCb[i]);
      PLOT_GENERIC("FieldColorClassifierFull:BottomCam:weighted_Cr_histogram", i, weightedHistCr[i]);
    }
  );

  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:TopCam:set_in_image",
    CANVAS_PX_TOP;
    int imageWidth = getImageTop().cameraInfo.resolutionWidth;
    int imageHeight = getImageTop().cameraInfo.resolutionHeight;
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImageTop().get(x, y);
        if
        (
          getFieldColorPerceptTop().isFieldColor(pixel)
        )
        {
          POINT_PX(ColorClasses::green, x, y);
        }
      }
    }
  );
  DEBUG_REQUEST("NeoVision:FieldColorClassifierFull:BottomCam:set_in_image",
    CANVAS_PX_BOTTOM;
    int imageWidth = getImage().cameraInfo.resolutionWidth;
    int imageHeight = getImage().cameraInfo.resolutionHeight;
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


