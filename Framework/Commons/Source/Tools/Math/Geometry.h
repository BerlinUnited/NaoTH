/**
* @file Geometry.cpp
*
* @author <a href=mailto:goehring@informatik.hu-berlin.de>Daniel Goehring</a>
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
* 
* Declaration of the class Geometry
*/

#ifndef _Geometry_h_
#define _Geometry_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Line.h"

#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Matrix_mxn.h"

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
  static double angleTo(const Pose2D& from, const Vector2d& to);

  /** 
  * copied from GT07
  * Calculates the distance from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the distance from the pose to the position.
  */
  static double distanceTo(const Pose2D& from, const Vector2d& to);

  /** 
  * copied from GT07
  * Calculates the relative vector from a pose to a position
  * @param from The base pose.
  * @param to The other position.
  * @return the vector from the pose to the position.
  */
  static Vector2d vectorTo(const Pose2D& from, const Vector2d& to);

  /**
  * copied from GT07
  * Function does the transformation from 2d field coordinates
  * to coordinates relative to the robot.
  * @param robotPose current Robot Pose.
  * @param fieldCoord absolute field coordinates of a position
  * @return Returns the positon in relative coordinates
  */
  static Vector2d fieldCoordToRelative(const Pose2D& robotPose, const Vector2d& fieldCoord);


  /**
  * copied from GT07
  * Function does the transformation from 2d relative robot coordinates
  * to absolute field coordinates.
  * @param rp current Robot Pose.
  * @param relativeCoord relative coordinates of a position (relative to robot)
  * @return Returns the positon in absolute coordinates
  */
  static Vector2d relativeToFieldCoord(const Pose2D& robotPose, const Vector2d& relativeCoord);


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
    const Vector2i& bottomLeft, 
    const Vector2i& topRight,
    const Math::Line& line,
    Vector2i& point1, 
    Vector2i& point2);

  /**
  * Calculated a cricle out of up to 20 points
  * @param pointList The list of points
  * @param center The calulated center returned if ready
  * @param radius The calulated radius returned if ready
  */
  //static bool calculateCircle( 
  //  const PointList<20>& pointList, 
  //  Vector2d& center, 
  //  double& radius);

  /**********************************************************************************
      Mathematical Foundations
    ---------------------------------------------------------------------------------
    Given: Array of Points (xi, yi), i = 1,...,n belonging to the circle.
    Consider the following circle model: (x-xi)^2 + (y-yi)^2 = r^2
    where (x, y) is the center and r the radius of the circle.

    Now we make the following substitution to make the problem linear:
      r^2 = a^2/4 + b^2/4 - c
      x   = -a/2
      y   = -b/2
    Thus we get the following transformated formula
        (x-xi)^2 + (y-yi)^2 = r^2
    <=> x^2 - 2xxi + xi^2 + y^2 - 2yyi yi^2 - r^2 = 0
    (substitution)
    <=> (-a/2)^2 - 2(-a/2)xi + xi^2 + (-b/2)^2 - 2(-b/2)yi + yi^2 - (a^2/4 + b^2/4 - c) = 0
    <=> a^2/4 + axi + xi^2 + b^2/4 + byi + yi^2 - a^2/4 - b^2/4 + c = 0
        +----------------------------------+
    <=> | axi + xi^2 + byi + yi^2 + c = 0  |
        +----------------------------------+
    <=>   axi + byi + c = - xi^2 - yi^2

    Thus we can write the Problem as a linear equation:

      Ax=B
  
    where 

      x = (a, b, c)^t
      B = - ( (x1^2 + y1^2), ... , (xn^2 + yn^2) )^t
    
          x1  y1  1
          .   .   .
      A = .   .   .
          .   .   .
          xn  yn  1
  
    Now we solve the problem my means of Least Square Method
      x = (A^tA)^(-1)A^tB
  **********************************************************************************/

  template<class T>
  static bool calculateCircle( const T& pointList, Vector2d& center, double& radius )
  {
    if (pointList.size() < 3) {
      return false;
    }
  
    /********************************************************
     * create the matrix M := A^tA and the vector v := A^tB
     ********************************************************/

    double Mx = 0.0;   // \sum_{k=1}^n x_k
    double My = 0.0;   // \sum_{k=1}^n y_k
    double Mxx = 0.0;  // \sum_{k=1}^n x_k^2
    double Myy = 0.0;  // \sum_{k=1}^n y_k^2
    double Mxy = 0.0;  // \sum_{k=1}^n y_k * x_k
    double Mz = 0.0;   // \sum_{k=1}^n (y_k^2 + x_k^2) = Myy + Mxx
    double Mxz = 0.0;  // \sum_{k=1}^n x_k*(y_k^2 + x_k^2)
    double Myz = 0.0;  // \sum_{k=1}^n y_k*(y_k^2 + x_k^2)
    double n = (double)pointList.size();

    // calculate the entries for A^tA and A^tB
    for (int i = 0; i < n; ++i)
    {
      double x = pointList[i].x;
      double y = pointList[i].y;
      double xx = x*x;
      double yy = y*y;
      double z = xx + yy;
    
      Mx += x;
      My += y;
      Mxx += xx;
      Myy += yy;
      Mxy += x*y;
      Mz += z;
      Mxz += x*z;
      Myz += y*z;
    }
  
    try
    {
      // create the matrix M := A^tA
      Matrix_nxn<double, 3> M;
      double Matrix[9] = 
      {
          Mxx, Mxy, Mx,
          Mxy, Myy, My,
          Mx , My , n
      };
      M = Matrix;
    
      // create the vector v := A^tB
      Vector_n<double, 3> v;
    
      v[0] = -Mxz;
      v[1] = -Myz;
      v[2] = -Mz;


      /********************************************************
       * solve the Problem Mx = v <=> (A^tA)x = A^tB
       ********************************************************/
      Vector_n<double, 3> x;
      x = M.solve(v);
    
      center.x = -x[0] * 0.5;
      center.y = -x[1] * 0.5;
    

      double tmpWurzel = x[0]*x[0]/4.0 + x[1]*x[1]/4.0 - x[2];
    
      if (tmpWurzel < 0.0) {
        return false;
      }
    
      radius = sqrt(tmpWurzel);

    }
    catch (MVException)
    {
      return false;
    }
    catch (...)
    {
      //OUTPUT(idText, text, "Unknown exception in BallDetector::calculateCircle");
      return false;
    }
  
    return true;
  }//end calculateCircle
	

  /**
  A simple routine to estimate a circle from a set of points
  */
  template<class T>
  static bool estimateCircle( const T& points, Vector2d& center, double& radius )
  {
    if (points.size() < 3) {
      return false;
    }

    Math::Matrix_mxn<double> A(static_cast<unsigned int> (points.size()), 3);
    Math::Matrix_mxn<double> b(static_cast<unsigned int> (points.size()), 1);

    for(unsigned int i = 0; i < static_cast<unsigned int>(points.size()); i++)
    {
      A(i, 0) = points[i].x;
      A(i, 1) = points[i].y;
      A(i, 2) = 1.0;
      b(i, 0) = - points[i].abs2(); // - (x^2 + y^2)
    }
    
    try {
      Math::Matrix_mxn<double> AT(A.transpose());
      //Math::Matrix_mxn<double> result(((AT * A).invert() * AT) * b);
      Math::Matrix_mxn<double> result =  (AT*A).solve(AT*b);

      center = Vector2d(-result(0, 0)*0.5, -result(1, 0)*0.5);
      radius = sqrt(fabs(result(2, 0) - center.abs2()));
    }
    catch (MVException) {
      return false;
    }
    catch (...) {
      return false;
    }

    return true;
  }

  template<class T>
  static void simpleLinearRegression(const T& points, double& slope, double& intercept) 
  {
    if(points.size() < 2) {
      slope = 0.0;
      intercept = points[0].y;
      return;
    }

    Vector2d avg;
    for(const Vector2d& point : points) {
      avg += point;
    }
    avg /= static_cast<double>(points.size());

    double top = 0.;
    double bot = 0.;
    for(const Vector2d& point : points) {
      double x_diff = point.x - avg.x;
      top += x_diff * (point.y - avg.y);
      bot += x_diff * x_diff;
    }

    slope = top / bot;
    intercept = avg.y - slope * avg.x;
  }



  class Rect2d 
  {
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

    Rect2d(double x0, double y0, double x1, double y1) :
      minimum(std::min(x0,x1), std::min(y0,y1)),
	    maximum(std::max(x0,x1), std::max(y0,y1))
    {}

    inline bool inside(const Vector2d& p) const {
       return maximum.x >= p.x && p.x >= minimum.x &&
              maximum.y >= p.y && p.y >= minimum.y;
    }

    inline const Vector2d& min() const { return minimum; }
    inline const Vector2d& max() const { return maximum; }
  };
};

#endif //__Geometry_h____
