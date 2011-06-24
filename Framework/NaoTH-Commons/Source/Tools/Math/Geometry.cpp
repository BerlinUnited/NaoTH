/**
* @file Geometry.cpp
*
* @author <a href=mailto:goehring@informatik.hu-berlin.de>Daniel Goehring</a>
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
*
* Implementation of the class Geometry
*/

#include "Geometry.h"


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

