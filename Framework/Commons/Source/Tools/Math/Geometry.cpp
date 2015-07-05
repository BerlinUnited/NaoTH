/**
* @file Geometry.cpp
*
* @author <a href=mailto:goehring@informatik.hu-berlin.de>Daniel Goehring</a>
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
*
* Implementation of the class Geometry
*/

#include "Geometry.h"

double Geometry::angleTo(const Pose2D& from, const Vector2d& to)
{
  return (Pose2D(to) - from).translation.angle();
}

double Geometry::distanceTo(const Pose2D& from, const Vector2d& to)
{
  return (Pose2D(to) - from).translation.abs();
}

Vector2d Geometry::vectorTo(const Pose2D& from, const Vector2d& to)
{
  return (Pose2D(to) - from).translation;
}


Vector2d Geometry::fieldCoordToRelative(const Pose2D& robotPose, const Vector2d& fieldCoord)
{
  // this is the same as robotPose.invert()*fieldCoord or robotPose/fieldCoord
  return (fieldCoord - robotPose.translation).rotate(-robotPose.rotation);
}


Vector2d Geometry::relativeToFieldCoord(const Pose2D& robotPose, const Vector2d& relativeCoord)
{
  return robotPose * relativeCoord;
}


bool Geometry::getIntersectionPointsOfLineAndRectangle(
  const Vector2i& bottomLeft, 
  const Vector2i& topRight,
  const Math::Line& line,
  Vector2i& point1, 
  Vector2i& point2)
{
  int foundPoints=0;
  Vector2d point[2];
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


