#ifndef NOGREENOBSTACLEDETECTOR_H
#define NOGREENOBSTACLEDETECTOR_H

#include <iostream>

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/DoubleCamHelpers.h"

// Representations
//#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/Obstacle.h"

// Tools
#include "Tools/CameraGeometry.h"

BEGIN_DECLARE_MODULE(NoGreenObstacleDetector)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

  PROVIDE(VisionObstacle)

  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
END_DECLARE_MODULE(NoGreenObstacleDetector)

class NoGreenObstacleDetector: public NoGreenObstacleDetectorBase
{
public:
  NoGreenObstacleDetector();
  ~NoGreenObstacleDetector();

  virtual void execute();

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("NoGreenObstacleDetector")
    {
      PARAMETER_REGISTER(detector_field_offset) = 200.;

      PARAMETER_REGISTER(detector_width) = 280.;
      PARAMETER_REGISTER(detector_range) = 200.;

      PARAMETER_REGISTER(min_expected_area) = .8;

      PARAMETER_REGISTER(max_green_density) = .5;

      syncWithConfig();
    }
    double detector_field_offset;
    double detector_width;
    double detector_range;
    double min_expected_area;

    // maximal green density witch still is classified as an obstacle
    double max_green_density;
  } params;

  class DetectorField
  {
  public:
    DetectorField() : edges(4) {
      // indices are expected to look like
      // (0) 2 --- 3 (x)
      //      |   |
      // (y) 0 --- 1
      edges.resize(4);
    }

    DetectorField(Vector2i bottomLeft, Vector2i bottomRight, Vector2i topLeft, Vector2i topRight) : edges(4) {
      edges = {bottomLeft, bottomRight, topLeft, topRight};
    }

    void rectify() {
      int maxX = edges[0].x,
          maxY = edges[0].y,
          minX = edges[0].x,
          minY = edges[0].y;

      for (size_t i=1; i<4; i++) {
        maxX = std::max(maxX, edges[i].x);
        maxY = std::max(maxY, edges[i].y);
        minX = std::min(minX, edges[i].x);
        minY = std::min(minY, edges[i].y);
      }

      edges[0] = Vector2i(minX, maxY);
      edges[1] = Vector2i(maxX, maxY);
      edges[2] = Vector2i(minX, minY);
      edges[3] = Vector2i(maxX, minY);
    }

    bool limit(int maxX, int maxY, int minX, int minY) {
      // if field is not a rectangle it needs to be rectified first

      // check if rectangle is at least partially inside limits
      if(edges[0].x > maxX || edges[1].x < minX) {
        return false;
      }
      else if(edges[2].y > maxY || edges[0].y < minY) {
        return false;
      }

      edges[0].x = edges[2].x = std::max(edges[0].x, minX);
      edges[1].x = edges[3].x = std::min(edges[1].x, maxX);

      edges[0].y = edges[1].y = std::min(edges[0].y, maxY);
      edges[2].y = edges[3].y = std::max(edges[2].y, minY);

      return true;
    }

    int area() {
      // expects field to be a rectangle
      int x = maxX() - minX();
      int y = maxY() - minY();

      return x*y;
    }

    int pixels() {
      // expects field to be a rectangle
      int x = maxX() - minX() + 1;
      int y = maxY() - minY() + 1;

      return x*y;
    }

    int minX() {
      return edges[2].x;
    }

    int minY() {
      return edges[2].y;
    }

    int maxX() {
      return edges[1].x;
    }

    int maxY() {
      return edges[1].y;
    }

    int green(const BallDetectorIntegralImage& integralImage) {
      int factor = integralImage.FACTOR;
      // HACK: not correct because of integer division
      return integralImage.getSumForRect(minX()/factor, minY()/factor, maxX()/factor, maxY()/factor, 1) * (factor * factor);
    }

    int greenLeft(const BallDetectorIntegralImage& integralImage) {
      int factor = integralImage.FACTOR;
      int halfY = (maxY() - minY()) / 2;
      // HACK: not correct because of integer division
      return integralImage.getSumForRect(minX()/factor, minY()/factor, maxX()/factor, (maxY()-halfY)/factor, 1) * (factor * factor);
    }

    int greenRight(const BallDetectorIntegralImage& integralImage) {
      int factor = integralImage.FACTOR;
      int halfY = (maxY() - minY()) / 2;
      // HACK: not correct because of integer division
      return integralImage.getSumForRect(minX()/factor, (minY()+halfY)/factor, maxX()/factor, maxY()/factor, 1) * (factor * factor);
    }

    std::vector<Vector2i> edges;
  };

  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(NoGreenObstacleDetector, BallDetectorIntegralImage)
  DOUBLE_CAM_REQUIRE(NoGreenObstacleDetector, CameraInfo)
};

#endif // NOGREENOBSTACLEDETECTOR_H
