#include "NoGreenObstacleDetector.h"

NoGreenObstacleDetector::NoGreenObstacleDetector():
  detector_field_offset(params.detector_field_offset),
  detector_width(params.detector_width),
  detector_range(params.detector_range),
  ball_offset(params.ball_offset)
{
  //DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_detector_field", "", false);
  DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_detector_behind_ball", "", false);
  DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_ball_pos", "", false);
  getDebugParameterList().add(&params);

  create_detector_on_field(detectorField, detector_field_offset);
}

NoGreenObstacleDetector::~NoGreenObstacleDetector() {
  getDebugParameterList().remove(&params);
}

bool NoGreenObstacleDetector::detector_parameters_changed() {
  bool changend = false;
  if (detector_field_offset != params.detector_field_offset) {
    detector_field_offset = params.detector_field_offset;
    changend = true;
  }
  if (detector_width != params.detector_width) {
    detector_width = params.detector_width;
    changend = true;
  }
  if (detector_range != params.detector_range) {
    detector_range = params.detector_range;
    changend = true;
  }
  if (ball_offset != params.ball_offset) {
    ball_offset = params.ball_offset;
    changend = true;
  }
  return changend;
}

void NoGreenObstacleDetector::create_detector_on_field(DetectorField& detectorField,  double xOffset) {
  // create detector points on field
  // (x) x3 --- x4
  //       |   |
  // (0) x1 --- x2 (y)
  Vector2d x1(xOffset, -detector_width/2);
  Vector2d x2(xOffset,  detector_width/2);
  Vector2d x3(x1.x + detector_range, x1.y);
  Vector2d x4(x3.x, x2.y);
  detectorField.edges = {x1, x2, x3, x4};
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

    Vector2d offset = ball_direction * ball_offset;

    Vector2d left(detector_width/2, 0);
    left.rotate(ball_position.angle() - Math::pi_2);

    Vector2d x1 = (ball_position + offset) + left;
    Vector2d x2 = (ball_position + offset) - left;

    offset = offset + (ball_direction * detector_range);

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
  /*
  getVisionObstacle().valid = false;
  getVisionObstaclePreview().valid = false;  

  if(detector_parameters_changed()) {
    create_detector_on_field(detectorField, detector_field_offset);
  }

  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF69B4", 5);
    BOX(detectorField.minX(), detectorField.minY(), detectorField.maxX(), detectorField.maxY());
  );

  // project detector field onto image
  DetectorImage detectorImage;
  bool succes = projectDetector(detectorField, detectorImage);
  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    IMAGE_DRAWING_CONTEXT;
    if (succes) {
      PEN("FF69B4", 5);
    } else {
      PEN("FF4D4D", 5);
    }
    BOX(detectorImage.minX(), detectorImage.minY(), detectorImage.maxX(), detectorImage.maxY());
  );
  if(succes) {
    // calculate green density
    double green_density = (double) detectorImage.green(getBallDetectorIntegralImage()) / detectorImage.pixels();
    double green_density_left = (double) detectorImage.greenLeft(getBallDetectorIntegralImage()) / (detectorImage.pixels()/2);
    double green_density_right = (double) detectorImage.greenRight(getBallDetectorIntegralImage()) / (detectorImage.pixels()/2);

    DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
      FIELD_DRAWING_CONTEXT;
      PEN("FF69B4", 5);

      std::ostringstream stringStream;
      stringStream << green_density * 100 << '%';
      TEXT_DRAWING(1000, 0, stringStream.str());
      stringStream.str("");

      stringStream << green_density_left * 100 << '%';
      TEXT_DRAWING(1000, -1000, stringStream.str());
      stringStream.str("");

      stringStream << green_density_right * 100 << '%';
      TEXT_DRAWING(1000, 1000, stringStream.str());
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

  // preview
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
  succes = projectDetector(previewDetectorField, previewDetectorImage);
  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    IMAGE_DRAWING_CONTEXT;
    if (succes) {
      PEN("FF69B4", 5);
    } else {
      PEN("FF4D4D", 5);
    }
    BOX(previewDetectorImage.minX(), previewDetectorImage.minY(), previewDetectorImage.maxX(), previewDetectorImage.maxY());
  );
  if(succes) {    
    // calculate green density
    double green_density = (double) previewDetectorImage.green(getBallDetectorIntegralImage()) / previewDetectorImage.pixels();
    double green_density_left = (double) previewDetectorImage.greenLeft(getBallDetectorIntegralImage()) / (previewDetectorImage.pixels()/2);
    double green_density_right = (double) previewDetectorImage.greenRight(getBallDetectorIntegralImage()) / (previewDetectorImage.pixels()/2);

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
  */

  getObstacleBehindBall().valid = false;
  detector_parameters_changed();

  DetectorField detectorBehindBall;
  bool succes = create_detector_behind_ball(detectorBehindBall);
  if(succes) {
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
    succes = projectDetector(detectorBehindBall, detectorImageBehindBall);
    DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_behind_ball",
      IMAGE_DRAWING_CONTEXT;
      if (succes) {
        PEN("FF69B4", 5);
      } else {
        PEN("FF4D4D", 5);
      }
      BOX(detectorImageBehindBall.minX(), detectorImageBehindBall.minY(),
          detectorImageBehindBall.maxX(), detectorImageBehindBall.maxY());
    );
    if(succes) {
      // calculate green density
      double green_density = (double)
          detectorImageBehindBall.green(getBallDetectorIntegralImage()) /
          detectorImageBehindBall.pixels();
      double green_density_left = (double)
          detectorImageBehindBall.greenLeft(getBallDetectorIntegralImage()) /
          (detectorImageBehindBall.pixels()/2);
      double green_density_right = (double)
          detectorImageBehindBall.greenRight(getBallDetectorIntegralImage()) /
          (detectorImageBehindBall.pixels()/2);

      DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_behind_ball",
        FIELD_DRAWING_CONTEXT;
        DetectorField& detector = detectorBehindBall;

        PEN("0080FF", 5);
        std::ostringstream stringStream;
        stringStream << green_density * 100 << '%';
        TEXT_DRAWING(detector.edges[0].x, detector.edges[0].y, stringStream.str());
        stringStream.str("");

        stringStream << green_density_left * 100 << '%';
        TEXT_DRAWING(detector.edges[0].x, detector.edges[0].y + 1000, stringStream.str());
        stringStream.str("");

        stringStream << green_density_right * 100 << '%';
        TEXT_DRAWING(detector.edges[0].x, detector.edges[0].y - 1000, stringStream.str());
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
  }
}
