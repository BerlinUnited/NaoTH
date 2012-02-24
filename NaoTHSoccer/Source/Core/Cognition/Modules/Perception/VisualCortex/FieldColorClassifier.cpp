/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 * 
 * Created on 15. März 2011, 15:56
 */

#include "FieldColorClassifier.h"

FieldColorClassifier::FieldColorClassifier()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:show_Y_range", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:show_Cb_range", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:show_Cr_range", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:show_weighted_Y_histogram", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:show_weighted_Cb_histogram", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:show_weighted_Cr_histogram", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:set_in_image", " ", false);

  memset(&weightedSmoothedHistY, 0, sizeof(weightedSmoothedHistY));
  memset(&weightedSmoothedHistCb, 0, sizeof(weightedSmoothedHistCb));
  memset(&weightedHistY, 0, sizeof(weightedHistY));
  memset(&weightedHistCb, 0, sizeof(weightedHistCb));
  memset(&weightedHistCr, 0, sizeof(weightedHistCr));

  getFieldColorPercept().init(fieldParams);
  adaptationRate = 0.2;
}

void FieldColorClassifier::execute()
{

  STOPWATCH_START("FieldColorClassifier:Cb_Cr_filtering");
  if(!getHistogram().colorChannelIsUptodate)
  {
    return;
  }

  int halfChannelWidth = COLOR_CHANNEL_VALUE_COUNT / 2;

  unsigned int distY = (unsigned int)getFieldColorPercept().distY;
  unsigned int distCb = (unsigned int)getFieldColorPercept().distCb;
  unsigned int distCr = (unsigned int)getFieldColorPercept().distCr;

  unsigned int maxDistY = (unsigned int)fieldParams.fieldcolorDistMax.y;
  unsigned int maxDistCb = (unsigned int)fieldParams.fieldcolorDistMax.u;
  unsigned int maxDistCr = (unsigned int)fieldParams.fieldcolorDistMax.v;

  unsigned int idx;

  MODIFY("FieldColorClassifier:adaptationRate", adaptationRate);

  double modifyDist = maxDistY;
  MODIFY("FieldColorClassifier:maxDistY", modifyDist);
  maxDistY = (unsigned int) (modifyDist);

  modifyDist = maxDistCb;
  MODIFY("FieldColorClassifier:maxDistCb", modifyDist);
  maxDistCb = (unsigned int) (modifyDist);

  modifyDist = maxDistCr;
  MODIFY("FieldColorClassifier:maxDistCr", modifyDist);
  maxDistCr = (unsigned int) (modifyDist);

  double maxWeightedY = 0.5 * getFieldColorPercept().maxWeightedY;
  unsigned int maxWeightedIndexY = (unsigned int) (getFieldColorPercept().maxWeightedIndexY);

  double weightedMeanY = 0;

  double maxWeightedCb = 0.85 * getFieldColorPercept().maxWeightedCb;
  unsigned int maxWeightedIndexCb = (unsigned int) (getFieldColorPercept().maxWeightedIndexCb);

  double maxWeightedCr = 0.85 * getFieldColorPercept().maxWeightedCr;
  unsigned int maxWeightedIndexCr = (unsigned int) (getFieldColorPercept().maxWeightedIndexCr);

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    weightedHistY[i] = 0.0;
    weightedHistCb[i] = 0.0;

    double mCr = max(0.0, (double) halfChannelWidth - (double) i);
    double wCr = mCr / (double) halfChannelWidth;
    weightedHistCr[i] = wCr * (double) getHistogram().colorChannelHistogramField[2][i];
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
  while (idx >= 0 && meanRegionBeginIndexCr == 0)
  {
    if(weightedHistCr[idx] <= 0.5)//<= log(maxWeightedCr) || maxWeightedIndexCr - idx > maxDistCr)
    {
      meanRegionBeginIndexCr = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexCr;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCr == 0)
  {
    if(weightedHistCr[idx] <= 0.5)//<= log(maxWeightedCr) || idx - maxWeightedIndexCr > maxDistCr)
    {
      meanRegionEndIndexCr = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexCr > maxWeightedIndexCr && meanRegionBeginIndexCr < maxWeightedIndexCr)
  {
    maxWeightedIndexCr = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexCr + adaptationRate * maxWeightedIndexCr);
    double dCr = (distCr + maxDistCr) * 0.5;
    distCr = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distCr + adaptationRate * dCr);
  }
  else
  {
    maxWeightedIndexCr = 0;
  }

  STOPWATCH_STOP("FieldColorClassifier:Cb_Cr_filtering");


  STOPWATCH_START("FieldColorClassifier:GridWalk");
  double meanFieldY = 0.0;
  double meanFieldCountY = 1.0;
  Pixel pixel;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    getImage().get(point.x, point.y, pixel);
    if
    (
      abs((int)pixel.v  - (int) maxWeightedIndexCr) < (int) distCr
    )
    {
      weightedHistY[pixel.y]++;
      meanFieldY += pixel.y;
      meanFieldCountY++;
      weightedHistCb[pixel.u]++;
    }
  }
  STOPWATCH_STOP("FieldColorClassifier:GridWalk");

  STOPWATCH_START("FieldColorClassifier:Y_filtering");
  meanFieldY /= meanFieldCountY;

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCb = COLOR_CHANNEL_VALUE_COUNT - i;
    double wCb = mCb / (double) COLOR_CHANNEL_VALUE_COUNT;
    weightedHistCb[i] *= wCb;
    double smoothWeightedCb = smoothRungeKutta4(i, weightedHistCb);
    weightedSmoothedHistCb[i] = smoothWeightedCb;
    if(smoothWeightedCb > maxWeightedCb)
    {
      maxWeightedCb = smoothWeightedCb;
      maxWeightedIndexCb = i;
    }

    double mY = max(0.0,meanFieldY - fabs(meanFieldY - (double) i));
    double wY = mY / meanFieldY ;
    weightedHistY[i] *= wY;
    double smoothWeightedY = smoothRungeKutta4(i, weightedHistY);
    weightedSmoothedHistY[i] = smoothWeightedY;
    //search for max Y channel value with weight w
    if
    (
      weightedSmoothedHistY[i] > maxWeightedY
    )
    {
      maxWeightedY = weightedSmoothedHistY[i];
      maxWeightedIndexY = i;
    }
  }

   
  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    weightedHistY[i] = weightedSmoothedHistY[i];
    weightedHistCb[i] = weightedSmoothedHistCb[i];
  }

  unsigned int meanRegionBeginIndexY = 0;
  unsigned int meanRegionEndIndexY = 0;

  idx = maxWeightedIndexY;
  while (idx >= 0 && meanRegionBeginIndexY == 0)
  {
    if(weightedSmoothedHistY[idx] <= 0.5)// /*log(maxWeightedY)*/ /*/ log(2.0)*/ || maxWeightedIndexY - idx >  factorDistY * maxDistY)
    {
      meanRegionBeginIndexY = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexY;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexY == 0)// && idx < getFieldColorPercept().minWhite)
  {
    if(weightedSmoothedHistY[idx] <= 0.5)// /* log(maxWeightedY)*/ /*/ log(2.0)*/ || idx - maxWeightedIndexY > factorDistY * maxDistY)
    {
      meanRegionEndIndexY = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexY > maxWeightedIndexY && meanRegionBeginIndexY < maxWeightedIndexY)
  {
    maxWeightedIndexY = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexY + adaptationRate * maxWeightedIndexY);
    double dY = (distY + maxDistY) * 0.5;
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
    if(weightedHistCb[idx] <= 0.5)//<= log(maxWeightedCb) || maxWeightedIndexCb - idx > maxDistCb)
    {
      meanRegionBeginIndexCb = idx;
    }
    idx--;
  }

  idx = maxWeightedIndexCb;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCb == 0)
  {
    if(weightedHistCb[idx] <= 0.5)//<= log(maxWeightedCb) || idx - maxWeightedIndexCb > maxDistCb)
    {
      meanRegionEndIndexCb = idx;
    }
    idx++;
  }

  if(meanRegionEndIndexCb > maxWeightedIndexCb && meanRegionBeginIndexCb < maxWeightedIndexCb)
  {
    maxWeightedIndexCb = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().maxWeightedIndexCb + adaptationRate * maxWeightedIndexCb);
    double dCb = (distCb + maxDistCb) * 0.5;
    distCb = (unsigned int) ((1 - adaptationRate) * getFieldColorPercept().distCb + adaptationRate * dCb);
 }
  else
  {
    maxWeightedIndexCb = 0;
  }

  STOPWATCH_STOP("FieldColorClassifier:Y_filtering");


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
  }
  getFieldColorPercept().lastUpdated = getFrameInfo();

  runDebugRequests((int)weightedMeanY, (int)meanFieldY);
}

