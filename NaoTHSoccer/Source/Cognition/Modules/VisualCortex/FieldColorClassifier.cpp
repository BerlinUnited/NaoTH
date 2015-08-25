/*
 * File:   FieldColorClassifier.cpp
 * Author: claas
 * 
 * Created on 15. März 2011, 15:56
 */

#include "FieldColorClassifier.h"

using namespace std;

FieldColorClassifier::FieldColorClassifier()
: 
  cameraID(CameraInfo::Bottom),
  uniformGrid(getImage().width(), getImage().height(), 60, 40)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:markCrClassification", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:markCbClassification", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:markCrClassification", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:markCbClassification", "", false);

  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:BottomCam:HistogramPlot", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldColorClassifier:TopCam:HistogramPlot", "", false);
}

FieldColorClassifier::~FieldColorClassifier()
{

}


void FieldColorClassifier::execute(const CameraInfo::CameraID id)
{
  // TODO: set this global
  cameraID = id;

  // reset stuff
  histogramV.clear();
  histogramU.clear();
  const int SCALE_FACTOR = 265 / histogramV.size;


  // calculate the histogram in the V channel
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    
    if(!getBodyContour().isOccupied(point) && point.y > getArtificialHorizon().point(point.x).y )
    {
      POINT_PX(ColorClasses::red, point.x, point.y);
      unsigned char v = getImage().getV(point.x, point.y);
      histogramV.add(v / SCALE_FACTOR);
    }
  }

  // STEP 1: search for the maximal value in the weighted Cr histogramm 
  // CAUTION: the histogram created by the grid provider in the last frame is used
  double maxWeightedCr = 0.0;
  int maxWeightedIndexCr = -1;

  // the histogram is weighted with the function 
  // max^2(0,128-i)/128, i.e., we are interested only in the first half of it
  const int cr_max = histogramV.size/2 - histogramV.size/32;
  for(int i = 0; i < cr_max; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCr =  ((double)(cr_max - i)) / (double)cr_max;
    double weightedCr = wCr * (double) histogramV.rawData[i];

    //HACK:
    histogramV.rawData[i] = (int)(1000*weightedCr+0.5);

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

  for(int i = cr_max; i < histogramV.size; i++)
  {
    histogramV.rawData[i] = 0;
  }

  // ..and find optCr maxDiffs
  int maxIndexUpCr = maxWeightedIndexCr;
  int maxIndexDownCr = maxWeightedIndexCr;

  for (int i = maxWeightedIndexCr; i < histogramV.size; i++)
  {
    if (histogramV.rawData[i] > maxWeightedCr/8)
      maxIndexUpCr = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }
  for (int i = maxWeightedIndexCr; i >= 0; i--)
  {
    if (histogramV.rawData[i] > maxWeightedCr/8)
      maxIndexDownCr = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }

  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:markCrClassification",
    if(cameraID == CameraInfo::Bottom)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        unsigned char v = getImage().getV(x, y);
        
        if( v > maxIndexDownCr && v < maxIndexUpCr ) {
          POINT_PX(ColorClasses::red, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:markCrClassification",
    if(cameraID == CameraInfo::Top)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        unsigned char v = getImage().getV(x, y);
        
        if( v > maxIndexDownCr && v < maxIndexUpCr ) {
          POINT_PX(ColorClasses::red, x, y);
        }
      }
    }
  );


  //STEP 2
  Pixel pixel;
  for(unsigned int i = 0; i < uniformGrid.size(); i++)
  {
    const Vector2i& point = uniformGrid.getPoint(i);
    
    if(!getBodyContour().isOccupied(point) && point.y > getArtificialHorizon().point(point.x).y )
    {
      POINT_PX(ColorClasses::red, point.x, point.y);
      getImage().get(point.x, point.y, pixel);
      
      //if( pixel.v > maxIndexDownCr && pixel.v < maxIndexUpCr ) {
        histogramU.add(pixel.u / SCALE_FACTOR);
      //}
    }
  }

  double maxWeightedCb = 0.0;
  int maxWeightedIndexCb = -1;

  // the histogram is weighted with the function 
  // max^2(0,128-i)/128, i.e., we are interested only in the first half of it
  const int cb_max = histogramU.size/2;
  for(int i = 0; i < cb_max; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCb =  ((double)(cb_max - i)) / (double)cb_max;
    double weightedCb = wCb * (double) histogramU.rawData[i];

    //HACK:
    histogramU.rawData[i] = (int)(1000*weightedCb+0.5);

    // search for maximum in the wighted Cr channel
    if(weightedCb > maxWeightedCb)
    {
      maxWeightedCb = weightedCb;
      maxWeightedIndexCb = i;
    }
  }

  // no green candidates found
  if(maxWeightedIndexCb < 0) {
    return;
  }

  for(int i = cb_max; i < histogramU.size; i++)
  {
    histogramU.rawData[i] = 0;
  }

  // ..and find optCr maxDiffs
  int maxIndexUpCb = maxWeightedIndexCb;
  int maxIndexDownCb = maxWeightedIndexCb;

  for (int i = maxWeightedIndexCb; i < histogramU.size; i++)
  {
    if (histogramU.rawData[i] > maxWeightedCb/8)
      maxIndexUpCb = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }
  for (int i = maxWeightedIndexCb; i >= 0; i--)
  {
    if (histogramU.rawData[i] > maxWeightedCb/8)
      maxIndexDownCb = i*SCALE_FACTOR + SCALE_FACTOR/2;
    else
      break;
  }


  DEBUG_REQUEST("Vision:FieldColorClassifier:BottomCam:markCbClassification",
    if(cameraID == CameraInfo::Bottom)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        if( pixel.v > maxIndexDownCr && pixel.v < maxIndexUpCr && pixel.u > maxIndexDownCb && pixel.u < maxIndexUpCb ) {
          POINT_PX(ColorClasses::yellow, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("Vision:FieldColorClassifier:TopCam:markCbClassification",
    if(cameraID == CameraInfo::Top)
    for(unsigned int x = 0; x < getImage().width(); x+=4) {
      for(unsigned int y = 0; y < getImage().height(); y+=4) {
        getImage().get(x, y, pixel);
        
        if( pixel.v > maxIndexDownCr && pixel.v < maxIndexUpCr && pixel.u > maxIndexDownCb && pixel.u < maxIndexUpCb ) {
          POINT_PX(ColorClasses::yellow, x, y);
        }
      }
    }
  );

}//end execute


