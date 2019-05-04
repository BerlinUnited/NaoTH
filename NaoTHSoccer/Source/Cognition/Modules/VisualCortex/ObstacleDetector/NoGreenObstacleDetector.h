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
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BallModel.h"

// Tools
#include "detectorfield.h"
#include "Tools/CameraGeometry.h"

BEGIN_DECLARE_MODULE(NoGreenObstacleDetector)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)
  REQUIRE(MotionStatus)
  REQUIRE(BallModel)

  PROVIDE(VisionObstacle)
  PROVIDE(VisionObstaclePreview)

  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
END_DECLARE_MODULE(NoGreenObstacleDetector)

class NoGreenObstacleDetector: public NoGreenObstacleDetectorBase
{
public:
  double detector_field_offset;
  double detector_width;
  double detector_range;
  double ball_offset;

  CameraInfo::CameraID cameraID;

  DetectorField detectorField;

  NoGreenObstacleDetector();
  ~NoGreenObstacleDetector();

  virtual void execute();

  void create_detector_on_field(DetectorField& detectorField, double xOffset);
  bool create_detector_behind_ball(DetectorField& detectorField);

  bool detector_parameters_changed();

  bool projectDetector(DetectorField& detectorField, DetectorImage& detectorImage);

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("NoGreenObstacleDetector")
    {
      PARAMETER_REGISTER(detector_field_offset) = 200.;
      PARAMETER_REGISTER(detector_width) = 280.;
      PARAMETER_REGISTER(detector_range) = 200.;

      PARAMETER_REGISTER(ball_offset) = 10.;

      PARAMETER_REGISTER(min_expected_area) = .8;

      PARAMETER_REGISTER(max_green_density) = .5;

      syncWithConfig();
    }
    double detector_field_offset;
    double detector_width;
    double detector_range;

    double ball_offset;

    double min_expected_area;

    // maximal green density witch still is classified as an obstacle
    double max_green_density;
  } params;

  DOUBLE_CAM_REQUIRE(NoGreenObstacleDetector, BallDetectorIntegralImage)
  DOUBLE_CAM_REQUIRE(NoGreenObstacleDetector, CameraInfo)
};

#endif // NOGREENOBSTACLEDETECTOR_H
