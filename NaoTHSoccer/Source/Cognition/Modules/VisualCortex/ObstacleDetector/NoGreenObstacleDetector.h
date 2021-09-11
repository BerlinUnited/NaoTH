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
#include "Representations/Perception/ObstaclePercept.h"
#include "Representations/Motion/MotionStatus.h"
#include "Representations/Modeling/BallModel.h"

// Tools
#include "detectorfield.h"
#include "Tools/CameraGeometry.h"

BEGIN_DECLARE_MODULE(NoGreenObstacleDetector)
  REQUIRE(CameraInfo)
  //REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  //REQUIRE(CameraMatrixTop)
  REQUIRE(BallDetectorIntegralImage)
  //REQUIRE(BallDetectorIntegralImageTop)
  REQUIRE(MotionStatus)
  REQUIRE(BallModel)

  PROVIDE(VisionObstacle)
  PROVIDE(VisionObstaclePreview)
  PROVIDE(ObstacleBehindBall)

  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)
END_DECLARE_MODULE(NoGreenObstacleDetector)

class NoGreenObstacleDetector: public NoGreenObstacleDetectorBase
{
public:
  CameraInfo::CameraID cameraID;

  DetectorField detectorField;

  NoGreenObstacleDetector();
  ~NoGreenObstacleDetector();

  virtual void execute();

  void create_detector_on_field(DetectorField& detectorField, double xOffset);
  bool create_detector_behind_ball(DetectorField& detectorField);

  bool projectDetector(DetectorField& detectorField, DetectorImage& detectorImage);

private:
  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("NoGreenObstacleDetector")
    {
      PARAMETER_REGISTER(enable_obstacle_detector) = false;
      PARAMETER_REGISTER(enable_preview_obstacle_detector) = false;
      PARAMETER_REGISTER(enable_obstacle_detector_behind_ball) = true;

      PARAMETER_REGISTER(detector_field_offset) = 200.;
      PARAMETER_REGISTER(detector_width) = 280.;
      PARAMETER_REGISTER(detector_range) = 200.;

      // detector behind ball parameters
      PARAMETER_REGISTER(ball_offset) = 100.;
      PARAMETER_REGISTER(behind_ball_width) = 280.;
      PARAMETER_REGISTER(behind_ball_range) = 200.;

      PARAMETER_REGISTER(min_expected_area) = .8;

      PARAMETER_REGISTER(max_green_density) = .5;
      PARAMETER_REGISTER(min_brightness_density) = 100.;

      PARAMETER_REGISTER(check_brightness) = true;
      PARAMETER_REGISTER(check_green) = false;

      syncWithConfig();
    }
    bool enable_obstacle_detector;
    bool enable_preview_obstacle_detector;
    bool enable_obstacle_detector_behind_ball;

    double detector_field_offset;
    double detector_width;
    double detector_range;

    double ball_offset;
    double behind_ball_width;
    double behind_ball_range;

    double min_expected_area;

    // maximal green density which still is classified as an obstacle
    double max_green_density;
    // minimal brightness density to be classified as obstacle
    double min_brightness_density;

    bool check_brightness;
    bool check_green;

  } params;
};

#endif // NOGREENOBSTACLEDETECTOR_H
