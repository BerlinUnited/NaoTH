/*
 * File:   FieldColorClassifierPostProcessor.cpp
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
  adaptationRate(0.2),
  enablePlots(false),
  distY(0),
  distCb(0),
  distCr(0),
  maxDistY(0),
  maxDistCb(0),
  maxDistCr(0),
  meanFieldY(0.0),
  weightedMeanY(0),
  maxWeightedY(0.0),
  maxWeightedIndexY(0),
  maxWeightedCb(0.0),
  maxWeightedIndexCb(0),
  maxWeightedCr(0.0),
  maxWeightedIndexCr(0)
{
  //DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:show_Y_range", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:show_Cb_range", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:show_Cr_range", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:show_weighted_Y_histogram", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:show_weighted_Cb_histogram", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:show_weighted_Cr_histogram", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:set_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:weightedHistCr", " ", false);
  
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:weightedSmoothedHistY", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifierPostProcessor:weightedSmoothedHistCb", " ", false);



  memset(&weightedSmoothedHistY, 0, sizeof(weightedSmoothedHistY));
  memset(&weightedSmoothedHistCb, 0, sizeof(weightedSmoothedHistCb));
  //memset(&weightedHistY, 0, sizeof(weightedHistY));
  //memset(&weightedHistCb, 0, sizeof(weightedHistCb));
  //memset(&weightedHistCr, 0, sizeof(weightedHistCr));

  getFieldColorPercept().init(fieldParams);
}

void FieldColorClassifierPostProcessor::execute()
{
  const Histogram& histogram = getHistogram();
  if(!histogram.colorChannelIsUptodate)
  {
    return;
  }
  const FieldColorPreProcessingPercept &fccPre = getFieldColorPreProcessingPercept();

  FieldColorPercept& fPercept = getFieldColorPercept();

  weightedMeanY = 0;
  maxWeightedY = 0.0;
  maxWeightedIndexY = 0;
  maxWeightedCb = 0.0;
  maxWeightedIndexCb = 0;
  maxWeightedCr = fccPre.maxWeightedCr;
  maxWeightedIndexCr = fccPre.maxWeightedIndexCr;

  meanFieldY = histogram.getMeanFieldY();

  distY = (unsigned int) fPercept.distY;
  distCb = (unsigned int) fPercept.distCb;
  distCr = (unsigned int) fPercept.distCr;
  maxDistY = (unsigned int) fieldParams.fieldcolorDistMax.y;
  maxDistCb = (unsigned int) fieldParams.fieldcolorDistMax.u;
  maxDistCr = (unsigned int) fieldParams.fieldcolorDistMax.v;

  MODIFY("FieldColorClassifier:PostProcessor_adaptationRate", adaptationRate);

  double modifyDist = maxDistY;
  MODIFY("FieldColorClassifier:PostProcessor_maxDistY", modifyDist);
  maxDistY = (unsigned int) (modifyDist);

  modifyDist = maxDistCb;
  MODIFY("FieldColorClassifier:PostProcessor_maxDistCb", modifyDist);
  maxDistCb = (unsigned int) (modifyDist);

  modifyDist = maxDistCr;
  MODIFY("FieldColorClassifier:PostProcessor_maxDistCr", modifyDist);
  maxDistCr = (unsigned int) (modifyDist);

  enablePlots = false;
  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
    enablePlots = true;
  );

  //PLOT("FieldColorClassifierPostProcessor:maxWeightedIndexY", getFieldColorPercept().maxWeightedIndexY);
  //PLOT("FieldColorClassifierPostProcessor:maxWeightedIndexCb", getFieldColorPercept().maxWeightedIndexCb);
  //PLOT("FieldColorClassifierPostProcessor:maxWeightedIndexCr", getFieldColorPercept().maxWeightedIndexCr);
  //PLOT("FieldColorClassifierPostProcessor:maxWeightedY", getFieldColorPercept().maxWeightedY);
  //PLOT("FieldColorClassifierPostProcessor:maxWeightedCb", getFieldColorPercept().maxWeightedCb);
  //PLOT("FieldColorClassifierPostProcessor:maxWeightedCr", getFieldColorPercept().maxWeightedCr);
  //PLOT("FieldColorClassifierPostProcessor:distY", getFieldColorPercept().distY);
  //PLOT("FieldColorClassifierPostProcessor:distCb", getFieldColorPercept().distCb);
  //PLOT("FieldColorClassifierPostProcessor:distCr", getFieldColorPercept().distCr);


  //STOPWATCH_START("FieldColorClassifierPostProcessor:Cr_filtering");
  

  //weightedMeanY = 0;
  //maxWeightedY = 0.0;//0.5 * getFieldColorPercept().maxWeightedY;
  //maxWeightedIndexY = 0;//(unsigned int) (getFieldColorPercept().maxWeightedIndexY);
  //maxWeightedCb = 0.0;//0.85 * getFieldColorPercept().maxWeightedCb;
  //maxWeightedIndexCb = 0;//(unsigned int) (getFieldColorPercept().maxWeightedIndexCb);
  //maxWeightedCr = 0.0;//0.85 * getFieldColorPercept().maxWeightedCr;
  //maxWeightedIndexCr = 0;//(unsigned int) (getFieldColorPercept().maxWeightedIndexCr);

  //for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  //{
  //  weightedHistY[i] = 0.0;
  //  weightedHistCb[i] = 0.0;

  //  double mCr = (double) max<int>(0,  128 - i);
  //  double wCr = mCr / 128.0;
  //  double wCrG = exp(-Math::sqr(0.0 - i)/(60.0*60.0));
  //  weightedHistCr[i] = (double) getHistogram().colorChannelHistogramField[2][i];
  //  //if(enablePlots)
  //  //{
  //  //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCr_unfiltered",i, weightedHistCr[i]);
  //  //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCr_weight",i, wCr);
  //  //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCr_weight_square",i, wCr * wCr);
  //  //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCr_weight_gauss",i, wCrG);
  //  //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCr_gauss_filter",i, weightedHistCr[i] * wCrG);
  //  //}
  //  weightedHistCr[i] *= wCr;
  //  //if(enablePlots)
  //  //{
  //  //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCr_linear_filter",i, weightedHistCr[i]);
  //  //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCr_square_filter",i, weightedHistCr[i] * wCr);
  //  //}
  //  if(weightedHistCr[i] > maxWeightedCr)
  //  {
  //    maxWeightedCr = weightedHistCr[i];
  //    maxWeightedIndexCr = i;
  //  }
  //}
  //STOPWATCH_STOP("FieldColorClassifierPostProcessor:Cr_filtering");


  //STOPWATCH_START("FieldColorClassifierPostProcessor:GridWalk");
  //double meanFieldY = 0.0;
  //double meanFieldCountY = 1.0;
  //Pixel pixel;

  //for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  //{
  //  const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
  //  getImage().get(point.x, point.y, pixel);
  //  if
  //  (
  //    abs((int)pixel.v  - (int) maxWeightedIndexCr) < (int) distCr
  //  )
  //  {
  //    weightedHistY[pixel.y]++;
  //    meanFieldY += pixel.y;
  //    meanFieldCountY++;
  //    weightedHistCb[pixel.u]++;
  //  }
  //}
  //STOPWATCH_STOP("FieldColorClassifierPostProcessor:GridWalk");

  //meanFieldY /= meanFieldCountY;


  unsigned int idx;

  vector<Vector3<double> >regionList;
  Vector2<double> region;
  double area = 0.0;
  bool found = false;

  STOPWATCH_START("FieldColorClassifier:PostProcessor_Y_Cb_filtering");
  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    //double mY = max(0.0,meanFieldY - fabs(meanFieldY - (double) i));
    double mY = max(0.0,128.0 - fabs(meanFieldY - (double) i));
    double wY = mY / 128.0 ;
    double wYG = exp(-Math::sqr(meanFieldY - i)/(60.0*60.0));
    double wYgS = exp(-Math::sqr(64.0 - i)/(32.0*32.0));
    //double wYG = exp(-Math::sqr(128.0 - i)/(64.0*64.0));
    weightedHistY[i] = histogram.weightedHistY[i];
    if(enablePlots)
    {
      PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_unfiltered",i, weightedHistY[i]);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight",i, wY);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_square",i, wY * wY);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss",i, wYG);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss+linear",i, (wY + wYgS) * 0.5);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss*linear",i, wY * wYgS);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss+square",i, (wY * wY + wYgS) * 0.5);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss*square",i, wY * wY * wYgS);
      PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_gauss_filter",i, weightedHistY[i] * wYG);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_gauss+linear_filter",i, weightedHistY[i] * (wY + wYG) * 0.5);
    }

    //weightedHistY[i] *= (wY + wYgS) * 0.5;
    //if(enablePlots)
    //{
      PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_filtered",i, weightedHistY[i]);
    //  PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_square_filter",i, weightedHistY[i]  * wY);
    //}
    double smoothWeightedY = smoothRungeKutta4(i, weightedHistY);
    weightedSmoothedHistY[i] = smoothWeightedY;

    if(weightedSmoothedHistY[i] >= 0.25)
    {
      if(!found)
      {
        found = true;
        region.x = i;
      }
      area += weightedSmoothedHistY[i];
    }
    else
    {
      if(found)
      {
         found = false;
         region.y = i;
         Vector3<double> regionEntry(region.x, region.y, area);
         regionList.push_back(regionEntry);
         area = 0.0;
      }
    }

    if(enablePlots)
    {
      PLOT_GENERIC("FCCPost_weightedSmoothedHistY",i, weightedSmoothedHistY[i]);
    }
    //if
    //(
    //  weightedSmoothedHistY[i] > maxWeightedY
    //)
    //{
    //  maxWeightedY = weightedSmoothedHistY[i];
      //maxWeightedIndexY = (unsigned int) meanFieldY;
    //}
    
    double mCb = COLOR_CHANNEL_VALUE_COUNT - i;
    double wCb = mCb / (double) COLOR_CHANNEL_VALUE_COUNT;
    double wCbG = exp(Math::sqr(i)/(-96.0*96.0));

    //weightedHistCb[i] = histogram.weightedHistCb[i];
    //double smoothWeightedCb = smoothRungeKutta4(i, weightedHistCb);
    double smoothWeightedCb = histogram.weightedHistCb[i];
    weightedSmoothedHistCb[i] = smoothWeightedCb;
   if(enablePlots)
    {
   //   PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCb_unfiltered",i, weightedHistCb[i]);
   //   PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCb_weight",i, wCb);
   //   PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCb_weight_square",i, wCb * wCb);
   //   PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCb_weight_gauss",i, wCbG);
   //   PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCb_gauss_filter",i, weightedHistCb[i] * wCbG);
   }
    weightedSmoothedHistCb[i] *= wCb * wCb;
    if(enablePlots)
    {
      PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCb_filtered",i, weightedHistCb[i]);
   //   PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistCb_square_filter",i, weightedHistCb[i] * wCb);
    }
    //double smoothWeightedCb = smoothRungeKutta4(i, weightedHistCb);
    //weightedSmoothedHistCb[i] = smoothWeightedCb;
    //weightedSmoothedHistCb[i] *= wCb;
   if(enablePlots)
    {
      PLOT_GENERIC("FCCPost_weightedSmoothedHistCb",i, weightedSmoothedHistCb[i]);
    }
    if(smoothWeightedCb > maxWeightedCb)
    {
      maxWeightedCb = smoothWeightedCb;
      maxWeightedIndexCb = i;
    }
  }
   
  Vector3<double> maxRegion;
  for(unsigned int i = 0; i < regionList.size(); i++)
  {
    //PLOT_GENERIC("FieldColorClassifierPostProcessor:regions", maxRegion.x - 2, 0.0);
    //PLOT_GENERIC("FieldColorClassifierPostProcessor:regions", maxRegion.x, 15.0);
    //PLOT_GENERIC("FieldColorClassifierPostProcessor:regions", maxRegion.y, 15.0);
    //PLOT_GENERIC("FieldColorClassifierPostProcessor:regions", maxRegion.y + 2, 0.0);
    if(regionList[i].z > maxRegion.z)
    {
      maxRegion = regionList[i];
    }
  }
  maxWeightedIndexY = (unsigned int) Math::round((maxRegion.y + maxRegion.x) * 0.5);

  STOPWATCH_STOP("FieldColorClassifier:PostProcessor_Y_Cb_filtering");

  unsigned int meanRegionBeginIndexY = 0;
  unsigned int meanRegionEndIndexY = 0;

  idx = maxWeightedIndexY;
  while (idx >= 0 && meanRegionBeginIndexY == 0)
  {
    if(weightedSmoothedHistY[idx] <= 0.25)// /*log(maxWeightedY)*/ /*/ log(2.0)*/ || maxWeightedIndexY - idx >  factorDistY * maxDistY)
    {
      meanRegionBeginIndexY = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexY;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexY == 0)// && idx < getFieldColorPercept().minWhite)
  {
    if(weightedSmoothedHistY[idx] <= 0.25)// /* log(maxWeightedY)*/ /*/ log(2.0)*/ || idx - maxWeightedIndexY > factorDistY * maxDistY)
    {
      meanRegionEndIndexY = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexY > maxWeightedIndexY && meanRegionBeginIndexY < maxWeightedIndexY)
  {
//    maxWeightedIndexY = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexY + adaptationRate * maxWeightedIndexY);
    maxWeightedIndexY = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexY + adaptationRate * (meanRegionBeginIndexY + meanRegionEndIndexY) * 0.5);
    double dY = (distY + maxDistY + (meanRegionEndIndexY - meanRegionBeginIndexY) * 0.5) * 0.333;
    distY = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distY + adaptationRate * dY);
  }
  else
  {
    maxWeightedIndexY = 0;
  }

  unsigned int meanRegionBeginIndexCb = 0;
  unsigned int meanRegionEndIndexCb = 0;

  idx = maxWeightedIndexCb;
  while (idx > 0 && meanRegionBeginIndexCb == 0)
  {
    if(weightedSmoothedHistCb[idx] <= 0.5)//<= log(maxWeightedCb) || maxWeightedIndexCb - idx > maxDistCb)
    {
      meanRegionBeginIndexCb = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexCb;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCb == 0)
  {
    if(weightedSmoothedHistCb[idx] <= 0.5)//<= log(maxWeightedCb) || idx - maxWeightedIndexCb > maxDistCb)
    {
      meanRegionEndIndexCb = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexCb > maxWeightedIndexCb && meanRegionBeginIndexCb < maxWeightedIndexCb)
  {
    maxWeightedIndexCb = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexCb + adaptationRate * maxWeightedIndexCb);
    double dCb = (distCb + maxDistCb + (meanRegionEndIndexCb - meanRegionBeginIndexCb) * 0.5) * 0.333;
    distCb = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distCb + adaptationRate * dCb);
 }
  else
  {
    maxWeightedIndexCb = 0;
  }

  unsigned int meanRegionBeginIndexCr = 0;
  unsigned int meanRegionEndIndexCr = 0;

  idx = maxWeightedIndexCr;
  while (idx >= 0 && meanRegionBeginIndexCr == 0)
  {
    if(fccPre.weightedHistCr[idx] <= 0.5)//<= log(maxWeightedCr) || maxWeightedIndexCr - idx > maxDistCr)
    {
      meanRegionBeginIndexCr = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexCr;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCr == 0)
  {
    if(fccPre.weightedHistCr[idx] <= 0.5)//<= log(maxWeightedCr) || idx - maxWeightedIndexCr > maxDistCr)
    {
      meanRegionEndIndexCr = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexCr > maxWeightedIndexCr && meanRegionBeginIndexCr < maxWeightedIndexCr)
  {
    maxWeightedIndexCr = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexCr + adaptationRate * maxWeightedIndexCr);
    double dCr = (distCr + maxDistCr + (meanRegionEndIndexCr - meanRegionBeginIndexCr) * 0.5) * 0.333;
    distCr = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distCr + adaptationRate * dCr);
  }
  else
  {
    maxWeightedIndexCr = 0;
  }
  setPerceptData();
  runDebugRequests((int)weightedMeanY, (int)meanFieldY);
}

