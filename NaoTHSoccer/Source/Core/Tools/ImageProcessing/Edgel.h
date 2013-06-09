/**
* @file Edgel.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of struct Edgel
*/

#ifndef _Edgel_H_
#define _Edgel_H_

class Edgel
{
public:
  Edgel() : angle(0.0)
  {}

  Vector2<int> point;
  double angle;
};

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

  Vector2<int> center;
  double center_angle;
  Vector2<int> begin;
  double begin_angle;
  Vector2<int> end;
  double end_angle;

  bool valid;
  unsigned int runID;
  unsigned int ScanLineID;
};

#endif //_Edgel_H_
