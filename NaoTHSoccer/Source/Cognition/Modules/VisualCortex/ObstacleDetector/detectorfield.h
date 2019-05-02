#ifndef DETECTORFIELD_H
#define DETECTORFIELD_H

#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"

#include "Tools/CameraGeometry.h"

class DetectorImage
{
public:
  std::vector<Vector2i> edges;

  DetectorImage() : edges(4) {
    // indices are expected to look like
    // (0) 2 --- 3 (x)
    //      |   |
    // (y) 0 --- 1
    edges.resize(4);
  }

  DetectorImage(Vector2i bottomLeft, Vector2i bottomRight, Vector2i topLeft, Vector2i topRight) : edges(4) {
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
};

class DetectorField
{
public:
  std::vector<Vector2d> edges;

  DetectorField() : edges(4) {
    // indices are expected to look like
    // (x) 2 --- 3
    //      |   |
    // (0) 0 --- 1 (y)
    edges.resize(4);
  }

  bool projectOnImage(DetectorImage& detectorImage, const CameraMatrix& cameraMatrix, const naoth::CameraInfo& cameraInfo) {
    for(size_t i=0; i<4; ++i) {
      Vector2d& fieldPoint = edges[i];
      Vector2i& pointInImage = detectorImage.edges[i];

      //Vector3d cameraMatrixOffset = Vector3d(getCameraMatrix().translation.x, getCameraMatrix().translation.y, 0);
      bool projectionSuccess = CameraGeometry::relativePointToImage(
            cameraMatrix, cameraInfo,
            RotationMatrix::getRotationZ(cameraMatrix.rotation.getZAngle()) * Vector3d(fieldPoint.x, fieldPoint.y, 0),
            pointInImage);
      if(!projectionSuccess) {
        return false;
      }
    }
    detectorImage.rectify();
    return true;
  }

  double minX() {
    return edges[0].x;
  }

  double minY() {
    return edges[0].y;
  }

  double maxX() {
    return edges[3].x;
  }

  double maxY() {
    return edges[3].y;
  }

};

#endif // DETECTORFIELD_H
