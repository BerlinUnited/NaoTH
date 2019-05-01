#include "NoGreenObstacleDetector.h"

NoGreenObstacleDetector::NoGreenObstacleDetector() {
  DEBUG_REQUEST_REGISTER("Vision:NoGreenObstacleDetector:draw_detector_field", "", false);

  getDebugParameterList().add(&params);
}

NoGreenObstacleDetector::~NoGreenObstacleDetector() {
  getDebugParameterList().remove(&params);
}

void NoGreenObstacleDetector::execute()
{
  // create detector points on field
  // (x) x3 --- x4
  //       |   |
  // (0) x1 --- x2 (y)
  std::vector<Vector2d> edges(4);
  Vector2d x1(params.detector_field_offset, -params.detector_width/2);
  Vector2d x2(params.detector_field_offset,  params.detector_width/2);
  Vector2d x3(x1.x + params.detector_range, x1.y);
  Vector2d x4(x3.x, x2.y);
  edges = {x1, x2, x3, x4};

  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF69B4", 5);
    BOX(x1.x, x1.y, x4.x, x4.y);
  );

  // project field points onto image so that
  // (0) 2 --- 3 (x)
  //      |   |
  // (y) 0 --- 1
  DetectorField detectorImage;

  for(size_t i=0; i<4; ++i) {
    Vector2d& fieldPoint = edges[i];
    Vector2i& pointInImage = detectorImage.edges[i];

    //Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x, getCameraMatrix().translation.y, 0);
    bool projectionSuccess = CameraGeometry::relativePointToImage(
          getCameraMatrix(), getCameraInfo(),
          RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle()) * Vector3d(fieldPoint.x, fieldPoint.y, 0),
          pointInImage);
    if(!projectionSuccess) {
      return;
    }
  }

  detectorImage.rectify();

  int expectedArea = detectorImage.area();

  bool limitSuccess = detectorImage.limit((int) getCameraInfo().resolutionWidth-1, (int) getCameraInfo().resolutionHeight-1, 0, 0);
  if(!limitSuccess) {
    return;
  }

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

  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF69B4", 5);

    std::ostringstream stringStream;
    stringStream << (double) detectorImage.green(getBallDetectorIntegralImage()) / detectorImage.pixels() * 100 << '%';
    TEXT_DRAWING(1000, 1000, stringStream.str());
  );
}
