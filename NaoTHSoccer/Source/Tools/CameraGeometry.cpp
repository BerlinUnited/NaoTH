/**
* @file CameraGeometry.cpp
*
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
*
* Implementation of the class CameraGeometry
*/

#include "CameraGeometry.h"

using namespace naoth;

/*
 * NOTE: we are using the "natural embedding" of pixel inices into the 
 *       continuous image plane. I.e., a pixel with indices (x,y)
 *       lies also at the cordinates (x,y) in the contunuous plane.
 *       This means, that the origin of the CIP lies in the center of the 
 *       pixel (0,0), and the upper left corner of the image has coordinates 
 *       (-0.5, -0.5). The center of the CIP is located at (width/2-0.5, height/2-0.5)
 *
 * Visualization of used Coordinate-Frames and their relation:
 *                     z-cam
 *                     ^
 *            |        |                        Legend:
 *   x-idx--> 0 1 2 3 4|5 6 7 8 9                 ?-cip: Continuous Image Plane
 *           -|--------|----------                ?-idx: Image Pixel Index
 *        --0|¤-¤-¤-¤-¤-¤-¤-¤-¤-¤|--> x-cip              (sometimes also named
 *           ||        |         |                        Discrete Image Plane)
 * y-idx--> 1|¤ ¤ ¤ ¤ ¤|¤ ¤ ¤ ¤ ¤|                ?-cam: Camera Frame Axis
 *           ||        |         |
 *          2|¤ ¤ ¤ ¤ ¤|¤ ¤ ¤ ¤ ¤|              Note:
 * y-cam <---||--------|---------|--              - origin of CIP coincides with
 *          3|¤ ¤ ¤ ¤ ¤|¤ ¤ ¤ ¤ ¤|                  the center of pixel (0, 0)
 *           ||        |         |                - x-cam points into the image
 *          4|¤ ¤ ¤ ¤ ¤|¤ ¤ ¤ ¤ ¤<-pixel            plane (i.e. from your point
 *           ||        |         |                  of view into your monitor)
 *          5|¤ ¤ ¤ ¤ ¤|¤ ¤ ¤ ¤ ¤|<-image plane
 *           -|--------|----------
 *            |        |
 *            v
 *            y-cip
 *
 *  direction of view   z-cam
 *  of the image above  |                                         oooo
 *     ----------->     |                  |                    oooooooo
 *                      |                  |<- image plane     ooObjectoo
 *  projection center-> *-------------------------> x-cam        oooooo
 *                      |<- focal length ->|                       ll
 *                      |                  |                       ll
 *                      |                  y-cip                  dllb
 *
 */

Vector3d CameraGeometry::imagePixelToCameraCoords( const CameraInfo& cameraInfo,
                                                   const double imgX,
                                                   const double imgY)
{
  // NOTE: the origin of the Continuous Image Plane (CIP) lies in the center of the pixel (0,0) 
  //       thus the opical center in the CIP coordinates lies at (width/2-0.5, height/2-0.5)
  Vector3d pixelVector;
  pixelVector.x =  cameraInfo.getFocalLength();
  pixelVector.y = (cameraInfo.getOpticalCenterX() - 0.5) - imgX;
  pixelVector.z = (cameraInfo.getOpticalCenterY() - 0.5) - imgY;
  return pixelVector;
}

Vector2d CameraGeometry::relativePointToCameraAngle( 
  const CameraMatrix& cameraMatrix,
  const CameraInfo& /*cameraInfo*/,
  const Vector3d& point)
{
  // vector: O ---> point (in camera coordinates)
  Vector3d vectorToPoint = cameraMatrix.invert()*point;

  return Vector2d(
      atan2(vectorToPoint.y, vectorToPoint.x), // angle horizontal

      // TODO: maybe it should be
      //atan2(vectorToPoint.z, std::hypot(vectorToPoint.x, vectorToPoint.y) )
      atan2(vectorToPoint.z, vectorToPoint.x) // angle vertical
      );
}

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

  // NOTE: the origin of the Continuous Image Plane (CIP) lies in the center of the pixel (0,0) 
  //       thus the opical center in the CIP coordinates lies at (width/2-0.5, height/2-0.5)
  pointInImage.x = -(vectorToPoint.y * factor) + (cameraInfo.getOpticalCenterX() - 0.5);
  pointInImage.y = -(vectorToPoint.z * factor) + (cameraInfo.getOpticalCenterY() - 0.5);

  return true;
}

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


Vector2d CameraGeometry::pixelToAngles( const naoth::CameraInfo& cameraInfo,
                                        const double imgX,
                                        const double imgY)
{
  Vector3d pixelVector = imagePixelToCameraCoords(cameraInfo, imgX, imgY);

  return Vector2d(
      atan2(pixelVector.y, pixelVector.x), // angle horizontal
      atan2(pixelVector.z, pixelVector.x) // angle vertical
      );
}


bool CameraGeometry::imagePixelToFieldCoord( const CameraMatrix& cameraMatrix, 
                                       const CameraInfo& cameraInfo,
                                       const Vector2d& imagePoint, 
                                       const double objectHeight,
                                       Vector2d& result)
{
  return imagePixelToFieldCoord(cameraMatrix, cameraInfo, imagePoint.x, imagePoint.y, objectHeight, result);
}

