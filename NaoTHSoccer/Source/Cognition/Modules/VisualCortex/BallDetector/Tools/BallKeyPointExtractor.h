/**
* @file BallKeyPointExtractor.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _BallKeyPointExtractor_H_
#define _BallKeyPointExtractor_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/BodyContour.h"
// tools
#include "BestPatchList.h"
#include "Tools/DoubleCamHelpers.h"
#include "Tools/CameraGeometry.h"
//#include <algorithm>

// debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"

BEGIN_DECLARE_MODULE(BallKeyPointExtractor)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugDrawings)

  REQUIRE(FieldInfo) // needed for ball radius

  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(GameColorIntegralImage)
  REQUIRE(GameColorIntegralImageTop)

  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
END_DECLARE_MODULE(BallKeyPointExtractor)

class BallKeyPointExtractor : public BallKeyPointExtractorBase
{
public:
  virtual void execute(){} // dummy, do not use

  struct Parameter {
    double borderRadiusFactorClose;
    double borderRadiusFactorFar;

    double maxInnerGreenDensitiy;
  };

  BallKeyPointExtractor() : cameraID(CameraInfo::Bottom)
  {
    DEBUG_REQUEST_REGISTER("Vision:BallKeyPointExtractor:draw_value","", false);
  }

public:

  void calculateKeyPoints(BestPatchList& best) const {
    calculateKeyPoints(getGameColorIntegralImage(), best);
  }

  void calculateKeyPointsBetter(BestPatchList& best) const {
    //calculateKeyPoints(getBallDetectorIntegralImage(), best);
    calculateKeyPointsFast(getBallDetectorIntegralImage(), best);
    //calculateKeyPointsFull(getBallDetectorIntegralImage(), best);
  }

  // scan the integral image for white key points
  template<class ImageType>
  void calculateKeyPoints(const ImageType& integralImage, BestPatchList& best) const;

  template<class ImageType>
  void calculateKeyPointsFast(const ImageType& integralImage, BestPatchList& best) const;

  template<class ImageType>
  void calculateKeyPointsFull(const ImageType& integralImage, BestPatchList& best) const;

  BestPatchList::Patch refineKeyPoint(const BestPatchList::Patch& patch) const;

  void setParameter(const Parameter& params) {
    this->params = params;
  }

  void setCameraId(CameraInfo::CameraID id) {
    this->cameraID = id;
  }

private:

  void evaluatePatch(const GameColorIntegralImage& integralImage, BestPatchList& best, const Vector2i& point, const int size, const int border) const
  {
    int inner = integralImage.getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
    double greenBelow = integralImage.getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);

    if (inner*2 > size*size && greenBelow > 0.3)
    {
      int outer = integralImage.getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);
      double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

      // scale the patch up to the image coordinates
      best.add( 
        (point.x-border)*integralImage.FACTOR, 
        (point.y-border)*integralImage.FACTOR, 
        (point.x+size+border)*integralImage.FACTOR, 
        (point.y+size+border)*integralImage.FACTOR, 
        value);
    }
  }

  void evaluatePatch(const BallDetectorIntegralImage& integralImage, BestPatchList& best, const Vector2i& point, const int size, const int border) const
  {
    int inner = integralImage.getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
    double greenBelow = integralImage.getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);
    double greeInner = integralImage.getDensityForRect(point.x, point.y, point.x+size, point.y+size, 1);

    if (inner*2 > size*size && greenBelow > 0.3 && greeInner <= params.maxInnerGreenDensitiy)
    {
      int outer = integralImage.getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);
      double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

      // scale the patch up to the image coordinates
      best.add( 
        (point.x-border)*integralImage.FACTOR, 
        (point.y-border)*integralImage.FACTOR, 
        (point.x+size+border)*integralImage.FACTOR, 
        (point.y+size+border)*integralImage.FACTOR, 
        value);
    }
    /*
    int inner = integralImage.getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
    //double greenBelow = integralImage.getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);

    double greeInner = integralImage.getDensityForRect(point.x, point.y, point.x+size, point.y+size, 1);

    if (inner*2 > size*size && greeInner < 0.5) // && greenBelow > 0.3)
    {
      //int outer = integralImage.getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);
      //double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));
      double value = ((double)inner)/((double)(size)*(size));// (double)(inner - (outer - inner)) / ((double)(size + border)*(size + border));

      // scale the patch up to the image coordinates
      best.add( 
        (point.x-border)*integralImage.FACTOR, 
        (point.y-border)*integralImage.FACTOR, 
        (point.x+size+border)*integralImage.FACTOR, 
        (point.y+size+border)*integralImage.FACTOR, 
        value);
    }*/
  }

