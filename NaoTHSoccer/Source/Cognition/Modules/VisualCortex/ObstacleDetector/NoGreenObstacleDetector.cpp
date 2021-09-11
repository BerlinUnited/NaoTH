#include "NoGreenObstacleDetector.h"

NoGreenObstacleDetector::NoGreenObstacleDetector()
{
  DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_detector_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_detector_behind_ball", "", false);
  DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_ball_pos", "", false);
  getDebugParameterList().add(&params);
}

NoGreenObstacleDetector::~NoGreenObstacleDetector() {
  getDebugParameterList().remove(&params);
}

void NoGreenObstacleDetector::create_detector_on_field(DetectorField& detectorField,  double xOffset) {
  // create detector points on field
  // (x) x3 --- x4
  //       |   |
  // (0) x1 --- x2 (y)
  Vector2d x1(xOffset, -params.detector_width/2);
  Vector2d x2(xOffset,  params.detector_width/2);
  Vector2d x3(x1.x + params.detector_range, x1.y);
  Vector2d x4(x3.x, x2.y);
  detectorField = DetectorField(x1, x2, x3, x4);
}

bool NoGreenObstacleDetector::create_detector_behind_ball(DetectorField& detectorField) {
  if (getBallModel().valid && getBallModel().knows) {
    const Vector2d& ball_position = getBallModel().position;

    DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_ball_pos",
      FIELD_DRAWING_CONTEXT;
      PEN("FACEFF", 5);
      CIRCLE(ball_position.x, ball_position.y, 50);
    );

    Vector2d ball_direction(ball_position);
    ball_direction.normalize();

    Vector2d offset = ball_direction * params.ball_offset;

    Vector2d left(params.behind_ball_width/2, 0);
    left.rotate(ball_position.angle() - Math::pi_2);

    Vector2d x1 = (ball_position + offset) + left;
    Vector2d x2 = (ball_position + offset) - left;

    offset = offset + (ball_direction * params.behind_ball_range);

    Vector2d x3 = (ball_position + offset) + left;
    Vector2d x4 = (ball_position + offset) - left;

    detectorField.edges = {x1, x2, x3, x4};
    return true;
  }
  return false;
}

bool NoGreenObstacleDetector::projectDetector(DetectorField& detectorField, DetectorImage& detectorImage) {
  if(!detectorField.projectOnImage(detectorImage, getCameraMatrix(), getCameraInfo())) {
    return false;
  }

  int expectedArea = detectorImage.area();

  // limit detectorImage to be inside image bounds
  bool limitSuccess = detectorImage.limit((int) getCameraInfo().resolutionWidth-1, (int) getCameraInfo().resolutionHeight-1, 0, 0);
  if(!limitSuccess) {
    return false;
  }
  // check if detectorImage is still big enough
  int detectorArea = detectorImage.area();
  bool bigEnough = ((double) detectorArea / expectedArea >= params.min_expected_area);

  return bigEnough;
}