double FieldColorClassifierPostProcessor::smoothRungeKutta4(const unsigned int& idx, double* valueArray)
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

void FieldColorClassifierPostProcessor::setPerceptData()
{
  if(maxWeightedIndexY > 0 && maxWeightedIndexCb > 0 && maxWeightedIndexCr > 0)
  {
    if(distY < fieldParams.fieldColorMin.y)
    {
      distY = (unsigned int)fieldParams.fieldcolorDistMin.y;
    }
    if(distY > maxDistY)
    {
      distY = (unsigned int)maxDistY;
    }

    if(distCb < fieldParams.fieldColorMin.u)
    {
      distCb = (unsigned int)fieldParams.fieldcolorDistMin.u;
    }
    if(distCb > maxDistCb)
    {
      distCb = (unsigned int)maxDistCb;
    }

    if(distCr < fieldParams.fieldColorMin.v)
    {
      distCr = (unsigned int)fieldParams.fieldcolorDistMin.v;
    }
    if(distCr > maxDistCr)
    {
      distCr = (unsigned int)maxDistCr;
    }

    getFieldColorPercept().distY = distY;
    getFieldColorPercept().distCb = distCb;
    getFieldColorPercept().distCr = distCr;

    getFieldColorPercept().maxWeightedY = maxWeightedY;
    getFieldColorPercept().maxWeightedIndexY = maxWeightedIndexY;
    getFieldColorPercept().maxWeightedCb = maxWeightedCb;
    getFieldColorPercept().maxWeightedIndexCb = maxWeightedIndexCb;
    getFieldColorPercept().maxWeightedCr = maxWeightedCr;
    getFieldColorPercept().maxWeightedIndexCr = maxWeightedIndexCr;
    getFieldColorPercept().set();
    getFieldColorPercept().lastUpdated = getFrameInfo();
  }
}


