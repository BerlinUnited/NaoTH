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


template<class ImageType>
void BallPatchDetector::calculateKeyPoints(const ImageType& integralImage, BestPatchList& best) const
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

  Vector2<unsigned int> point;

  for(point.y = minY/FACTOR; point.y < integralImage.getHeight(); ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,
      static_cast<int>(point.x)*FACTOR, static_cast<int>(point.y)*FACTOR);

    double radius = std::max(6.0, estimatedRadius);
    unsigned int size   = (unsigned int)(radius*2.0/FACTOR+0.5);
    unsigned int border = (unsigned int)(radius*params.borderRadiusFactorClose/FACTOR+0.5);

    // HACK: different parameters depending on size
    if(size < 40/FACTOR) {
      border = (unsigned int)(radius*params.borderRadiusFactorFar/FACTOR+0.5);
    }
    border = std::max( 2u, border);

    // smallest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border >= integralImage.getHeight()) {
      continue;
    }

    for(point.x = border; point.x+size+border < integralImage.getWidth(); ++point.x)
    {
      evaluatePatch(integralImage, best, point, size, border);
    }
  }
}


template<class ImageType>
void BallPatchDetector::calculateKeyPointsFast(const ImageType& integralImage, BestPatchList& best) const
{
  DEBUG_REQUEST("Vision:BallPatchDetector:draw_value",
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

  Vector2<unsigned int> point;

  const unsigned int height = integralImage.getHeight();
  const unsigned int width = integralImage.getWidth();

  for(point.y = minY/FACTOR; point.y < height; ++point.y)
  {
    double estimatedRadius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,
      static_cast<int>(getImage().width()/2), static_cast<int>(point.y)*FACTOR);

    estimatedRadius = estimatedRadius / FACTOR + 0.5;
    // Note: we have a minimal allowed radius
    unsigned int radius = (estimatedRadius < 2.0) ? 2 : static_cast<unsigned int>(estimatedRadius);

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

      const unsigned int innerOffset = radius/2;
      const unsigned int area = 4*radius*radius;

      unsigned int inner = integralImage.getSumForRect(point.x-radius, point.y-radius, point.x+radius, point.y+radius, 0);
      double greenInner  = integralImage.getDensityForRect(point.x-innerOffset, point.y-innerOffset, point.x+innerOffset, point.y+innerOffset, 1);

      unsigned int below = 0;
      unsigned int radius_below = radius*2;

      if(point.y+radius_below < height) {
        below = integralImage.getSumForRect(point.x-radius, point.y+radius, point.x+radius, point.y+radius_below, 0);
      }

      if (inner*2 > area && greenInner <= params.maxInnerGreenDensitiy && below*params.area_below_factor < area)
      {
        double value = ((double)inner)/((double)(area));
        best.add(
            static_cast<int>(point.x-radius)*integralImage.FACTOR,
            static_cast<int>(point.y-radius)*integralImage.FACTOR,
            static_cast<int>(point.x+radius)*integralImage.FACTOR,
            static_cast<int>(point.y+radius)*integralImage.FACTOR,
            value);
      }

      DEBUG_REQUEST("Vision:BallPatchDetector:draw_value",
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
void BallPatchDetector::calculateKeyPointsFull(const ImageType& integralImage, BestPatchList& best) const
{
  DEBUG_REQUEST("Vision:BallPatchDetector:draw_value",
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
      getCameraMatrix(), getCameraInfo(), getFieldInfo().ballRadius,
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

#endif // BALL_PATCH_DETECTOR_H