void NoGreenObstacleDetector::execute()
{
  getVisionObstacle().valid = false;
  getVisionObstaclePreview().valid = false;  
  getObstacleBehindBall().valid = false;

  bool success = false;

  if(params.enable_obstacle_detector) {
    create_detector_on_field(detectorField, params.detector_field_offset);

    DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF69B4", 5);
      BOX(detectorField.minX(), detectorField.minY(), detectorField.maxX(), detectorField.maxY());
    );

    // project detector field onto image
    DetectorImage detectorImage;
    success = projectDetector(detectorField, detectorImage);
    DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
      IMAGE_DRAWING_CONTEXT;
      if (success) {
        PEN("FF69B4", 5);
      } else {
        PEN("FF4D4D", 5);
      }
      BOX(detectorImage.minX(), detectorImage.minY(), detectorImage.maxX(), detectorImage.maxY());
    );
    if(success) {
      // calculate green density
      double green_density       = detectorImage.green_density(getBallDetectorIntegralImage());
      double green_density_left  = detectorImage.green_density_left(getBallDetectorIntegralImage());
      double green_density_right = detectorImage.green_density_right(getBallDetectorIntegralImage());

      DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
        FIELD_DRAWING_CONTEXT;
        PEN("FF69B4", 5);

        TEXT_DRAWING(1000,    0, green_density       * 100 << '%');
        TEXT_DRAWING(1000, 1000, green_density_left  * 100 << '%');
        TEXT_DRAWING(1000,-1000, green_density_right * 100 << '%');
      );

      // set representation if detector is occupied
      getVisionObstacle().valid = true;
      getVisionObstacle().isOccupied = green_density <= params.max_green_density;
      if (getVisionObstacle().isOccupied) {
        getVisionObstacle().onTheRight = green_density_right <= green_density_left;
        getVisionObstacle().onTheLeft = green_density_right > green_density_left;
      } else {
        getVisionObstacle().onTheRight = false;
        getVisionObstacle().onTheLeft = false;
      }
    }
  }

  // preview
  if(params.enable_preview_obstacle_detector) {
    DetectorField previewDetectorField;
    detectorField.createPreview(previewDetectorField, getMotionStatus());

    DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
      FIELD_DRAWING_CONTEXT;
      PEN("0080FF", 5);
      LINE(previewDetectorField.edges[0].x, previewDetectorField.edges[0].y, previewDetectorField.edges[1].x, previewDetectorField.edges[1].y);
      LINE(previewDetectorField.edges[1].x, previewDetectorField.edges[1].y, previewDetectorField.edges[3].x, previewDetectorField.edges[3].y);
      LINE(previewDetectorField.edges[2].x, previewDetectorField.edges[2].y, previewDetectorField.edges[3].x, previewDetectorField.edges[3].y);
      LINE(previewDetectorField.edges[0].x, previewDetectorField.edges[0].y, previewDetectorField.edges[2].x, previewDetectorField.edges[2].y);
    );

    DetectorImage previewDetectorImage;
    success = projectDetector(previewDetectorField, previewDetectorImage);
    DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
      IMAGE_DRAWING_CONTEXT;
      if (success) {
        PEN("FF69B4", 5);
      } else {
        PEN("FF4D4D", 5);
      }
      BOX(previewDetectorImage.minX(), previewDetectorImage.minY(), previewDetectorImage.maxX(), previewDetectorImage.maxY());
    );
    if(success) {
      // calculate green density
      double green_density = previewDetectorImage.green_density(getBallDetectorIntegralImage());
      double green_density_left = previewDetectorImage.green_density_left(getBallDetectorIntegralImage());
      double green_density_right = previewDetectorImage.green_density_right(getBallDetectorIntegralImage());

      // set representation if detector is occupied
      getVisionObstaclePreview().valid = true;
      getVisionObstaclePreview().isOccupied = green_density <= params.max_green_density;
      if (getVisionObstaclePreview().isOccupied) {
        getVisionObstaclePreview().onTheRight = green_density_right <= green_density_left;
        getVisionObstaclePreview().onTheLeft = green_density_right > green_density_left;
      } else {
        getVisionObstaclePreview().onTheRight = false;
        getVisionObstaclePreview().onTheLeft = false;
      }
    }
  }

  if(params.enable_obstacle_detector_behind_ball) {
    DetectorField detectorBehindBall;
    success = create_detector_behind_ball(detectorBehindBall);
    if(success) {
      DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_behind_ball",
        FIELD_DRAWING_CONTEXT;
        PEN("0080FF", 5);
        DetectorField& detector = detectorBehindBall;
        LINE(detector.edges[0].x, detector.edges[0].y, detector.edges[1].x, detector.edges[1].y);
        LINE(detector.edges[1].x, detector.edges[1].y, detector.edges[3].x, detector.edges[3].y);
        LINE(detector.edges[2].x, detector.edges[2].y, detector.edges[3].x, detector.edges[3].y);
        LINE(detector.edges[0].x, detector.edges[0].y, detector.edges[2].x, detector.edges[2].y);
      );

      DetectorImage detectorImageBehindBall;
      success = projectDetector(detectorBehindBall, detectorImageBehindBall);
      DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_behind_ball",
        IMAGE_DRAWING_CONTEXT;
        if (!success) {
          PEN("FF4D4D", 5);
          BOX(detectorImageBehindBall.minX(), detectorImageBehindBall.minY(),
              detectorImageBehindBall.maxX(), detectorImageBehindBall.maxY());
        }
      );
      if(success) {
        if(params.check_green) {
          // calculate green density
          double green_density = detectorImageBehindBall.green_density(getBallDetectorIntegralImage());
          double green_density_left = detectorImageBehindBall.green_density_left(getBallDetectorIntegralImage());
          double green_density_right = detectorImageBehindBall.green_density_right(getBallDetectorIntegralImage());

          DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_behind_ball",
            FIELD_DRAWING_CONTEXT;
            DetectorField& detector = detectorBehindBall;

            PEN("0080FF", 5);
            TEXT_DRAWING(detector.edges[0].x, detector.edges[0].y       , green_density * 100 << '%');
            TEXT_DRAWING(detector.edges[0].x, detector.edges[0].y + 1000, green_density_left * 100 << '%');
            TEXT_DRAWING(detector.edges[0].x, detector.edges[0].y - 1000, green_density_right * 100 << '%');
          );

          // set representation if detector is occupied
          getObstacleBehindBall().valid = true;
          getObstacleBehindBall().isOccupied = green_density <= params.max_green_density;
          if (getObstacleBehindBall().isOccupied) {
            getObstacleBehindBall().onTheRight = green_density_right <= green_density_left;
            getObstacleBehindBall().onTheLeft = green_density_right > green_density_left;
          } else {
            getObstacleBehindBall().onTheRight = false;
            getObstacleBehindBall().onTheLeft = false;
          }
        }

        if(params.check_brightness) {
          // calculate brightness density
          double density = detectorImageBehindBall.density(getBallDetectorIntegralImage(), 0);
          double density_left = detectorImageBehindBall.density_left(getBallDetectorIntegralImage(), 0);
          double density_right = detectorImageBehindBall.density_right(getBallDetectorIntegralImage(), 0);

          DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_behind_ball",
            FIELD_DRAWING_CONTEXT;
            DetectorField& detector = detectorBehindBall;

            PEN("FFBFF8", 5);
            TEXT_DRAWING(detector.edges[0].x + 2000, detector.edges[0].y       , density);
            TEXT_DRAWING(detector.edges[0].x + 2000, detector.edges[0].y + 1000, density_left);
            TEXT_DRAWING(detector.edges[0].x + 2000, detector.edges[0].y - 1000, density_right);
          );

          // set representation if detector is occupied
          getObstacleBehindBall().valid = true;
          getObstacleBehindBall().isOccupied = density >= params.min_brightness_density;
          if (getObstacleBehindBall().isOccupied) {
            getObstacleBehindBall().onTheRight = density_right >= density_left;
            getObstacleBehindBall().onTheLeft = density_right < density_left;
          } else {
            getObstacleBehindBall().onTheRight = false;
            getObstacleBehindBall().onTheLeft = false;
          }
        }

        DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_behind_ball",
          IMAGE_DRAWING_CONTEXT;
          if(getObstacleBehindBall().valid) {
            if(getObstacleBehindBall().isOccupied) {
              PEN("FF4D4D", 5);
            } else {
              PEN("AAFCC7", 5);
            }

            BOX(detectorImageBehindBall.minX(), detectorImageBehindBall.minY(),
                detectorImageBehindBall.maxX(), detectorImageBehindBall.maxY());

            int halfX = (detectorImageBehindBall.maxX() - detectorImageBehindBall.minX()) / 2;

            if(getObstacleBehindBall().onTheRight) {
              PEN("FF4D4D", 3);
            } else {
              PEN("AAFCC7", 3);
            }
            BOX(detectorImageBehindBall.minX()+1 + halfX, detectorImageBehindBall.minY()+1,
                detectorImageBehindBall.maxX()-1, detectorImageBehindBall.maxY()-1);

            if(getObstacleBehindBall().onTheLeft) {
              PEN("FF4D4D", 3);
            } else {
              PEN("AAFCC7", 3);
            }
            BOX(detectorImageBehindBall.minX()+1, detectorImageBehindBall.minY()+1,
                detectorImageBehindBall.maxX()-1 - halfX, detectorImageBehindBall.maxY()-1);
          }
        );
      }
    }
  } // end if params.enable_obstacle_detector_behind_ball
} // end execute
