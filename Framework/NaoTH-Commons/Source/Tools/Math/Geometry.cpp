/**
* @file Geometry.cpp
*
* @author <a href=mailto:goehring@informatik.hu-berlin.de>Daniel Goehring</a>
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
*
* Implementation of the class Geometry
*/

#include "Geometry.h"
#include "Tools/Math/Matrix_nxn.h"

double Geometry::angleTo(const Pose2D& from, const Vector2<double>& to)
{
  return (Pose2D(to) - from).translation.angle();
}//end angleTo

double Geometry::distanceTo(const Pose2D& from, const Vector2<double>& to)
{
  return (Pose2D(to) - from).translation.abs();
}//end distanceTo

Vector2<double> Geometry::vectorTo(const Pose2D& from, const Vector2<double>& to)
{
  return (Pose2D(to) - from).translation;
}//end vectorTo


Vector2<double> Geometry::fieldCoordToRelative(const Pose2D& robotPose, const Vector2<double>& fieldCoord)
{
  // this is the same as robotPose.invert()*fieldCoord or robotPose/fieldCoord
  return (fieldCoord - robotPose.translation).rotate(-robotPose.rotation);
}//end fieldCoordToRelative


Vector2<double> Geometry::relativeToFieldCoord(const Pose2D& robotPose, const Vector2<double>& relativeCoord)
{
  return robotPose * relativeCoord;
}//end relativeToFieldCoord


bool Geometry::getIntersectionPointsOfLineAndRectangle(
  const Vector2<int>& bottomLeft, 
  const Vector2<int>& topRight,
  const Math::Line& line,
  Vector2<int>& point1, 
  Vector2<int>& point2)
{
  int foundPoints=0;
  Vector2<double> point[2];
  if (line.getDirection().x!=0)
  {
    double y1=line.getBase().y+(bottomLeft.x-line.getBase().x)*line.getDirection().y/line.getDirection().x;
    if ((y1>=bottomLeft.y)&&(y1<=topRight.y))
    {
      point[foundPoints].x=(double) bottomLeft.x;
      point[foundPoints++].y=y1;
    }
    double y2=line.getBase().y+(topRight.x-line.getBase().x)*line.getDirection().y/line.getDirection().x;
    if ((y2>=bottomLeft.y)&&(y2<=topRight.y))
    {
      point[foundPoints].x=(double) topRight.x;
      point[foundPoints++].y=y2;
    }
  }
  if (line.getDirection().y!=0)
  {
    double x1=line.getBase().x+(bottomLeft.y-line.getBase().y)*line.getDirection().x/line.getDirection().y;
    if ((x1>=bottomLeft.x)&&(x1<=topRight.x)&&(foundPoints<2))
    {
      point[foundPoints].x=x1;
      point[foundPoints].y=(double) bottomLeft.y;
      if ((foundPoints==0)||((point[0]-point[1]).abs()>0.1))
      {
        foundPoints++;
      }
    }
    double x2=line.getBase().x+(topRight.y-line.getBase().y)*line.getDirection().x/line.getDirection().y;
    if ((x2>=bottomLeft.x)&&(x2<=topRight.x)&&(foundPoints<2))
    {
      point[foundPoints].x=x2;
      point[foundPoints].y=(double) topRight.y;
      if ((foundPoints==0)||((point[0]-point[1]).abs()>0.1))
      {
        foundPoints++;
      }
    }
  }
  switch (foundPoints)
  {
  case 1:
    point1.x=(int)point[0].x;
    point2.x=point1.x;
    point1.y=(int)point[0].y;
    point2.y=point1.y;
    foundPoints++;
    return true;
  case 2:
    if ((point[1]-point[0])*line.getDirection() > 0)
    {
      point1.x=(int)point[0].x;
      point1.y=(int)point[0].y;
      point2.x=(int)point[1].x;
      point2.y=(int)point[1].y;
    }
    else
    {
      point1.x=(int)point[1].x;
      point1.y=(int)point[1].y;
      point2.x=(int)point[0].x;
      point2.y=(int)point[0].y;
    }
    return true;
  default:
    return false;
  }
}//end getIntersectionPointsOfLineAndRectangle

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


bool Geometry::calculateCircle( const PointList<20>& pointList, Vector2d& center, double& radius )
{
  if (pointList.length < 3)
    return false;
  
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
  double n = pointList.length;

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
  }//end for
  
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
    
    center.x = (int)(-x[0] / 2.0);
    center.y = (int)(-x[1] / 2.0);
    

    double tmpWurzel = x[0]*x[0]/4.0 + x[1]*x[1]/4.0 - x[2];
    
    if (tmpWurzel < 0.0)
      return false;
    
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