private:
  Parameter params;
  CameraInfo::CameraID cameraID;

  mutable double values[640/4][480/4][2];

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, Image);
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, CameraMatrix);
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, FieldPercept);
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, BodyContour);

  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, GameColorIntegralImage);
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, BallDetectorIntegralImage);
};


template<class ImageType>
void BallKeyPointExtractor::calculateKeyPoints(const ImageType& integralImage, BestPatchList& best) const
{
  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return;
  }

  // find the top point of the polygon
  int minY = getFieldPercept().getMinY();

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // todo needs a better place
  const int32_t FACTOR = integralImage.FACTOR;

  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y < (int)integralImage.getHeight(); ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getImage().cameraInfo, getFieldInfo().ballRadius,
      point.x*FACTOR, point.y*FACTOR);
    
    double radius = std::max( 6.0, estimatedRadius);
    int size   = (int)(radius*2.0/FACTOR+0.5);
    int border = (int)(radius*params.borderRadiusFactorClose/FACTOR+0.5);

    // HACK: different parameters depending on size
    if(size < 40/FACTOR) {
      border = (int)(radius*params.borderRadiusFactorFar/FACTOR+0.5);
    }
    border = std::max( 2, border);

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border >= (int)integralImage.getHeight()) {
      continue;
    }
    
    for(point.x = border; point.x+size+border < (int)integralImage.getWidth(); ++point.x)
    {
      evaluatePatch(integralImage, best, point, size, border);
    }
  }
}


template<class ImageType>
void BallKeyPointExtractor::calculateKeyPointsFast(const ImageType& integralImage, BestPatchList& best) const
{
  DEBUG_REQUEST("Vision:BallKeyPointExtractor:draw_value",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  );

  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return;
  }

  // find the top point of the polygon
  int minY = getFieldPercept().getMinY();

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // todo needs a better place
  const int32_t FACTOR = integralImage.FACTOR;

  Vector2i point;

  const int height = (int)integralImage.getHeight();
  const int width = (int)integralImage.getWidth();
  
  for(point.y = minY/FACTOR; point.y < height; ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getImage().cameraInfo, getFieldInfo().ballRadius,
      getImage().width()/2, point.y*FACTOR);
    
    // Note: we have a minimal allowed radius
    int radius = (int)(estimatedRadius / FACTOR + 0.5);
    // the minimal alowed radius
    if(radius < 2) {
      radius = 2;
    }
    
    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y < radius || point.y + radius >= height) {
      continue;
    }
    
    for(point.x = radius; point.x + radius < width; ++point.x)
    {
      //evaluatePatch(integralImage, best, point, size, border);

      if(cameraID == CameraInfo::Bottom && //point.y+radius >(int)(integralImage.getHeight()/2) &&
         getBodyContour().isOccupied(point.x*integralImage.FACTOR, (point.y+radius)*integralImage.FACTOR)) {
        continue;
      }
      
      const int innerOffset = radius/2;
      const int area = 4*radius*radius;

      int inner = integralImage.getSumForRect(point.x-radius, point.y-radius, point.x+radius, point.y+radius, 0);
      double greenInner = integralImage.getDensityForRect(point.x-innerOffset, point.y-innerOffset, point.x+innerOffset, point.y+innerOffset, 1);
      
      if (inner*2 > area && greenInner <= params.maxInnerGreenDensitiy)
      {
        double value = ((double)inner)/((double)(area));
        best.add( 
            (point.x-radius)*integralImage.FACTOR, 
            (point.y-radius)*integralImage.FACTOR, 
            (point.x+radius)*integralImage.FACTOR, 
            (point.y+radius)*integralImage.FACTOR, 
            value);
      }

      DEBUG_REQUEST("Vision:BallKeyPointExtractor:draw_value",
          double value = ((double)inner)/((double)(area));

          value = Math::clamp(value / 200.0, 0.0,1.0);
          PEN(Color(1.0,1.0-value,1.0-value,0.8),0.1);

          FILLBOX((point.x)*integralImage.FACTOR - integralImage.FACTOR/2, 
                  (point.y)*integralImage.FACTOR - integralImage.FACTOR/2, 
                  (point.x)*integralImage.FACTOR + integralImage.FACTOR/2, 
                  (point.y)*integralImage.FACTOR + integralImage.FACTOR/2);
      );
    }
  }
}


