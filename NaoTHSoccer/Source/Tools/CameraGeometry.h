/**
* @file CameraGeometry.cpp
*
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
* 
* Declaration of the class CameraGeometry
*/

#ifndef _CameraGeometry_h_
#define _CameraGeometry_h_

#include "Tools/Math/Pose3D.h"
#include "Tools/NaoInfo.h"

#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include <Representations/Modeling/KinematicChain.h>
#include "Representations/Modeling/InertialModel.h"

#include <Representations/Modeling/CameraMatrixOffset.h>

/**
* The class Geometry defines representations for geometric objects and Methods
* for calculations with such object.
*
*/
class CameraGeometry
{
private:
  CameraGeometry(){} // there shouldn't be any instances of it

public:
  /** 
   * transform a point in image coordinates to the camera coordinates
   * i.e., a direction vector to the point in the image is calculated
   */
  static Vector3d imagePixelToCameraCoords( const naoth::CameraInfo& cameraInfo,
                                            const double imgX,
                                            const double imgY);

  static Vector2d relativePointToCameraAngle( const CameraMatrix& cameraMatrix,
                                              const naoth::CameraInfo& cameraInfo,
                                              const Vector3d& point);

  /**
  * the same as relativePointToImage but without rounding to int
  */
  static bool relativePointToImage( const CameraMatrix& cameraMatrix,
                                    const naoth::CameraInfo& cameraInfo,
                                    const Vector3d& point,
                                    Vector2d& pointInImage);

  /**
   * Calculate the projection of a point in relative coordinates of the robot 
   * in the image. If the point is behind the image plane the result is (-1,-1)
   * @param cameraMatrix
   * @param cameraInfo
   * @param point 3D point in robot coordinates
   * @return calculated projection in px
   */
  static bool relativePointToImage( const CameraMatrix& cameraMatrix,
                                    const naoth::CameraInfo& cameraInfo,
                                    const Vector3d& point,
                                    Vector2i& pointInImage);

  /** 
   * Calculates the angles to a point in the image
   */
  static Vector2d pixelToAngles( const naoth::CameraInfo& cameraInfo,
                                 const double imgX,
                                 const double imgY);


  /**
    wrapper
  */
  static bool imagePixelToFieldCoord( const CameraMatrix& cameraMatrix, 
                                       const naoth::CameraInfo& cameraInfo,
                                       const Vector2d& imagePoint, 
                                       const double objectHeight,
                                       Vector2d& result);

  /** 
   * Calculates from a given Pixel in the Image and the Camera Matrix the Point, 
   * where the Object lies on the Ground, or on the 
   * plane with height "objectHeight", e.g. for ball
   * if the point lies over the horizon, the function returns false
   */
  static bool imagePixelToFieldCoord(  const CameraMatrix& cameraMatrix, 
                                       const naoth::CameraInfo& cameraInfo,
                                       const double imgX, 
                                       const double imgY, 
                                       const double objectHeight,
                                       Vector2d& result);



  /** 
   * projects an point in image to a point P in the world such that P is 
   * the orthogonal projection of pointInWorld to the line defined by origin of the camera 
   * and the direction given by the point in the image
   */
  static Vector3d imagePixelToWorld( const CameraMatrix& cameraMatrix,
                                            const naoth::CameraInfo& cameraInfo,
                                            const double imgX,
                                            const double imgY,
                                            const double distance);
  
  /**
   * Calculates the Artificial Horizon by CameraMatrix and returns the two horizone
   * limiting points in the image
   */
  static void calculateArtificialHorizon(  const Pose3D& cameraMatrix,
                                           const naoth::CameraInfo& cameraInfo,
                                           Vector2d& p1,
                                           Vector2d& p2);

  /**
   * calculation of the angles from the head of the robot to a given point in
   * the egocentric coordination system of the robot
   */
  static Vector2d lookAtPoint(const Vector3d& point, double cameraHeight);


  /**
  */
  static Pose3D calculateCameraMatrix(
    const KinematicChain& theKinematicChain,
    const Vector3d& translationOffset,
    double rotationOffsetY,
    const Vector2d& theCameraCorrectionOffset);

  static Pose3D calculateCameraMatrix(
          const CameraMatrixOffset& theCameraMatrixOffset, const KinematicChain& theKinematicChain, const InertialModel &theInertialModel, const naoth::SensorJointData &theSensorJointData,
    const Vector3d& translationOffset,
    double rotationOffsetY,
    const naoth::CameraInfo::CameraID cameraID);

  static Pose3D calculateCameraMatrixFromChestPose(
      Pose3D chest,
      const Vector3d& translationOffset,
      double rotationOffsetY,
      const Vector2d &theBodyCorrectionOffset,
      const Vector3d &theHeadCorrectionOffset,
      const Vector3d &theCameraCorrectionOffset,
      double headYaw,
      double headPitch,
      const InertialModel& theInertialModel);
  };

#endif //_CameraGeometry_h_