bool CameraGeometry::imagePixelToFieldCoord( const CameraMatrix& cameraMatrix, 
                                       const CameraInfo& cameraInfo,
                                       const double imgX, 
                                       const double imgY, 
                                       const double objectHeight,
                                       Vector2d& result)
{
  const static double epsilon = 1e-13;

  // point --> head coordinates
  Vector3d pixelVector = cameraMatrix.rotation * imagePixelToCameraCoords(cameraInfo, imgX, imgY);
  
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


Vector3d CameraGeometry::imagePixelToWorld(const CameraMatrix& cameraMatrix,
                                            const CameraInfo& cameraInfo,
                                            const double imgX,
                                            const double imgY,
                                            const double distance)
{
  // point --> head coordinates
  Vector3d w = cameraMatrix.rotation*imagePixelToCameraCoords(cameraInfo, imgX, imgY);
  return cameraMatrix.translation + w.normalize(distance);
}

void CameraGeometry::calculateArtificialHorizon( const Pose3D& cameraMatrix,
                                           const CameraInfo& cameraInfo,
                                           Vector2d& p1,
                                           Vector2d& p2 )
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
  if((cameraMatrix.rotation * Vector3d(0,0,1)).z < 0)
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


Vector2d CameraGeometry::lookAtPoint(const Vector3d& point, double cameraHeight)
{
  Vector3d vector(point);
  vector.z -= cameraHeight;

  double yaw   =  atan2(vector.y, vector.x);
  double pitch = -atan2(vector.z, sqrt(vector.y*vector.y + vector.x*vector.x));

  return Vector2d(yaw, pitch);
}//end lookAtPoint

Vector2d CameraGeometry::lookAtPoint(const CameraMatrix& cameraMatrix, const Vector3d& point)
{
  // vector: O ---> point (in camera coordinates)
  Vector3d vectorToPoint = cameraMatrix.invert()*point;

  double yaw   =  atan2(vectorToPoint.y, vectorToPoint.x);
  double pitch = -atan2(vectorToPoint.z, std::hypot(vectorToPoint.x, vectorToPoint.y));

  return Vector2d(yaw, pitch);
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

Pose3D CameraGeometry::calculateCameraMatrix(
  const CameraMatrixOffset& theCameraMatrixOffset,
  const KinematicChain&     theKinematicChain,
  const InertialModel&      theInertialModel,
  const SensorJointData&    theSensorJointData,
  const Vector3d& translationOffset,
  double rotationOffsetY,
  const naoth::CameraInfo::CameraID cameraID)
{
    // get the pose of the head
    Pose3D chest(theKinematicChain.theLinks[KinematicChain::Torso].M);

    //overwrite rotation using information from the ineratial model
    chest.rotation = RotationMatrix::getRotationX(theInertialModel.orientation.x + theCameraMatrixOffset.body_rot.x);
    chest.rotateY(theInertialModel.orientation.y + theCameraMatrixOffset.body_rot.y);

    //translate pose into neck/head
    chest.translate(Vector3d(0,0,NaoInfo::NeckOffsetZ)); //neck = head

    //apply rotation of neck angles with offsets
    chest.rotateZ(theSensorJointData.position[JointData::HeadYaw]   + theCameraMatrixOffset.head_rot.z);
    chest.rotateY(theSensorJointData.position[JointData::HeadPitch] + theCameraMatrixOffset.head_rot.y);
    chest.rotateX(theCameraMatrixOffset.head_rot.x);

    //translate pose into camera
    chest.translate(translationOffset);

    //apply rotation parameter of the camera
    chest.rotateZ(theCameraMatrixOffset.cam_rot[cameraID].z)
            .rotateY(theCameraMatrixOffset.cam_rot[cameraID].y + rotationOffsetY) // tilt
            .rotateX(theCameraMatrixOffset.cam_rot[cameraID].x); // roll

    return chest;
}

// calculates the location of the camera like in forward kinematics but incorporates different angular offset in the kinematic chain from chest to camera
Pose3D CameraGeometry::calculateCameraMatrixFromChestPose(
    Pose3D chest,
    const Vector3d& translationOffset,
    double rotationOffsetY,
    const Vector2d& theBodyCorrectionOffset,
    const Vector3d& theHeadCorrectionOffset,
    const Vector3d& theCameraCorrectionOffset,
    double headYaw,
    double headPitch,
    const Vector2d& bodyRotation
  )
{   //overwrite rotation using information from the ineratial model
    chest.rotation = RotationMatrix::getRotationX(bodyRotation.x + theBodyCorrectionOffset.x);
    chest.rotateY(bodyRotation.y + theBodyCorrectionOffset.y);

    //translate pose into neck/head
    chest.translate(Vector3d(0,0,NaoInfo::NeckOffsetZ)); //neck = head

    //apply rotation of neck angles with offsets
    chest.rotateZ(headYaw   + theHeadCorrectionOffset.z);
    chest.rotateY(headPitch + theHeadCorrectionOffset.y);
    chest.rotateX(theHeadCorrectionOffset.x);

    //translate pose into camera
    chest.translate(translationOffset);

    //apply rotation parameter of the camera
    chest.rotateZ(theCameraCorrectionOffset.z)
            .rotateY(theCameraCorrectionOffset.y + rotationOffsetY) // tilt
            .rotateX(theCameraCorrectionOffset.x); // roll

    return chest;
}//end calculateCameraMatrix

double CameraGeometry::estimatedBallRadius(const Pose3D& cameraMatrix, const CameraInfo& cameraInfo, const double ballRadius, int x, int y)
{
  Vector2d pointOnField;
  if(!imagePixelToFieldCoord(
		  cameraMatrix, 
		  cameraInfo,
		  x, 
		  y, 
		  ballRadius,
		  pointOnField))
  {
    return -1;
  }

  Vector3d d = cameraMatrix.invert()*Vector3d(pointOnField.x, pointOnField.y, ballRadius);
  double cameraBallDistance = d.abs();
  if(cameraBallDistance > ballRadius) {
    double a = atan2(ballRadius, cameraBallDistance);
    return a / cameraInfo.getOpeningAngleHeight() * cameraInfo.resolutionHeight;
  }
  
  return -1;
}
