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

// tools
#include "BestPatchList.h"
#include "Tools/DoubleCamHelpers.h"
#include "Tools/CameraGeometry.h"

// debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"

BEGIN_DECLARE_MODULE(BallKeyPointExtractor)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo) // needed for ball radius

  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(GameColorIntegralImage)
  REQUIRE(GameColorIntegralImageTop)

  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

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
  };

  BallKeyPointExtractor() : cameraID(CameraInfo::Bottom)
  {}

public:

  void calculateKeyPoints(BestPatchList& best) const {
    calculateKeyPoints(getGameColorIntegralImage(), best);
  }

  void calculateKeyPointsBetter(BestPatchList& best) const {
    calculateKeyPoints(getBallDetectorIntegralImage(), best);
  }

  // scan the integral image for white key points
  template<class ImageType>
  void calculateKeyPoints(const ImageType& integralImage, BestPatchList& best) const;

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

private:
  Parameter params;
  CameraInfo::CameraID cameraID;

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, Image);
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, CameraMatrix);
  DOUBLE_CAM_REQUIRE(BallKeyPointExtractor, FieldPercept);

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

  // we search for key points only inside the field polygon
  const FieldPercept::FieldPoly& fieldPolygon = getFieldPercept().getValidField();

  // find the top point of the polygon
  int minY = getImage().height();
  for(int i = 0; i < fieldPolygon.length ; i++)
  {
    if(fieldPolygon.points[i].y < minY && fieldPolygon.points[i].y >= 0) {
      minY = fieldPolygon.points[i].y;
    }
  }

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

#endif // _BallKeyPointExtractor_H_
