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
  adaptationRate(0.5),
  justStarted(true),
  
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

  GT_TRACE("before FieldColorClassifierPostProcessor constructor memset");
  memset(&weightedSmoothedHistY, 0, sizeof(weightedSmoothedHistY));
  memset(&weightedSmoothedHistCb, 0, sizeof(weightedSmoothedHistCb));
  memset(&weightedHistY, 0, sizeof(weightedHistY));
  //memset(&weightedHistCb, 0, sizeof(weightedHistCb));
  GT_TRACE("after FieldColorClassifierPostProcessor constructor memset");
  
  getFieldColorPercept().init(fieldParams);
}

void FieldColorClassifierPostProcessor::execute()
{
  if(!getHistogram().colorChannelIsUptodate)
  {
    return;
  }

  weightedMeanY = 0;
  maxWeightedY = 0.0;
  maxWeightedIndexY = 0;
  maxWeightedCb = 0.0;
  maxWeightedIndexCb = 0;
  maxWeightedCr = getFieldColorPreProcessingPercept().maxWeightedCr;
  maxWeightedIndexCr = getFieldColorPreProcessingPercept().maxWeightedIndexCr;

  meanFieldY = getHistogram().getMeanFieldY();


  // set by default to the old values
  distY = (unsigned int) getFieldColorPercept().distY;
  distCb = (unsigned int) getFieldColorPercept().distCb;
  distCr = (unsigned int) getFieldColorPercept().distCr;

  maxDistY = (unsigned int) fieldParams.fieldcolorDistMax.y;
  maxDistCb = (unsigned int) fieldParams.fieldcolorDistMax.u;
  maxDistCr = (unsigned int) fieldParams.fieldcolorDistMax.v;
 

  MODIFY("FieldColorClassifier:PostProcessor_adaptationRate", adaptationRate);


  DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
    PLOT("FieldColorPercept:maxWeightedIndexY", getFieldColorPercept().maxWeightedIndexY);
    PLOT("FieldColorPercept:maxWeightedIndexCb", getFieldColorPercept().maxWeightedIndexCb);
    PLOT("FieldColorPercept:maxWeightedIndexCr", getFieldColorPercept().maxWeightedIndexCr);
    PLOT("FieldColorPercept:maxWeightedY", getFieldColorPercept().maxWeightedY);
    PLOT("FieldColorPercept:maxWeightedCb", getFieldColorPercept().maxWeightedCb);
    PLOT("FieldColorPercept:maxWeightedCr", getFieldColorPercept().maxWeightedCr);
    PLOT("FieldColorPercept:distY", getFieldColorPercept().distY);
    PLOT("FieldColorPercept:distCb", getFieldColorPercept().distCb);
    PLOT("FieldColorPercept:distCr", getFieldColorPercept().distCr);
  );

  
  //
  vector<Vector3<double> >regionList;
  Vector2<double> region;
  double area = 0.0;
  bool found = false;

  STOPWATCH_START("FieldColorClassifier:PostProcessor_Y_Cb_filtering");
  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    //double mY = max(0.0,meanFieldY - fabs(meanFieldY - (double) i));
    double mY = max(0.0,128.0 - fabs(meanFieldY - (double) i));
    double wY = mY / 128.0;
    double wYG = exp(-Math::sqr(meanFieldY - i)/(60.0*60.0));
    //double wYgS = exp(-Math::sqr(64.0 - i)/(32.0*32.0));
    //double wYG = exp(-Math::sqr(128.0 - i)/(64.0*64.0));


    // TODO: do we need it?
    weightedHistY[i] = getHistogram().weightedHistY[i];
    
    
    DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight",i, wY);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_square",i, wY * wY);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss",i, wYG);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss+linear",i, (wY + wYgS) * 0.5);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss*linear",i, wY * wYgS);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss+square",i, (wY * wY + wYgS) * 0.5);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_weight_gauss*square",i, wY * wY * wYgS);
      PLOT_GENERIC("FCCPost_weightedHistY_unfiltered",i, weightedHistY[i]);
      PLOT_GENERIC("FCCPost_weightedHistY_linear_filter",i, weightedHistY[i] * wY);
      PLOT_GENERIC("FCCPost_weightedHistY_square_filter",i, weightedHistY[i] * wY * wY);
      PLOT_GENERIC("FCCPost_weightedHistY_gauss_filter",i, weightedHistY[i] * wYG);
      //PLOT_GENERIC("FieldColorClassifierPostProcessor:weightedHistY_gauss+linear_filter",i, weightedHistY[i] * (wY + wYG) * 0.5);

      PLOT_GENERIC("FCCPost_weightedHistY_filtered",i, weightedHistY[i]);
    );

    //
    weightedSmoothedHistY[i] = smoothRungeKutta4(i, weightedHistY);


    // calculate areas of values larger than the threshold
    if(weightedSmoothedHistY[i] >= 0.25) // TODO: make parameter
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
         region.x = 0.0;
         region.y = 0.0;
      }
    }

    DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
      if(!found)
      {
        PLOT_GENERIC("FCCPost_regionsY", i, 0.0);
      }
      else
      {
        PLOT_GENERIC("FCCPost_regionsY", i, 20.0);
      }
      PLOT_GENERIC("FCCPost_weightedSmoothedHistY", i, weightedSmoothedHistY[i]);
    );


    // 
    maxWeightedIndexY = (unsigned int) meanFieldY;



    // STEP 2: find the maximum in the Cb channel
    double mCb = COLOR_CHANNEL_VALUE_COUNT - i;
    double wCb = mCb / (double) COLOR_CHANNEL_VALUE_COUNT;
    
    double smoothWeightedCb = getHistogram().weightedHistCb[i];
    weightedSmoothedHistCb[i] = smoothWeightedCb * wCb * wCb;

    
    DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
      double wCbG = exp(Math::sqr(i)/(-96.0*96.0));
   //   PLOT_GENERIC("FCCPost_weightedHistCb_weight",i, wCb);
   //   PLOT_GENERIC("FCCPost_weightedHistCb_weight_square",i, wCb * wCb);
   //   PLOT_GENERIC("FCCPost_weightedHistCb_weight_gauss",i, wCbG);
      PLOT_GENERIC("FCCPost_weightedHistCb_unfiltered",i, smoothWeightedCb);
      PLOT_GENERIC("FCCPost_weightedHistCb_gauss_filter",i, smoothWeightedCb * wCbG);
      PLOT_GENERIC("FCCPost_weightedHistCb_linear_filter",i, smoothWeightedCb * wCb);
      PLOT_GENERIC("FCCPost_weightedHistCb_square_filter",i, smoothWeightedCb * wCb * wCb);
    
      PLOT_GENERIC("FCCPost_weightedHistCb_filtered",i, getHistogram().weightedHistCb[i]);
      PLOT_GENERIC("FCCPost_weightedSmoothedHistCb",i, weightedSmoothedHistCb[i]);
    );

    if(smoothWeightedCb > maxWeightedCb)
    {
      maxWeightedCb = smoothWeightedCb;
      maxWeightedIndexCb = i;
    }
  }//end for i



  // STEP 3: find the largest region in the Y channel
  Vector3<double> maxRegion(0.0, 0.0, 0.0);
  bool foundRegion = false;
  for(unsigned int i = 0; i < regionList.size(); i++)
  {
    if(regionList[i].z > maxRegion.z)
    {
      maxRegion = regionList[i];
      foundRegion = true;
    }
  }

  // calculate the mean of the largest region
  unsigned int meanRegionBeginIndexY = 0;
  unsigned int meanRegionEndIndexY = 0;
  if(foundRegion)
  {
    meanRegionBeginIndexY = (unsigned int)maxRegion.x;
    meanRegionEndIndexY = (unsigned int)maxRegion.y;
    maxWeightedIndexY = (unsigned int) Math::round((maxRegion.x + maxRegion.y) * 0.5);
    maxWeightedY = weightedSmoothedHistY[maxWeightedIndexY];
  }



  // STEP 4: determine the bounds for the Cb channel
  STOPWATCH_STOP("FieldColorClassifier:PostProcessor_Y_Cb_filtering");

  // find lower border for Cb
  unsigned int meanRegionBeginIndexCb = maxWeightedIndexCb;
  while(meanRegionBeginIndexCb > 0 && weightedSmoothedHistCb[meanRegionBeginIndexCb] > 0.15)
  {
    meanRegionBeginIndexCb--;
  }

  // find higher border for Cb
  unsigned int meanRegionEndIndexCb = maxWeightedIndexCb;
  while(meanRegionEndIndexCb < COLOR_CHANNEL_VALUE_COUNT-1 && weightedSmoothedHistCb[meanRegionEndIndexCb] > 0.15)
  {
    meanRegionEndIndexCb++;
  }


  // find lower border for Cr
  unsigned int meanRegionBeginIndexCr = maxWeightedIndexCr;
  while(meanRegionBeginIndexCr > 0 && getFieldColorPreProcessingPercept().weightedHistCr[meanRegionBeginIndexCr] > 0.15)
  {
    meanRegionBeginIndexCr--;
  }

  // find higher border for Cr
  unsigned int meanRegionEndIndexCr = maxWeightedIndexCr;
  while(meanRegionEndIndexCr < COLOR_CHANNEL_VALUE_COUNT-1 && getFieldColorPreProcessingPercept().weightedHistCr[meanRegionEndIndexCr] > 0.15)
  {
    meanRegionEndIndexCr++;
  }



  // STEP 4: adaptation

  if(false && !justStarted)
  {
    if(meanRegionEndIndexY > maxWeightedIndexY && meanRegionBeginIndexY < maxWeightedIndexY)
    {
      maxWeightedIndexY = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexY + adaptationRate * (meanRegionBeginIndexY + meanRegionEndIndexY) * 0.5);
      double dY = (maxDistY + (meanRegionEndIndexY - meanRegionBeginIndexY) * 0.5) * 0.5;// TODO: check
      distY = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distY + adaptationRate * dY);
      
      DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
        for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
        {
          if(meanFieldY + maxDistY >= i && meanFieldY - maxDistY <= i)
          {
            PLOT_GENERIC("FCCPost_region_meanY", i, 20.0);
          }
          else
          {
            PLOT_GENERIC("FCCPost_region_meanY", i, 0.0);
          }
          if(maxWeightedIndexY + distY >= i && maxWeightedIndexY - distY<= i)
          {
            PLOT_GENERIC("FCCPost_regionY", i, 20.0);
          }
          else
          {
            PLOT_GENERIC("FCCPost_regionY", i, 0.0);
          }
        }//end for
      );
    }
    else
    {
      maxWeightedIndexY = 0;
    }

    if(meanRegionEndIndexCb > maxWeightedIndexCb && meanRegionBeginIndexCb < maxWeightedIndexCb)
    {
      maxWeightedIndexCb = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexCb + adaptationRate * maxWeightedIndexCb);
      double dCb = (maxDistCb + (meanRegionEndIndexCb - meanRegionBeginIndexCb) * 0.5) * 0.5;
      distCb = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distCb + adaptationRate * dCb);
      
      DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
        for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
        {
          if(maxWeightedIndexCb + distCb >= i && maxWeightedIndexCb - distCb<= i)
          {
            PLOT_GENERIC("FCCPost_regionCb", i, 20.0);
          }
          else
          {
            PLOT_GENERIC("FCCPost_regionCb", i, 0.0);
          }
        }
      );
    }
    else
    {
      maxWeightedIndexCb = 0;
    }

    if(meanRegionEndIndexCr > maxWeightedIndexCr && meanRegionBeginIndexCr < maxWeightedIndexCr)
    {
      maxWeightedIndexCr = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexCr + adaptationRate * maxWeightedIndexCr);
      double dCr = (maxDistCr + (meanRegionEndIndexCr - meanRegionBeginIndexCr) * 0.5) * 0.5;
      distCr = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distCr + adaptationRate * dCr);
      DEBUG_REQUEST("ImageProcessor:FieldColorClassifierPostProcessor:enable_plots",
        for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
        {
         if(maxWeightedIndexCr + distCr >= i && maxWeightedIndexCr - distCr <= i)
          {
            PLOT_GENERIC("FCCPost_regionCr", i, 20.0);
          }
          else
          {
            PLOT_GENERIC("FCCPost_regionCr", i, 0.0);
          }
        }
      );
    }
    else
    {
      maxWeightedIndexCr = 0;
    }
  }

  setPerceptData();
  runDebugRequests((int)weightedMeanY, (int)meanFieldY);
}//end execute


double FieldColorClassifierPostProcessor::smoothRungeKutta4(const unsigned int& idx, double* valueArray) const
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
    justStarted = false;

    // reset internal values
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

    // generate the percept
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
}//end setPerceptData


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
