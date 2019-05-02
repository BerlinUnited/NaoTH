#include "NoGreenObstacleDetector.h"

NoGreenObstacleDetector::NoGreenObstacleDetector():
  detector_field_offset(params.detector_field_offset),
  detector_width(params.detector_width),
  detector_range(params.detector_range)
{
  DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_detector_field", "", false);
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

void NoGreenObstacleDetector::execute()
{
  getVisionObstacle().valid = false;

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
  if(!detectorField.projectOnImage(detectorImage, getCameraMatrix(), getCameraInfo())) {
    return;
  }

  int expectedArea = detectorImage.area();

  // limit detectorImage to be inside image bounds
  bool limitSuccess = detectorImage.limit((int) getCameraInfo().resolutionWidth-1, (int) getCameraInfo().resolutionHeight-1, 0, 0);
  if(!limitSuccess) {
    return;
  }
  // check if detectorImage is still big enough
  int detectorArea = detectorImage.area();
  bool bigEnough = (double) detectorArea / expectedArea >= params.min_expected_area;

  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    IMAGE_DRAWING_CONTEXT;
    if (bigEnough) {
      PEN("FF69B4", 5);
    } else {
      PEN("FF4D4D", 5);
    }
    BOX(detectorImage.minX(), detectorImage.minY(), detectorImage.maxX(), detectorImage.maxY());
  );

  if (!bigEnough) {
    return;
  }

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
  getVisionObstacle().inFront = green_density <= params.max_green_density;
  if (getVisionObstacle().inFront) {
    getVisionObstacle().onTheRight = green_density_right <= green_density_left;
    getVisionObstacle().onTheLeft = green_density_right > green_density_left;
  } else {
    getVisionObstacle().onTheRight = false;
    getVisionObstacle().onTheLeft = false;
  }
}
