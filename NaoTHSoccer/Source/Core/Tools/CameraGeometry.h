/**
* @file CameraGeometry.cpp
*
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
* 
* Declaration of the class CameraGeometry
*/

#ifndef __CameraGeometry_h__
#define __CameraGeometry_h__

#include "Tools/Math/Pose3D.h"

#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"

/**
* The class Geometry defines representations for geometric objects and Methods
* for calculations with such object.
*
*/
class CameraGeometry
{
public:
  /** 
   * transform a point in image coordinates to the camera coordinates
   * i.e., a direction vector to the point in the image is calculated
   */
  static Vector3<double> imagePixelToCameraCoords( const CameraMatrix& cameraMatrix,
                                                   const naoth::CameraInfo& cameraInfo,
                                                   const double imgX,
                                                   const double imgY);

  /**
   * calculate the projection of a point in relative coordinates of the robot 
   * in the image
   */
  static Vector2<int> relativePointToImage( const CameraMatrix& cameraMatrix,
                                            const naoth::CameraInfo& cameraInfo,
                                            const Vector3<double>& point);

  /** 
   * calculates the angles to a point in the image
   */
  static Vector2<double> angleToPointInImage( const CameraMatrix& cameraMatrix,
                                              const naoth::CameraInfo& cameraInfo,
                                              const double imgX,
                                              const double imgY);


  /**
    wrapper
  */
  static bool imagePixelToFieldCoord( const CameraMatrix& cameraMatrix, 
                                       const naoth::CameraInfo& cameraInfo,
                                       const Vector2<double>& imagePoint, 
                                       const double objectHeight,
                                       Vector2<double>& result);

  /** 
   * Calculates from a given Pixel in the Image and the Camera Matrix the Point, 
   * where the Object lies on the Ground, or on the 
   * plane with height "objectHeight", e.g. for ball
   * if the point lies over the horizon, the function returns false
   */
  static bool imagePixelToFieldCoord(  const CameraMatrix& cameraMatrix, 
                                       const naoth::CameraInfo& cameraInfo,
                                       const double& imgX, 
                                       const double& imgY, 
                                       const double& objectHeight,
                                       Vector2<double>& result);



  /** 
   * projects an point in image to a point P in the world such that P is 
   * the orthogonal projection of pointInWorld to the line defined by origin of the camera 
   * and the direction given by the point in the image
   */
  static Vector3<double> imagePixelToWorld( const CameraMatrix& cameraMatrix,
                                            const naoth::CameraInfo& cameraInfo,
                                            const double imgX,
                                            const double imgY,
                                            const Vector3<double>& pointInWorld);
  
  /**
   * Calculates the Artificial Horizon by CameraMatrix and returns the two horizone
   * limiting points in the image
   */
  static void calculateArtificialHorizon(  const Pose3D& cameraMatrix,
                                           const naoth::CameraInfo& cameraInfo,
                                           Vector2<double>& p1,
                                           Vector2<double>& p2);

  /**
   * calculation of the angles from the head of the robot to a given point in
   * the egocentric coordination system of the robot
   */
  Vector2<double> lookAtPoint(Vector3<double> point, double cameraHeight);

  };

#endif //__CameraGeometry_h____