template<class ImageType>
void BallKeyPointExtractor::calculateKeyPointsFull(const ImageType& integralImage, BestPatchList& best) const
{
  DEBUG_REQUEST("Vision:BallKeyPointExtractor:draw_value",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  );

  //
  // STEP I: find the maximal height minY to be scanned in the image
  //
  if(!getFieldPercept().valid) {
    return;
  }

  // find the top point of the polygon
  int minY = getFieldPercept().getMinY();

  // double check: polygon is empty
  if(minY == (int)getImage().height() || minY < 0) {
    return;
  }

  // todo needs a better place
  const int32_t FACTOR = integralImage.FACTOR;

  Vector2i point;

  // TODO: this has to be made more general
  for(int y = 1; y+1 < 480/4; ++y) {
    for(int x = 1; x+1 < 640/4; ++x) {
      values[x][y][0] = 0.0;
    }
  }
  
  // TODO: faster reset?
  //std::fill_n(&values[0][0][0], (480/4)*(640/4)*2, 0);
  
  for(point.y = minY/FACTOR; point.y < (int)integralImage.getHeight(); ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getImage().cameraInfo, getFieldInfo().ballRadius,
      getImage().width()/2, point.y*FACTOR);
    
    
    int radius = (int)(estimatedRadius / FACTOR + 0.5);

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y < radius || point.y + radius >= (int)integralImage.getHeight()) {
      continue;
    }

    for(point.x = radius; point.x+radius < (int)integralImage.getWidth(); ++point.x)
    {
      //evaluatePatch(integralImage, best, point, size, border);

      if(cameraID == CameraInfo::Bottom && getBodyContour().isOccupied(point.x*integralImage.FACTOR, (point.y+radius)*integralImage.FACTOR)) {
        values[point.x][point.y][0] = 0.0;
        values[point.x][point.y][1] = radius;
        continue;
      }

      int inner = integralImage.getSumForRect(point.x-radius, point.y-radius, point.x+radius, point.y+radius, 0);
      double greeInner = integralImage.getDensityForRect(point.x-radius, point.y-radius, point.x+radius, point.y+radius, 1);
      const int size = radius*2;

      if (inner*2 > size*size && greeInner <= params.maxInnerGreenDensitiy)
      {
        double value = ((double)inner)/((double)(size)*(size));
        values[point.x][point.y][0] = value;
        values[point.x][point.y][1] = radius;
      }
      else 
      {
        values[point.x][point.y][0] = 0.0;
        values[point.x][point.y][1] = radius;
      }

      DEBUG_REQUEST("Vision:BallKeyPointExtractor:draw_value",
          double value = ((double)inner)/((double)(size*size));

          value = Math::clamp(value / 200.0, 0.0,1.0);
          PEN(Color(1.0,1.0-value,1.0-value,0.8),0.1);

          FILLBOX((point.x)*integralImage.FACTOR - integralImage.FACTOR/2, 
                  (point.y)*integralImage.FACTOR - integralImage.FACTOR/2, 
                  (point.x)*integralImage.FACTOR + integralImage.FACTOR/2, 
                  (point.y)*integralImage.FACTOR + integralImage.FACTOR/2);
      );
    }
  }


  for(int y = 1; y+1 < 480/4; ++y) {
    for(int x = 1; x+1 < 640/4; ++x) {
      if(values[x][y][0] > 0) {
        if( values[x][y][0] > values[x-1][y][0] && 
            values[x][y][0] > values[x+1][y][0] && 
            values[x][y][0] > values[x  ][y-1][0] && 
            values[x][y][0] > values[x  ][y+1][0] &&

            values[x][y][0] > values[x-1][y-1][0] && 
            values[x][y][0] > values[x+1][y-1][0] && 
            values[x][y][0] > values[x-1][y+1][0] && 
            values[x][y][0] > values[x+1][y+1][0])
        {
          double value = values[x][y][0];
          int rad = (int)values[x][y][1];
          best.add(
            (x-rad)*integralImage.FACTOR, 
            (y-rad)*integralImage.FACTOR, 
            (x+rad)*integralImage.FACTOR, 
            (y+rad)*integralImage.FACTOR, 
            value);
        }
      }
    }
  }
}

#endif // _BallKeyPointExtractor_H_-
