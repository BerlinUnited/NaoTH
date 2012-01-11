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
//  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:set_in_colortable", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:FieldColorClassifier:set_in_image", " ", false);

  memset(&weightedSmoothedHistY, 0, sizeof(weightedSmoothedHistY));
  memset(&weightedSmoothedHistCb, 0, sizeof(weightedSmoothedHistCb));
  memset(&weightedHistY, 0, sizeof(weightedHistY));
  memset(&weightedHistCb, 0, sizeof(weightedHistCb));
  memset(&weightedHistCr, 0, sizeof(weightedHistCr));

//  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
//  {
//    weightedSmoothedHistY[i] = 0.0;
//    weightedSmoothedHistCb[i] = 0.0;
//    weightedHistY[i] = 0.0;
//    weightedHistCb[i] = 0.0;
//    weightedHistCr[i] = 0.0;
//  }

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

  unsigned int maxDistY = MAX_FIELD_COLOR_Y_CHANNEL_DIST;
  unsigned int maxDistCb = MAX_FIELD_COLOR_Cb_CHANNEL_DIST;
  unsigned int maxDistCr = MAX_FIELD_COLOR_Cr_CHANNEL_DIST;

  unsigned int idx;

  double modifyDist = maxDistY;
  MODIFY("FieldColorClassifier:maxDistY", modifyDist);
  maxDistY = (unsigned int) modifyDist * getBaseColorRegionPercept().meanY / 128;

  modifyDist = maxDistCb;
  MODIFY("FieldColorClassifier:maxDistCb", modifyDist);
  maxDistCb = (unsigned int) modifyDist * getBaseColorRegionPercept().meanCb / 128;

  modifyDist = maxDistCr;
  MODIFY("FieldColorClassifier:maxDistCr", modifyDist);
  maxDistCr = (unsigned int) modifyDist * getBaseColorRegionPercept().meanCr / 128;

  double maxWeightedY = 0.5 * getFieldColorPercept().maxWeightedY;
  unsigned int maxWeightedIndexY = getFieldColorPercept().maxWeightedIndexY;

  double weightedMeanY = 0;

  double maxWeightedCb = 0.85 * getFieldColorPercept().maxWeightedCb;
  unsigned int maxWeightedIndexCb = getFieldColorPercept().maxWeightedIndexCb;

  double maxWeightedCr = 0.85 * getFieldColorPercept().maxWeightedCr;
  unsigned int maxWeightedIndexCr = getFieldColorPercept().maxWeightedIndexCr;

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    weightedHistY[i] = 0.0;
    weightedHistCb[i] = 0.0;

    double mCr = max(0.0, (double) halfChannelWidth - (double) i);
    double wCr = mCr / (double) halfChannelWidth;
    weightedHistCr[i] = wCr * (double) getHistogram().colorChannelHistogramField[2][i];
    //search for max Cr channel value with weight w

    if
    (weightedHistCr[i] > maxWeightedCr)
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

  if(meanRegionEndIndexCr > 0 && meanRegionBeginIndexCr > 0)
  {
    maxWeightedIndexCr = (meanRegionBeginIndexCr + meanRegionEndIndexCr + getFieldColorPercept().maxWeightedIndexCr) / 3;
    distCr = (meanRegionEndIndexCr - meanRegionBeginIndexCr + maxDistCr + (unsigned int)getFieldColorPercept().distCr) / 4;
  }

  STOPWATCH_STOP("FieldColorClassifier:Cb_Cr_filtering");


  STOPWATCH_START("FieldColorClassifier:GridWalk");
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
      if
      (
        abs((int)(pixel.u  - getFieldColorPercept().distCb ) <= (int) maxWeightedIndexCb)
        &&
        pixel.y > MIN_FIELD_COLOR_Y_LEVEL
        &&
        pixel.y < MAX_FIELD_COLOR_Y_LEVEL
//        &&
//        !getBlackAndWhitePercept().isWhite(pixel)
        &&
        getBaseColorRegionPercept().isGreenOrBlue(pixel)
      )
      {
        weightedHistY[pixel.y]++;
        meanFieldY += pixel.y;
        meanFieldCountY++;

        weightedHistCb[pixel.u]++;
      }
    }
  }
  STOPWATCH_STOP("FieldColorClassifier:GridWalk");

  STOPWATCH_START("FieldColorClassifier:Y_filtering");
  meanFieldY /= meanFieldCountY;
  double factorDistY =  ((double) halfChannelWidth - fabs((double) halfChannelWidth - meanFieldY) ) / (double) halfChannelWidth;

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    double mCb = COLOR_CHANNEL_VALUE_COUNT - i;
    double wCb = mCb / (double) COLOR_CHANNEL_VALUE_COUNT;
    weightedHistCb[i] *= wCb;//= wCb * (double) getHistogram().colorChannelHistogramField[1][i];
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
    if(weightedSmoothedHistY[i] > maxWeightedY)
    {
      maxWeightedY = weightedSmoothedHistY[i];
      maxWeightedIndexY = i;
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
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexY == 0)// && idx < getFieldColorPercept().minWhite)
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

    if(meanRegionEndIndexCb > 0 && meanRegionBeginIndexCb > 0)
    {
      maxWeightedIndexCb = (meanRegionBeginIndexCb + meanRegionEndIndexCb + getFieldColorPercept().maxWeightedIndexCb) / 3;
      distCb = (meanRegionEndIndexCb - meanRegionBeginIndexCb + maxDistCb + (unsigned int)getFieldColorPercept().distCb) / 4;
    }

  STOPWATCH_STOP("FieldColorClassifier:Y_filtering");

  for(unsigned int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    weightedHistY[i] = weightedSmoothedHistY[i];
    weightedHistCb[i] = weightedSmoothedHistCb[i];
  }

  if(meanRegionEndIndexY > 0 && meanRegionBeginIndexY > 0)
  {
    maxWeightedIndexY = (meanRegionBeginIndexY + meanRegionEndIndexY + getFieldColorPercept().maxWeightedIndexY) / 3;
    distY =(meanRegionEndIndexY - meanRegionBeginIndexY + maxDistY + (unsigned int)getFieldColorPercept().distY) / 4;
  }

  double fY = (double) meanFieldY / 128;
  distY = (unsigned int) (distY * fY) + MIN_FIELD_COLOR_Y_CHANNEL_DIST;
  if(distY < MIN_FIELD_COLOR_Y_CHANNEL_DIST)
  {
    distY = MIN_FIELD_COLOR_Y_CHANNEL_DIST;
  }
  if(distY > maxDistY)
  {
    distY = maxDistY;
  }

  double fCb = log((double) maxWeightedCb) / log(2.0) * 0.1;
  distCb += (unsigned int) (distCb * fCb);
  if(distCb < MIN_FIELD_COLOR_Cb_CHANNEL_DIST)
  {
    distCb = MIN_FIELD_COLOR_Cb_CHANNEL_DIST;
  }
  if(distCb > maxDistCb)
  {
    distCb = maxDistCb;
  }

  double fCr = log((double) maxWeightedCr) / log(2.0) * 0.1;
  distCr += (unsigned int) (distCr * fCr);
  if(distCr < MIN_FIELD_COLOR_Cr_CHANNEL_DIST)
  {
    distCr = MIN_FIELD_COLOR_Cr_CHANNEL_DIST;
  }
  if(distCr > maxDistCr)
  {
    distCr = maxDistCr;
  }

  if(distY > 0)
  {
    getFieldColorPercept().distY = distY;
  }

  if(distCb > 0)
  {
    getFieldColorPercept().distCb = distCb;
  }

  if(distCr > 0)
  {
    getFieldColorPercept().distCr = distCr;
  }
  if(maxWeightedY > 3.0 && maxWeightedCb > 3.0 && maxWeightedCr > 3.0)
  {
    //if(maxWeightedY > 2.0)
    {
      getFieldColorPercept().maxWeightedY = maxWeightedY;
      getFieldColorPercept().maxWeightedIndexY = maxWeightedIndexY;
    }
    //if(maxWeightedCb > 2.0)
    {
      getFieldColorPercept().maxWeightedCb = maxWeightedCb;
      getFieldColorPercept().maxWeightedIndexCb = maxWeightedIndexCb;
    }
    //if(maxWeightedCr > 2.0)
    {
      getFieldColorPercept().maxWeightedCr = maxWeightedCr;
      getFieldColorPercept().maxWeightedIndexCr = maxWeightedIndexCr;
    }
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
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY), 0, imageWidth),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY - getFieldColorPercept().distY), 0, imageWidth),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexY, 0, imageWidth),
      imageHeight,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexY, 0, imageWidth),
      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedY, 0, imageHeight)
    );
    LINE_PX
    (
      ColorClasses::white,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY), 0, imageWidth),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY), 0, imageWidth),
      imageHeight
    );
  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_Cb_range",
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb), 0, imageWidth),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb - getFieldColorPercept().distCb), 0, imageWidth),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::skyblue,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCb, 0, imageWidth),
      imageHeight,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCb, 0, imageWidth),
      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedCb, 0, imageHeight)
    );
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb), 0, imageWidth),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCb + getFieldColorPercept().distCb), 0, imageWidth),
      imageHeight
    );
  );

  DEBUG_REQUEST("ImageProcessor:FieldColorClassifier:show_Cr_range",
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr), 0, imageWidth),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr - getFieldColorPercept().distCr), 0, imageWidth),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::orange,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCr, 0, imageWidth),
      imageHeight,
      Math::clamp((int) getFieldColorPercept().maxWeightedIndexCr, 0, imageWidth),
      Math::clamp(imageHeight - (int) getFieldColorPercept().maxWeightedCr, 0, imageHeight)
    );
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr), 0, imageWidth),
      0,
      Math::clamp((int) (getFieldColorPercept().maxWeightedIndexCr + getFieldColorPercept().distCr), 0, imageWidth),
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
