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
  // create detector field
  Vector2d x1(params.detector_field_offset, -params.detector_width/2);
  Vector2d x2(params.detector_field_offset, params.detector_width/2);
  Vector2d x3(x1.x + params.detector_range, x1.y);
  Vector2d x4(x3.x, x2.y);

  DetectorField<double> detectorField(x1, x2, x3, x4);

  // project detector field onto image
  DetectorField<int> detectorImage;

  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    FIELD_DRAWING_CONTEXT;
    PEN("FF69B4", 5);

    BOX(x1.x, x1.y, x4.x, x4.y);
  );

  for(size_t i=0; i<4; ++i) {
    Vector2d& fieldPoint = detectorField.edges[i];
    Vector2i& pointInImage = detectorImage.edges[i];

    //Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x, getCameraMatrix().translation.y, 0);
    bool projectionSuccess = CameraGeometry::relativePointToImage(
          getCameraMatrix(), getCameraInfo(),
          RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle()) * Vector3d(fieldPoint.x, fieldPoint.y, 0),
          pointInImage);
    if(!projectionSuccess)
    {
      return;
    }
  }

  detectorImage.squarify();

  DEBUG_REQUEST("Vision:NoGreenObstacleDetector:draw_detector_field",
    IMAGE_DRAWING_CONTEXT;
    PEN("FF69B4", 5);

    BOX(detectorImage.edges[0].x, detectorImage.edges[0].y, detectorImage.edges[3].x, detectorImage.edges[3].y);
  );



  //Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x, getCameraMatrix().translation.y, 0);
  /*
  bool projectionSuccess = CameraGeometry::relativePointToImage(
        getCameraMatrix(), getCameraInfo(),
        RotationMatrix::getRotationZ(getCameraMatrix().rotation.getZAngle()) * Vector3d(1, 0, 0),
        pointInImage);

  if(projectionSuccess)
  {

  }*/
}
