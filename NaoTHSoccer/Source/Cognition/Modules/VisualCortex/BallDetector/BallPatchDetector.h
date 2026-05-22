#ifndef BALL_PATCH_DETECTOR_H
#define BALL_PATCH_DETECTOR_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/CameraInfo.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/BestPatchList.h"
#include "Representations/Perception/MultiBallPercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"
#include "Tools/CameraGeometry.h"

// debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"

BEGIN_DECLARE_MODULE(BallPatchDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugDrawings)

  REQUIRE(FieldInfo) // needed for ball radius

  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

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

  REQUIRE(MultiBallPercept)

  PROVIDE(BestPatchList)
  PROVIDE(BestPatchListTop)
END_DECLARE_MODULE(BallPatchDetector)

/**
 * Provides a sorted list of evaluated non-overlaping patches. The detector will
 * search for key points ( possible candiate center points for a ball patches).
 * Each key point is evaluated and if the evaluation criteria are fullfilled, a
 * patch for the projected ball size is created around the key point. The
 * provided representation will automatically sort the patches by a value
 * provided by this detector, which determines how "interesting" the key point
 * is.
 */
class BallPatchDetector : public BallPatchDetectorBase
{
  public:
    BallPatchDetector();
    virtual ~BallPatchDetector();


    virtual void execute()
    {
      execute(CameraInfo::CameraID::Bottom);
      execute(CameraInfo::CameraID::Top);
    }

  private:
    struct Parameter: public ParameterList
    {
      double borderRadiusFactorClose;
      double borderRadiusFactorFar;

      double maxInnerGreenDensitiy;

      int area_below_factor;

      Parameter() : ParameterList("BallPatchDetector")
      {
        PARAMETER_REGISTER(borderRadiusFactorClose) = 0.5;
        PARAMETER_REGISTER(borderRadiusFactorFar) = 0.8;
        PARAMETER_REGISTER(maxInnerGreenDensitiy) = 0.5;

        PARAMETER_REGISTER(area_below_factor) = 4;

        syncWithConfig();
      };
    } params;
  private:
    void execute(const CameraInfo::CameraID id);
    void addPatchByLastPercept();

  public:
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

    void evaluatePatch(const GameColorIntegralImage& integralImage, BestPatchList& best, const Vector2<unsigned int>& point, const unsigned int size, const unsigned int border) const
    {
      unsigned int inner = integralImage.getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      double greenBelow = integralImage.getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);

      if (inner*2 > size*size && greenBelow > 0.3)
      {
        unsigned int outer = integralImage.getSumForRect(point.x-border, point.y-border, point.x+size+border, point.y+size+border, 0);
        double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

        // scale the patch up to the image coordinates
        best.add(
          static_cast<int>((point.x-border)*integralImage.FACTOR),
          static_cast<int>((point.y-border)*integralImage.FACTOR),
          static_cast<int>((point.x+size+border)*integralImage.FACTOR),
          static_cast<int>((point.y+size+border)*integralImage.FACTOR),
          value);
      }
    }

    void evaluatePatch(const BallDetectorIntegralImage& integralImage, BestPatchList& best, const Vector2<unsigned int>& point, const unsigned int size, const unsigned int border) const
    {
      unsigned int inner = integralImage.getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      double greenBelow = integralImage.getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);
      double greeInner = integralImage.getDensityForRect(point.x, point.y, point.x+size, point.y+size, 1);

      if (inner*2 > size*size && greenBelow > 0.3 && greeInner <= params.maxInnerGreenDensitiy)
      {
        unsigned int outer = integralImage.getSumForRect(point.x-border, point.y-border, point.x+size+border, point.y+size+border, 0);
        double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

        // scale the patch up to the image coordinates
        best.add(
          static_cast<int>((point.x-border)*integralImage.FACTOR),
          static_cast<int>((point.y-border)*integralImage.FACTOR),
          static_cast<int>((point.x+size+border)*integralImage.FACTOR),
          static_cast<int>((point.y+size+border)*integralImage.FACTOR),
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
    CameraInfo::CameraID cameraID;

    // FIXME: the same size as integral image?
    mutable double values[naoth::IMAGE_WIDTH/4][naoth::IMAGE_HEIGHT/4][2];

    // double cam stuff
    DOUBLE_CAM_REQUIRE(BallPatchDetector, CameraInfo);
    DOUBLE_CAM_REQUIRE(BallPatchDetector, Image);
    DOUBLE_CAM_REQUIRE(BallPatchDetector, CameraMatrix);
    DOUBLE_CAM_REQUIRE(BallPatchDetector, FieldPercept);
    DOUBLE_CAM_REQUIRE(BallPatchDetector, BodyContour);

    DOUBLE_CAM_REQUIRE(BallPatchDetector, GameColorIntegralImage);
    DOUBLE_CAM_REQUIRE(BallPatchDetector, BallDetectorIntegralImage);

    DOUBLE_CAM_PROVIDE(BallPatchDetector, BestPatchList);
    DOUBLE_CAM_PROVIDE(BallPatchDetector, DebugImageDrawings);
};

#endif // BALL_PATCH_DETECTOR_H
