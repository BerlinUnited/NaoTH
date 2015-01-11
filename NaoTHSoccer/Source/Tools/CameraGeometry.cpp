/**
* @file CameraGeometry.cpp
*
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
*
* Implementation of the class CameraGeometry
*/

#include "CameraGeometry.h"

using namespace naoth;

Vector3<double> CameraGeometry::imagePixelToCameraCoords(const CameraMatrix& cameraMatrix,
                                                   const CameraInfo& cameraInfo,
                                                   const double imgX,
                                                   const double imgY)
{
  Vector3d pixelVector;

  pixelVector.x = cameraInfo.getFocalLength();
  pixelVector.y = -imgX + cameraInfo.getOpticalCenterX();
  pixelVector.z = -imgY + cameraInfo.getOpticalCenterY();

  pixelVector = cameraMatrix.rotation * pixelVector;
  return pixelVector;
}//end imagePixelCameraCoords


bool CameraGeometry::relativePointToImage( 
  const CameraMatrix& cameraMatrix,
  const CameraInfo& cameraInfo,
  const Vector3d& point,
  Vector2d& pointInImage)
{
  const static double epsilon = 1e-13;

  // vector: O ---> point (in camera coordinates)
  Vector3d vectorToPoint = cameraMatrix.rotation.invert() * (point - cameraMatrix.translation);

  // the point is behind the camera plane
  if(vectorToPoint.x <= epsilon) {
    return false;
  }

  double factor = cameraInfo.getFocalLength() / vectorToPoint.x;
  pointInImage.x = -(vectorToPoint.y * factor) + 0.5 + cameraInfo.getOpticalCenterX();
  pointInImage.y = -(vectorToPoint.z * factor) + 0.5 + cameraInfo.getOpticalCenterY();

  return true;
}//end relativePointToImageDouble

bool CameraGeometry::relativePointToImage( 
  const CameraMatrix& cameraMatrix,
  const CameraInfo& cameraInfo,
  const Vector3d& point,
  Vector2i& pointInImage)
{
  Vector2d pointInImageDouble;
  if(relativePointToImage(cameraMatrix,cameraInfo,point,pointInImageDouble)) {
    pointInImage.x = (int)(pointInImageDouble.x + 0.5);
    pointInImage.y = (int)(pointInImageDouble.y + 0.5);
    return true;
  }

  return false;
}//end relativePointToImage


Vector2<double> CameraGeometry::angleToPointInImage( const CameraMatrix& cameraMatrix,
                                               const CameraInfo& cameraInfo,
                                               const double imgX,
                                               const double imgY)
{
  Vector3<double> direction = imagePixelToCameraCoords(cameraMatrix, cameraInfo, imgX, imgY);
  direction.normalize();
  return Vector2<double>(
      atan2(direction.y, direction.x), // angle horizontal
      atan2(direction.z, direction.x) // angle vertical
      );
}//end angleToPointInImage


bool CameraGeometry::imagePixelToFieldCoord( const CameraMatrix& cameraMatrix, 
                                       const CameraInfo& cameraInfo,
                                       const Vector2<double>& imagePoint, 
                                       const double objectHeight,
                                       Vector2<double>& result)
{
  return imagePixelToFieldCoord(cameraMatrix, cameraInfo, imagePoint.x, imagePoint.y, objectHeight, result);
}

bool CameraGeometry::imagePixelToFieldCoord( const CameraMatrix& cameraMatrix, 
                                       const CameraInfo& cameraInfo,
                                       const double& imgX, 
                                       const double& imgY, 
                                       const double& objectHeight,
                                       Vector2<double>& result)
{
  const static double epsilon = 1e-13;

  Vector3<double> pixelVector = imagePixelToCameraCoords(cameraMatrix, cameraInfo, imgX, imgY);
  
  double heightDiff = objectHeight-cameraMatrix.translation.z;


  // projection is impossible, because
  //  - the height of the object is the same as the height of the camera
  //    (it should never be pixelVector.z == 0 or heightDiff == 0)
  //  - the point in image lies over the horizont, but the height of the 
  //    object is smaller as the height of the camera or vice versa 
  //    (it should allways hold sign(pixelVector.z) == sign(heightDiff))
  if (pixelVector.z*heightDiff < epsilon)
  {
    return false;
  }

  result.x = pixelVector.x;
  result.y = pixelVector.y;

  result *= heightDiff/pixelVector.z;

  result.x += cameraMatrix.translation.x;
  result.y += cameraMatrix.translation.y;    
  
  return true;
}//end imagePixelToFieldCoord


Vector3<double> CameraGeometry::imagePixelToWorld(const CameraMatrix& cameraMatrix,
                                            const CameraInfo& cameraInfo,
                                            const double imgX,
                                            const double imgY,
                                            const double distance)
{
  Vector3<double> w = imagePixelToCameraCoords(cameraMatrix, cameraInfo, imgX, imgY);
  return cameraMatrix.translation + w.normalize(distance);
}//end imagePixelToWorld

void CameraGeometry::calculateArtificialHorizon( const Pose3D& cameraMatrix,
                                           const CameraInfo& cameraInfo,
                                           Vector2<double>& p1,
                                           Vector2<double>& p2 )
{
  double r31 = cameraMatrix.rotation.c[0].z;
  double r32 = cameraMatrix.rotation.c[1].z;
  double r33 = cameraMatrix.rotation.c[2].z;
  
  if(r33 == 0) 
    r33 = 1e-5;
  
  double x1 = 0,
         x2 = cameraInfo.resolutionWidth - 1,

         v1 = cameraInfo.getFocalLength(),
         v2 = cameraInfo.getOpticalCenterX(),
         v3 = cameraInfo.getOpticalCenterY(),

         y1 = (v3 * r33 + r31 * v1 + r32 * v2) / r33,
         y2 = (v3 * r33 + r31 * v1 - r32 * v2) / r33;
  
  // Mirror ends of horizon if Camera rotated to the left  
  if((cameraMatrix.rotation * Vector3<double>(0,0,1)).z < 0)
  {
    double t = x1;
    x1 = x2;
    x2 = t;
    t = y1;
    y1 = y2;
    y2 = t;
  }//end if

  p1.x = x1;
  p1.y = y1;
  p2.x = x2;
  p2.y = y2;
}//end calculateArtificialHorizone


Vector2<double> CameraGeometry::lookAtPoint(const Vector3<double>& point, double cameraHeight)
{
  Vector3<double> vector;
  Vector2<double> result;
  double pitch;
  double yaw;
  vector.x = point.x;
  vector.y = point.y;
  //camera height
  vector.z = point.z-cameraHeight;

  yaw = atan2(vector.y,vector.x);
  pitch = -atan2(vector.z,sqrt(vector.y*vector.y + vector.x*vector.x));

  result.x = yaw;
  result.y = pitch;

  return result;
}//end lookAtPoint


Pose3D CameraGeometry::calculateCameraMatrix(
    const KinematicChain& theKinematicChain,
    const Vector3d& translationOffset,
    double rotationOffsetY,
    const Vector2d& theCameraCorrectionOffset
  )
{
  // get the pose of the head
  Pose3D pose(theKinematicChain.theLinks[KinematicChain::Head].M);

  // transformation from the head to the camera with correction offset
  pose.translate(translationOffset);
  
  pose.rotateY(theCameraCorrectionOffset.y + rotationOffsetY) // tilt
      .rotateX(theCameraCorrectionOffset.x); // roll

  return pose;
}//end calculateCameraMatrix