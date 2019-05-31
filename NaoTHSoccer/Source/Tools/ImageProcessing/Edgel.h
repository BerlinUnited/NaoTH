/**
* @file Edgel.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of struct Edgel
*/

#ifndef _Edgel_H_
#define _Edgel_H_

#include <Tools/Math/Vector2.h>

template<class T>
class EdgelT
{
public:
  enum Type
  {
    positive,
    negative,
    unknown
  };

public:
  EdgelT() 
    : type(unknown)
  {}

  EdgelT(const Vector2<T>& point, const Vector2d& direction, Type type)
    :
    point(point), direction(direction), type(type)
  {}


  Vector2<T> point;
  Vector2d direction;
  Type type; // deprecated

  // calculate the simmilarity to the other edgel
  // returns a value [0,1], 0 - not simmilar, 1 - very simmilar
  inline double sim(const EdgelT<T>& other) const
  {
    double s = 0.0;
    if(direction*other.direction > 0) {
      Vector2d v(other.point - point);
      v.rotateRight().normalize();
      s = 1.0-0.5*(fabs(direction*v) + fabs(other.direction*v));
    }

    return s;
  }

  // calculate the simmilarity to the other edgel
  // returns a value [0,1], 0 - not simmilar, 1 - very simmilar
  inline double sim2(const EdgelT<T>& other) const
  {
    double s = 0.0;
    if(direction*other.direction > 0) {
      Vector2d v(other.point - point);
      v.normalize();
      s = 1.0-0.5*(fabs(direction*v) + fabs(other.direction*v));
    }

    return s;
  }
};

typedef EdgelT<int> Edgel;
typedef EdgelT<double> EdgelD;

#endif //_Edgel_H_