double FieldColorClassifier::smoothRungeKutta4(const unsigned int& idx, double* valueArray)
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

void FieldColorClassifier::runDebugRequests(int weightedMeanY, int meanY)
{
  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_weighted_Y_histogram",
    LINE_PX
    (
      ColorClasses::gray,
      0,
      Math::clamp((int)getImage().cameraInfo.resolutionHeight - weightedMeanY, 0, (int)getImage().cameraInfo.resolutionHeight),
      getImage().cameraInfo.resolutionWidth,
      Math::clamp((int)getImage().cameraInfo.resolutionHeight - weightedMeanY, 0, (int)getImage().cameraInfo.resolutionHeight)
    );

    LINE_PX
    (
      ColorClasses::white,
      0,
      Math::clamp((int)getImage().cameraInfo.resolutionHeight - meanY, 0, (int)getImage().cameraInfo.resolutionHeight),
      getImage().cameraInfo.resolutionWidth,
      Math::clamp((int)getImage().cameraInfo.resolutionHeight - meanY, 0, (int)getImage().cameraInfo.resolutionHeight)
    );

    Vector2<int> last(0, Math::clamp((int) getImage().cameraInfo.resolutionHeight - (int) weightedHistY[0], 0, (int)getImage().cameraInfo.resolutionHeight));
    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
    {
      LINE_PX
      (
        ColorClasses::gray,
        last.x,
        last.y,
        x,
         Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistY[x], 0, (int)getImage().cameraInfo.resolutionHeight)
      );
      last.x = x;
      last.y = Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistY[x], 0, (int)getImage().cameraInfo.resolutionHeight);
    }
  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_weighted_Cb_histogram",
    Vector2<int> last(0, Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCb[0], 0, (int)getImage().cameraInfo.resolutionHeight) );
    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
    {
      LINE_PX
      (
        ColorClasses::skyblue,
        last.x,
        last.y,
        x,
        Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCb[x], 0, (int)getImage().cameraInfo.resolutionHeight)
      );
      last.x = x;
      last.y = Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCb[x], 0, (int)getImage().cameraInfo.resolutionHeight) ;
    }
  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_weighted_Cr_histogram",
    Vector2<int> last(0, Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCr[0], 0, (int)getImage().cameraInfo.resolutionHeight) );
    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
    {
      LINE_PX
      (
        ColorClasses::orange,
        last.x,
        last.y,
        x,
        Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCr[x], 0, (int)getImage().cameraInfo.resolutionHeight)
      );
      last.x = x;
      last.y = Math::clamp((int)getImage().cameraInfo.resolutionHeight - (int) weightedHistCr[x], 0, (int)getImage().cameraInfo.resolutionHeight) ;
    }
  );

  int imageWidth = getImage().cameraInfo.resolutionWidth;
  int imageHeight = getImage().cameraInfo.resolutionHeight;

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_Y_range",
    LINE_PX
    (
      ColorClasses::white,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY), 0, 255),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexY, 0, 255),
      imageHeight,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexY, 0, 255),
      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedY, 0, imageHeight)
    );
    LINE_PX
    (
      ColorClasses::white,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY), 0, 255),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY), 0, 255),
      imageHeight
    );
  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_Cb_range",
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb), 0, 255),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::skyblue,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCb, 0, 255),
      imageHeight,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCb, 0, 255),
      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedCb, 0, imageHeight)
    );
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb), 0, 255),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb), 0, 255),
      imageHeight
    );
  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_Cr_range",
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr), 0, 255),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::orange,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCr, 0, 255),
      imageHeight,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCr, 0, 255),
      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedCr, 0, imageHeight)
    );
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr), 0, 255),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr), 0, 255),
      imageHeight
    );
  );

//  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:set_in_colortable",
//    for(unsigned int yy = getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY; yy <= getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY; yy ++)
//    {
//      for(unsigned int cb = getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb; cb <= getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb; cb ++)
//      {
//        for(unsigned int cr = getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr; cr <= getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr; cr ++)
//        {
//          const ColorClasses::Color& color = theColorTable64.getColorClass(yy, cb, cr);
//          if(getFieldColorPercept().isOnlyFieldColor(yy, cb, cr, color))
//          {
//            theColorTable64.setColorClass(ColorClasses::green, yy, cb , cr);
//          }
//        }
//      }
//    }
//  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:set_in_image",
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
