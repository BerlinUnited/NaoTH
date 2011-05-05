/**
* @file Geometry.cpp
*
* @author <a href=mailto:goehring@informatik.hu-berlin.de>Daniel Goehring</a>
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
* 
* Declaration of the class Geometry
*/

#ifndef __Geometry_h__
#define __Geometry_h__

#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Pose3D.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Line.h"

#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"

/**
* The class Geometry defines representations for geometric objects and Methods
* for calculations with such object.
*
*/
class Geometry
{
public:

  /** 
   * transform a point in image coordinates to the camera coordinates
   * i.e., a direction vector to the point in the image is calculated
   */
  static Vector3<double> imagePixelToCameraCoords( const CameraMatrix& cameraMatrix,
                                                   const CameraInfo& cameraInfo,
                                                   const double imgX,
                                                   const double imgY);

  /**
   * calculate the projection of a point in relative coordinates of the robot 
   * in the image
   */
  static Vector2<int> relativePointToImage( const CameraMatrix& cameraMatrix,
                                            const CameraInfo& cameraInfo,
                                            const Vector3<double>& point);

  /** 
   * calculates the angles to a point in the image
   */
  static Vector2<double> angleToPointInImage( const CameraMatrix& cameraMatrix,
                                              const CameraInfo& cameraInfo,
                                              const double imgX,
                                              const double imgY);


  /** 
   * Calculates from a given Pixel in the Image and the Camera Matrix the Point, 
   * where the Object lies on the Ground, or on the 
   * plane with height "objectHeight", e.g. for ball
   * if the point lies over the horizon, the function returns false
   */
  static bool imagePixelToFieldCoord(  const CameraMatrix& cameraMatrix, 
                                       const CameraInfo& cameraInfo,
                                       const double imgX, 
                                       const double imgY, 
                                       const double objectHeight,
                                       Vector2<double>& result);



  /** 
   * projects an point in image to a point P in the world such that P is 
   * the orthogonal projection of pointInWorld to the line defined by origin of the camera 
   * and the direction given by the point in the image
   */
  static Vector3<double> imagePixelToWorld( const CameraMatrix& cameraMatrix,
                                            const CameraInfo& cameraInfo,
                                            const double imgX,
                                            const double imgY,
                                            const Vector3<double>& pointInWorld);
  
  /**
   * Calculates the Artificial Horizon by CameraMatrix and returns the two horizone
   * limiting points in the image
   */
  static void calculateArtificialHorizon(  const Pose3D& cameraMatrix,
                                           const CameraInfo& cameraInfo,
                                           Vector2<double>& p1,
                                           Vector2<double>& p2);

  /**
   * calculation of the angles from the head of the robot to a given point in
   * the egocentric coordination system of the robot
   */
  Vector2<double> lookAtPoint(Vector3<double> point, double cameraHeight);


  /** 
  * GT07
  * Calculates the angle between a pose and a position
  * @param from The base pose.
  * @param to The other position.
  * @return the angle from the pose to the position.
  */
  static double angleTo(const Pose2D& from, 
    const Vector2<double>& to);

  /** 
  * GT07
  * Calculates the distance from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the distance from the pose to the position.
  */
  static double distanceTo(const Pose2D& from, 
    const Vector2<double>& to);

  /** 
  * GT07
  * Calculates the relative vector from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the vector from the pose to the position.
  */
  static Vector2<double> vectorTo(const Pose2D& from, const Vector2<double>& to);

  /**
  * GT07
  * Function does the transformation from 2d field coordinates
	* to coordinates relative to the robot.
  * @param robotPose current Robot Pose.
  * @param fieldCoord absolute field coordinates of a position
  * @return Returns the positon in relative coordinates
  */
	static Vector2<double> fieldCoordToRelative(const Pose2D& robotPose, const Vector2<double>& fieldCoord);


  /**
  * GT07
  * Function does the transformation from 2d relative robot coordinates
	* to absolute field coordinates.
  * @param rp current Robot Pose.
  * @param relativeCoord relative coordinates of a position (relative to robot)
  * @return Returns the positon in absolute coordinates
  */
	static Vector2<double> relativeToFieldCoord(const Pose2D& robotPose, const Vector2<double>& relativeCoord);
};

#endif //__Geometry_h____
