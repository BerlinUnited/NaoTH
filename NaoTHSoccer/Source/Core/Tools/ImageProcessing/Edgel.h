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
  EdgelT() : angle(0.0)
  {}

  Vector2<T> point;
  Vector2d direction;

  double angle; // deprecated
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
