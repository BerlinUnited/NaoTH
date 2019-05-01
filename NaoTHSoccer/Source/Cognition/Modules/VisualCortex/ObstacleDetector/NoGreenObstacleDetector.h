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

// Tools
#include "Tools/CameraGeometry.h"

BEGIN_DECLARE_MODULE(NoGreenObstacleDetector)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

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

      syncWithConfig();
    }
    double detector_field_offset;
    double detector_width;
    double detector_range;
  } params;

  template <typename T>
  class DetectorField
  {
  public:
    DetectorField() : edges(4) {
      edges.resize(4);
    }

    DetectorField(Vector2<T> x1, Vector2<T> x2, Vector2<T> x3, Vector2<T> x4) : edges(4) {
      edges = {x1, x2, x3, x4};
    }

    void squarify() {
      T maxX = edges[0].x,
        maxY = edges[0].y,
        minX = edges[0].x,
        minY = edges[0].y;

      for (size_t i=1; i<4; i++) {
        maxX = std::max(maxX, edges[i].x);
        maxY = std::max(maxY, edges[i].y);
        minX = std::min(minX, edges[i].x);
        minY = std::min(minY, edges[i].y);
      }

      edges[0] = Vector2<T>(minX, minY);
      edges[1] = Vector2<T>(minX, maxY);
      edges[2] = Vector2<T>(maxX, minY);
      edges[3] = Vector2<T>(maxX, maxY);
    }

    std::vector<Vector2<T>> edges;
  };

  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(NoGreenObstacleDetector, BallDetectorIntegralImage)
};

#endif // NOGREENOBSTACLEDETECTOR_H
