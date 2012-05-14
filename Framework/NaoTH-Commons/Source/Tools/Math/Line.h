/**
* @file Line.cpp
*
* @author <a href=mailto:mellmann@informatik.hu-berlin.de>Heinrich Mellmann</a>
* 
* Declaration of the class Line
*/

#ifndef __Line_h__
#define __Line_h__

#include <limits>
#include <cmath>
#include "Vector2.h"
#include "Pose2D.h"

namespace Math {

/** Defines a (infinite) line by a base point and direction */
class Line
{
protected:
  Vector2<double> base;
  Vector2<double> direction;
  
public:
  Line() : direction(1.0,0.0) {}
  
  template <class V, class W>
  Line(const Vector2<V>& base, const Vector2<W>& direction) 
  {
    this->base = base;
    this->direction = Vector2<double>(direction).normalize(); //TODO: make it better?
  }



  Line(const Pose2D& pose)
  {
    this->base = pose.translation;
    this->direction = (Pose2D(pose.rotation) + Pose2D(Vector2<double>(1.0,0))).translation;
  }

  // ax*x + ay*y = b
  double intersection(const Line& other) const
  {
    Vector2<double> normal(-other.direction.y, other.direction.x);
    double t = normal*direction;
    if(t == 0) return std::numeric_limits<double>::infinity(); // the lines are paralell
    return normal*(other.base-base)/(t);
  }//end intersection

  Vector2<double> point(double t) const { return base + direction*t; }
  Vector2<double> getDirection() const { return direction; }
  Vector2<double> getBase() const { return base; }


  double minDistance(Vector2<double> p) const
  {
    return std::abs(direction.x*(p.y-base.y)+direction.y*(p.x-base.x))/direction.abs();
  }

};


/** Defines a (finite) segment of a line */
class LineSegment: public Line 
{
protected:
  //const Vector2<double> begin;
  //const Vector2<double> end;
  
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
  ~LineSegment(){}

  const Vector2<double>& begin() const { return base; }
  Vector2<double> end() const { return base + direction*length; }
  Vector2<double> point(double t) const { return Line::point(Math::clamp(t, 0.0, length)); }
  double getLength() const { return length; }

  double intersection(const Line& other) const
  {
    double t = Line::intersection(other);
    return Math::clamp(t, 0.0, length);
  }//end intersection

  bool intersect(const Line& other) const
  {
    double t = Line::intersection(other);
    return 0.0 <= t && t <= length;
  }//end intersect

  /** projection of the point to the line */
  double project(const Vector2<double>& p) const
  {
    return direction*p - direction*base;
  }//end project

  /** projection of the point to the line */
  Vector2<double> projection(const Vector2<double>& p) const
  {
    double t = direction*p - direction*base;
    return point(t);
  }//end projection


  /** Minimal distance to a point (2D). Will check the borders of the segment. */
  double minDistance(const Vector2<double>& p) const
  {
    return (projection(p) - p).abs();
    /*
    double distance = Line::minDistance(p);

    // a^2 + b^2 = c^2 => a = sqrt(c^2-b^2)
    Vector2<double> base2p = base - p;

    double c2 = pow(base2p.abs(),2);
    double b2 = pow(distance,2);

    double diff = c2-b2;
    if(diff < 0)
    {
      // fallback
      return distance;
    }

    double distance2Intersection = sqrt(diff);
    if(distance2Intersection <= length)
    {
      // in our bounds, return the result calculated by the line
      return distance;
    }
    else
    {
      // out of bounds, return either distance to start or end
      double distBegin = (begin()-p).abs();
      double distEnd = (end()-p).abs();
      if(distBegin <= distEnd)
      {
        return distBegin;
      }
      else
      {
        return distEnd;
      }
    }*/
  }//end minDistance

};

class Intersection
{
public:
  enum IntersectionType
  {
    unknown, //equals ColorClasses::none
    T, //1 line intersect an other, equals ColorClasses::orange
    L, //2 lines touch, equals ColorClasses::yellow
    C, //lines intersect on/with circle, equals ColorClasses::skyblue
    E, //line extends an other, equals ColorClasses::white
    X, //2 intersect each other, equals ColorClasses::red
    none //the line segments don't intersect
  };

  Intersection(const LineSegment& segmentOne, const LineSegment& segmentTwo)
    : segmentOne(segmentOne),
      segmentTwo(segmentTwo)
  {
    double s = segmentOne.Line::intersection(segmentTwo);
    double t = segmentTwo.Line::intersection(segmentOne);
    
    double angleDiff = fabs(Math::normalize(segmentOne.getDirection().angle() - segmentTwo.getDirection().angle()));
    angleDiff = std::min(angleDiff, Math::pi - angleDiff);

    if(segmentOne.getLength() < s || 
       s < 0 || 
       segmentTwo.getLength() < t || 
       t < 0 ||
       angleDiff < Math::pi_2 - 0.85 || 
       angleDiff > Math::pi_2 + 0.85 ) // 0.85 = 5deg
      type = none;
    else if(angleDiff > 0.85 * 2.0 && angleDiff < Math::pi_2 - 0.85 * 2.0 )
      type = C;
    else if(0 < s && s < segmentOne.getLength() && 0 < t && t < segmentTwo.getLength())
      type = X;
    else if((0 < s && s < segmentOne.getLength()) || (0 < t && t < segmentTwo.getLength()))
      type = T;
    else
      type = L;

    pos = segmentOne.point(s);
  }

  ~Intersection(){}

  IntersectionType type;
  Vector2<int> pos; /**< The fieldcoordinates of the intersection */
  LineSegment segmentOne;
  LineSegment segmentTwo;

};

}//end namespace Math

#endif //__Line_h__
