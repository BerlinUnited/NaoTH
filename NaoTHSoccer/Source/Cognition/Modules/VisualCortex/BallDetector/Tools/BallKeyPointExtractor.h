/**
* @file BallKeyPointExtractor.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _BallKeyPointExtractor_H_
#define _BallKeyPointExtractor_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/GameColorIntegralImage.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"

// tools
#include "BestPatchList.h"
#include "Tools/DoubleCamHelpers.h"

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
  PROVIDE(GameColorIntegralImage)
  PROVIDE(GameColorIntegralImageTop)

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

  // scan the integral image for white key points
  void calculateKeyPoints(BestPatchList& best) const;
  BestPatchList::Patch refineKeyPoint(const BestPatchList::Patch& patch) const;

  void setParameter(const Parameter& params) {
    this->params = params;
  }

  void setCameraId(CameraInfo::CameraID id) {
    this->cameraID = id;
  }

private:
  void evaluatePatch(BestPatchList& best, const Vector2i& point, const int size, const int border) const
  {
    int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
    double greenBelow = getGameColorIntegralImage().getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);

    if (inner*2 > size*size && greenBelow > 0.3)
    {
      int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);
      double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

      // scale the patch up to the image coordinates
      best.add( 
        (point.x-border)*getGameColorIntegralImage().FACTOR, 
        (point.y-border)*getGameColorIntegralImage().FACTOR, 
        (point.x+size+border)*getGameColorIntegralImage().FACTOR, 
        (point.y+size+border)*getGameColorIntegralImage().FACTOR, 
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
};

#endif // _BallKeyPointExtractor_H_
