/**
* @file Line.cpp
*
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
* 
* Declaration of the class Line
*/

#ifndef _Line_h_
#define _Line_h_

#include <limits>
#include <cmath>
#include <algorithm>
#include "Vector2.h"
#include "Pose2D.h"

namespace Math 
{

/** Defines a (infinite) line by a base point and direction */
class Line
{
protected:
  Vector2d base;
  Vector2d direction;
  
public:
  Line() : direction(1.0,0.0) {}
  
  template <class V, class W>
  Line(const Vector2<V>& base, const Vector2<W>& direction) 
  {
    this->base = base;
    this->direction = Vector2d(direction).normalize(); //TODO: make it better?
  }

  Line(const Pose2D& pose)
  {
    this->base = pose.translation;
    this->direction = (Pose2D(pose.rotation) + Pose2D(Vector2d(1.0,0))).translation;
  }

  Vector2d getDirection() const { return direction; }
  Vector2d getBase() const { return base; }
  Vector2d point(double t) const { return base + direction*t; }

  // ax*x + ay*y = b
  double intersection(const Line& other) const
  {
    Vector2d normal(-other.direction.y, other.direction.x);
    double t = normal*direction;
    if(t == 0) { // the lines are parallel
      return std::numeric_limits<double>::infinity(); 
    }
    return (normal*(other.base-base)) / t;
  }

  double minDistance(const Vector2d& p) const {
    return std::fabs(direction.x*(p.y-base.y)-direction.y*(p.x-base.x));
  }

  double project(const Vector2d& p) const {
    return direction*p - direction*base;
  }

  Vector2d projection(const Vector2d& p) const {
    return point(project(p));
  }
};


/** Defines a (finite) segment of a line */
class LineSegment: public Line 
{
protected:
  double length;

public:
  LineSegment():length(0){}

  template <class V>
  LineSegment(const Vector2<V>& begin, const Vector2<V>& end)
  {
    base = begin;
    direction = end-begin;
    length = direction.abs();
    direction.normalize();
  }

  template <class V>
  LineSegment(const Vector2<V>& base, const Vector2<V>& direction, double length)
    : Line(base,direction),
      length(length)
  {
  }

  ~LineSegment(){}

  const Vector2d& begin() const { return base; }
  Vector2d end() const { return base + direction*length; }
  Vector2d point(double t) const { return Line::point(Math::clamp(t, 0.0, length)); }
  double getLength() const { return length; }

  double intersection(const Line& other) const
  {
    double t = Line::intersection(other);
    return Math::clamp(t, 0.0, length);
  }

  bool intersect(const Line& other) const
  {
    double t = Line::intersection(other);
    return 0.0 <= t && t <= length;
  }

  /*
  // projection of the point to the line not to closest point on linesegment, if needed please adjust
  double project(const Vector2d& p) const
  {
    return direction*p - direction*base;
  }
  */

  /** projection of the point to the linesegment */
  Vector2d projection(const Vector2d& p) const
  {
    double t = direction*p - direction*base;
    return point(t);
  }

  /** Minimal distance to a point (2D). Will check the borders of the segment. */
  double minDistance(const Vector2d& p) const {
    return (projection(p) - p).abs();
  }
};

}//end namespace Math

#endif //_Line_h_
