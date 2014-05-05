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
#include "Tools/Math/Common.h"
#include "Tools/Math/Line.h"
#include "Tools/Math/PointList.h"

/**
* The class Geometry defines representations for geometric objects and Methods
* for calculations with such object.
*
*/
class Geometry
{
public:
  /** 
  * copied from GT07
  * Calculates the angle between a pose and a position
  * @param from The base pose.
  * @param to The other position.
  * @return the angle from the pose to the position.
  */
  static double angleTo(const Pose2D& from, const Vector2<double>& to);

  /** 
  * copied from GT07
  * Calculates the distance from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the distance from the pose to the position.
  */
  static double distanceTo(const Pose2D& from, const Vector2<double>& to);

  /** 
  * copied from GT07
  * Calculates the relative vector from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the vector from the pose to the position.
  */
  static Vector2<double> vectorTo(const Pose2D& from, const Vector2<double>& to);

  /**
  * copied from GT07
  * Function does the transformation from 2d field coordinates
  * to coordinates relative to the robot.
  * @param robotPose current Robot Pose.
  * @param fieldCoord absolute field coordinates of a position
  * @return Returns the positon in relative coordinates
  */
  static Vector2<double> fieldCoordToRelative(const Pose2D& robotPose, const Vector2<double>& fieldCoord);


  /**
  * copied from GT07
  * Function does the transformation from 2d relative robot coordinates
  * to absolute field coordinates.
  * @param rp current Robot Pose.
  * @param relativeCoord relative coordinates of a position (relative to robot)
  * @return Returns the positon in absolute coordinates
  */
  static Vector2<double> relativeToFieldCoord(const Pose2D& robotPose, const Vector2<double>& relativeCoord);


  /**
  * copied from BHuman10
  * Clips a line with a rectangle
  * @param bottomLeft The bottom left corner of the rectangle
  * @param topRight The top right corner of the rectangle
  * @param line The line to be clipped
  * @param point1 The starting point of the resulting line
  * @param point2 The end point of the resulting line
  * @return states whether clipping was necessary (and done)
  */
  static bool getIntersectionPointsOfLineAndRectangle(
    const Vector2<int>& bottomLeft, 
    const Vector2<int>& topRight,
    const Math::Line& line,
    Vector2<int>& point1, 
    Vector2<int>& point2);

  /**
  * Calculated a cricle out of up to 20 points
  * @param pointList The list of points
  * @param center The calulated center returned if ready
  * @param radius The calulated radius returned if ready
  */
  static bool calculateCircle( 
    const PointList<20>& pointList, 
    Vector2d& center, 
    double& radius);

	
  class Rect2d {
  private:
    Vector2d minimum;
    Vector2d maximum;

  public:
    Rect2d() {}
    Rect2d(const Vector2d& a, const Vector2d& b) 
	    :
	    minimum(std::min(a.x,b.x), std::min(a.y,b.y)),
	    maximum(std::max(a.x,b.x), std::max(a.y,b.y))
	  {
    }

    inline bool inside(const Vector2d& p) const {
       return maximum.x >= p.x && p.x >= minimum.x &&
              maximum.y >= p.y && p.y >= minimum.y;
    }

    inline const Vector2d& min() const { return minimum; }
    inline const Vector2d& max() const { return maximum; }
  };
};

#endif //__Geometry_h____