void FieldColorClassifierPostProcessor::runDebugRequests(int weightedMeanY, int meanY)
{
//  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:show_weighted_Y_histogram",
//    LINE_PX
//    (
//      ColorClasses::gray,
//      0,
//      Math::clamp((int)getImage().cameraInfo.resolutionHeight - weightedMeanY, 0, (int)getImage().cameraInfo.resolutionHeight),
//      getImage().cameraInfo.resolutionWidth,
//      Math::clamp((int)getImage().cameraInfo.resolutionHeight - weightedMeanY, 0, (int)getImage().cameraInfo.resolutionHeight)
//    );
//
//    LINE_PX
//    (
//      ColorClasses::white,
//      0,
//      Math::clamp((int)getImage().cameraInfo.resolutionHeight - meanY, 0, (int)getImage().cameraInfo.resolutionHeight),
//      getImage().cameraInfo.resolutionWidth,
//      Math::clamp((int)getImage().cameraInfo.resolutionHeight - meanY, 0, (int)getImage().cameraInfo.resolutionHeight)
//    );
//
//    Vector2<int> last(0, Math::clamp((int) getImage().cameraInfo.resolutionHeight - (int) weightedHistY[0], 0, (int)getImage().cameraInfo.resolutionHeight));
//    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
//    {
//      LINE_PX
//      (
//        ColorClasses::gray,
//        last.x,
//        last.y,
//        x,
//         Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistY[x], 0, (int)getImage().cameraInfo.resolutionHeight)
//      );
//      last.x = x;
//      last.y = Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistY[x], 0, (int)getImage().cameraInfo.resolutionHeight);
//    }
//  );
//
//  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:show_weighted_Cb_histogram",
//    Vector2<int> last(0, Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCb[0], 0, (int)getImage().cameraInfo.resolutionHeight) );
//    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
//    {
//      LINE_PX
//      (
//        ColorClasses::skyblue,
//        last.x,
//        last.y,
//        x,
//        Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCb[x], 0, (int)getImage().cameraInfo.resolutionHeight)
//      );
//      last.x = x;
//      last.y = Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCb[x], 0, (int)getImage().cameraInfo.resolutionHeight) ;
//    }
//  );
//
//  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:show_weighted_Cr_histogram",
//    Vector2<int> last(0, Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCr[0], 0, (int)getImage().cameraInfo.resolutionHeight) );
//    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
//    {
//      LINE_PX
//      (
//        ColorClasses::orange,
//        last.x,
//        last.y,
//        x,
//        Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCr[x], 0, (int)getImage().cameraInfo.resolutionHeight)
//      );
//      last.x = x;
//      last.y = Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCr[x], 0, (int)getImage().cameraInfo.resolutionHeight) ;
//    }
//  );
//
  int imageWidth = getImage().cameraInfo.resolutionWidth;
  int imageHeight = getImage().cameraInfo.resolutionHeight;
//
//  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:show_Y_range",
//    LINE_PX
//    (
//      ColorClasses::white,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY), 0, 255),
//      0,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY), 0, 255),
//      imageHeight
//    );
//    LINE_PX
//    (
//      ColorClasses::gray,
//      Math::clamp((int) getFieldColorPercept().maxWeightedIndexY, 0, 255),
//      imageHeight,
//      Math::clamp((int) getFieldColorPercept().maxWeightedIndexY, 0, 255),
//      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedY, 0, imageHeight)
//    );
//    LINE_PX
//    (
//      ColorClasses::white,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY), 0, 255),
//      0,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY), 0, 255),
//      imageHeight
//    );
//  );
//
//  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:show_Cb_range",
//    LINE_PX
//    (
//      ColorClasses::blue,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb), 0, 255),
//      0,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb), 0, 255),
//      imageHeight
//    );
//    LINE_PX
//    (
//      ColorClasses::skyblue,
//      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCb, 0, 255),
//      imageHeight,
//      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCb, 0, 255),
//      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedCb, 0, imageHeight)
//    );
//    LINE_PX
//    (
//      ColorClasses::blue,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb), 0, 255),
//      0,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb), 0, 255),
//      imageHeight
//    );
//  );
//
//  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:show_Cr_range",
//    LINE_PX
//    (
//      ColorClasses::red,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr), 0, 255),
//      0,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr), 0, 255),
//      imageHeight
//    );
//    LINE_PX
//    (
//      ColorClasses::orange,
//      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCr, 0, 255),
//      imageHeight,
//      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCr, 0, 255),
//      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedCr, 0, imageHeight)
//    );
//    LINE_PX
//    (
//      ColorClasses::red,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr), 0, 255),
//      0,
//      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr), 0, 255),
//      imageHeight
//    );
//  );
//
////  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:set_in_colortable",
////    for(unsigned int yy = getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY; yy <= getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY; yy ++)
////    {
////      for(unsigned int cb = getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb; cb <= getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb; cb ++)
////      {
////        for(unsigned int cr = getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr; cr <= getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr; cr ++)
////        {
////          const ColorClasses::Color& color = theColorTable64.getColorClass(yy, cb, cr);
////          if(getFieldColorPercept().isOnlyFieldColor(yy, cb, cr, color))
////          {
////            theColorTable64.setColorClass(ColorClasses::green, yy, cb , cr);
////          }
////        }
////      }
////    }
////  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:set_in_image",
//    Pixel greenPix;
//    greenPix.y = getFieldColorPercept().maxWeightedIndexY;
//    greenPix.u = getFieldColorPercept().maxWeightedIndexCb;
//    greenPix.v = getFieldColorPercept().maxWeightedIndexCr;

    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

//        double diffMean = 127.0 - getColoredGrid().meanBrightness;

//        double grayDiff = log(/*fabs((double) pixel.y - 128.0)*/ + fabs(diffMean)) / log(128.0);
////        double grayDiff = fabs(127.0 - getColoredGrid().meanBrightness) / 128;
////        double grayDiffY = fabs((double) pixel.y - 128.0) / (128 * (grayDiff > 0 ? grayDiff : 1.0));
//        double grayMin = 127.0 - (grayDiff > 0 ? 32.0 / grayDiff : 32.0);
//        double grayMax = 127.0 + (grayDiff > 0 ? 32.0 / grayDiff : 32.0);

//        double whiteMin = 160.0;
//        double blackMax = 5.0;

//        if(grayDiff > 0 && getColoredGrid().meanBrightness > 127.0)
//        {
//          whiteMin += log(getColoredGrid().meanBrightness);
//          blackMax += log(getColoredGrid().meanBrightness);
//        }
//        else if(grayDiff > 0 && getColoredGrid().meanBrightness < 127.0)
//        {
//          whiteMin -= log(getColoredGrid().meanBrightness);
//          blackMax -= log(getColoredGrid().meanBrightness);
//        }

////        if
////        (
////          (double) pixel.y > whiteMin
////          &&
////          (double) pixel.u > grayMin && (double) pixel.u < grayMax
////          &&
////          (double) pixel.v > grayMin && (double) pixel.v < grayMax
////        )
////        {
////          POINT_PX(ColorClasses::black, x, y);
////        }
////        else if
////        (
////          (double) pixel.y < blackMax
////          &&
////          (double) pixel.u > grayMin && (double) pixel.u < grayMax
////          &&
////          (double) pixel.v > grayMin && (double) pixel.v < grayMax
////        )
////        {
////          POINT_PX(ColorClasses::white, x, y);
////        }
////        else
//        if
//        (
//          (double) pixel.u > grayMin && (double) pixel.u < grayMax
//          &&
//          (double) pixel.v > grayMin && (double) pixel.v < grayMax
//        )
//        {
//          if((double) pixel.y > whiteMin)
//          {
//            POINT_PX(ColorClasses::skyblue, x, y);
//          }
//          else
//          if((double) pixel.y < blackMax)
//          {
//            POINT_PX(ColorClasses::yellow, x, y);
//          }
//          else
//          {
//            POINT_PX(ColorClasses::blue, x, y);
//          }
//        }
//        //else
        if
        (
          getFieldColorPercept().isFieldColor(pixel)
        )
        {
          POINT_PX(ColorClasses::green, x, y);
        }
//        else if
//        (
//          pixel.y > getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY
//          &&
//          (double) pixel.u > grayMin && (double) pixel.u < grayMax
//          &&
//          (double) pixel.v > grayMin && (double) pixel.v < grayMax
//        )
//        {
//          POINT_PX(ColorClasses::white, x, y);
//        }
//        else if
//        (
//          pixel.v > 192
//          &&
//          pixel.v > getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr
//        )
//        {
//          POINT_PX(ColorClasses::orange, x, y);
//        }
      }
    }
  );
}
