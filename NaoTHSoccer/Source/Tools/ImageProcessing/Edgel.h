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
  EdgelT() : type(unknown), angle(0.0)
  {}

  enum Type
  {
    positive,
    negative,
    unknown
  };

  Vector2<T> point;
  Vector2d direction;
  Type type;

  double angle; // deprecated

  // calculate the simmilarity to the other edgel
  // returns a value [0,1], 0 - not simmilar, 1 - very simmilar
  inline double sim(const EdgelT<T>& other) const
  {
    double s = 0.0;
    if(direction*other.direction > 0) {
      Vector2d v = (other.point - point).rotateRight().normalize();
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
      Vector2d v = (other.point - point).normalize();
      s = 1.0-0.5*(fabs(direction*v) + fabs(other.direction*v));
    }

    return s;
  }
};

typedef EdgelT<int> Edgel;
typedef EdgelT<double> EdgelD;

class DoubleEdgel
{
public:
  DoubleEdgel()
    :
    center_angle(0.0),
    begin_angle(0.0),
    end_angle(0.0),
    valid(false),
    runID(0),
    ScanLineID(0)
  {}

  Vector2d dCenter;
  Vector2i center; // deprecated
  double center_angle;

  Vector2i begin;
  
  double begin_angle;

  Vector2i end;
  double end_angle;

  bool valid;
  unsigned int runID;
  unsigned int ScanLineID;
};

#endif //_Edgel_H_
