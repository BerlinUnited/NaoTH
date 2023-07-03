#ifndef DETECTORFIELD_H
#define DETECTORFIELD_H

#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Motion/MotionStatus.h"

#include "Tools/CameraGeometry.h"

class DetectorImage
{
public:
  std::vector<Vector2i> edges;

  DetectorImage() : edges(4) {
    edges.resize(4);
  }

  DetectorImage(Vector2i bottomLeft, Vector2i bottomRight, Vector2i topLeft, Vector2i topRight) : edges(4) {
    edges = {bottomLeft, bottomRight, topLeft, topRight};
  }

  void rectify() {
    // rectifies detector so that edges look like
    // (0) 2 --- 3 (x)
    //      |   |
    // (y) 0 --- 1
    edges[0] = Vector2i(minX(), maxY());
    edges[1] = Vector2i(maxX(), maxY());
    edges[2] = Vector2i(minX(), minY());
    edges[3] = Vector2i(maxX(), minY());
  }

  bool limit(int maxX, int maxY, int minX, int minY) {
    rectify();

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
    int x = maxX() - minX();
    int y = maxY() - minY();

    return x*y;
  }

  int pixels() {
    int x = maxX() - minX() + 1;
    int y = maxY() - minY() + 1;

    return x*y;
  }

  int minX() {
    int minX = edges[0].x;
    for (size_t i=1; i<4; i++) {
      minX = std::min(minX, edges[i].x);
    }
    return minX;
  }

  int minY() {
    int minY = edges[0].y;
    for (size_t i=1; i<4; i++) {
      minY = std::min(minY, edges[i].y);
    }
    return minY;
  }

  int maxX() {
    int maxX = edges[0].x;
    for (size_t i=1; i<4; i++) {
      maxX = std::max(maxX, edges[i].x);
    }
    return maxX;
  }

  int maxY() {
    int maxY = edges[0].y;
    for (size_t i=1; i<4; i++) {
      maxY = std::max(maxY, edges[i].y);
    }
    return maxY;
  }

  double green_density(const BallDetectorIntegralImage& integralImage) {
    return density(integralImage, 1);
  }

  double green_density_left(const BallDetectorIntegralImage& integralImage) {
    return density_left(integralImage, 1);
  }

  double green_density_right(const BallDetectorIntegralImage& integralImage) {
    return density_right(integralImage, 1);
  }

  double density(const BallDetectorIntegralImage& integralImage, uint32_t c) {
    int minX = this->minX() / integralImage.FACTOR;
    int minY = this->minY() / integralImage.FACTOR;
    int maxX = this->maxX() / integralImage.FACTOR;
    int maxY = this->maxY() / integralImage.FACTOR;
    return integralImage.getDensityForRect(minX, minY, maxX, maxY, c);
  }

  double density_left(const BallDetectorIntegralImage& integralImage, uint32_t c) {
    int minX = this->minX() / integralImage.FACTOR;
    int minY = this->minY() / integralImage.FACTOR;
    int maxX = this->maxX() / integralImage.FACTOR;
    int maxY = this->maxY() / integralImage.FACTOR;
    int halfX = (maxX - minX) / 2;
    return integralImage.getDensityForRect(minX, minY, maxX - halfX, maxY, c);
  }

  double density_right(const BallDetectorIntegralImage& integralImage, uint32_t c) {
    int minX = this->minX() / integralImage.FACTOR;
    int minY = this->minY() / integralImage.FACTOR;
    int maxX = this->maxX() / integralImage.FACTOR;
    int maxY = this->maxY() / integralImage.FACTOR;
    int halfX = (maxX - minX) / 2;
    return integralImage.getDensityForRect(minX + halfX, minY, maxX, maxY, c);
  }
};

class DetectorField
{
public:
  std::vector<Vector2d> edges;

  DetectorField() : edges(4) {
    edges.resize(4);
  }

  // FIX: Order of indices is currently only important for debug drawings
  DetectorField(Vector2d bottomLeft, Vector2d bottomRight, Vector2d topLeft, Vector2d topRight) : edges(4) {
    edges = {bottomLeft, bottomRight, topLeft, topRight};
  }

  bool projectOnImage(DetectorImage& detectorImage, const CameraMatrix& cameraMatrix, const naoth::CameraInfo& cameraInfo) {
    for(size_t i=0; i<4; ++i) {
      Vector2d& fieldPoint = edges[i];
      Vector2i& pointInImage = detectorImage.edges[i];

      //Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x, getCameraMatrix().translation.y, 0);
      bool projectionSuccess = CameraGeometry::relativePointToImage(
            cameraMatrix, cameraInfo,
            //RotationMatrix::getRotationZ(cameraMatrix.rotation.getZAngle()) * Vector3d(fieldPoint.x, fieldPoint.y, 0),
            Vector3d(fieldPoint.x, fieldPoint.y, 0),
            pointInImage);
      if(!projectionSuccess) {
        return false;
      }
    }
    detectorImage.rectify();
    return true;
  }

  void createPreview(DetectorField& detectorField, const MotionStatus& motionStatus) {
    for(size_t i=0; i<4; i++) {
      detectorField.edges[i] = motionStatus.plannedMotion.hip * edges[i];
    }
  }

  double minX() {
    double minX = edges[0].x;
    for (size_t i=1; i<4; i++) {
      minX = std::min(minX, edges[i].x);
    }
    return minX;
  }

  double minY() {
    double minY = edges[0].y;
    for (size_t i=1; i<4; i++) {
      minY = std::min(minY, edges[i].y);
    }
    return minY;
  }

  double maxX() {
    double maxX = edges[0].x;
    for (size_t i=1; i<4; i++) {
      maxX = std::max(maxX, edges[i].x);
    }
    return maxX;
  }

  double maxY() {
    double maxY = edges[0].y;
    for (size_t i=1; i<4; i++) {
      maxY = std::max(maxY, edges[i].y);
    }
    return maxY;
  }
};

#endif // DETECTORFIELD_H
