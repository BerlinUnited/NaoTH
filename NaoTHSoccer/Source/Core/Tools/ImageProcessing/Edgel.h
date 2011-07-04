/**
* @file Edgel.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of struct Edgel
*/

#include "Tools/ColorClasses.h"

#ifndef __Edgel_H_
#define __Edgel_H_


class Edgel
{
public:
  Edgel()
    :
    center_angle(0.0),
    begin_angle(0.0),
    end_angle(0.0),
    thickness(0),
    valid(false),
    vertical(false),
    runID(0),
    ScanLineID(0)
  {}

	Vector2<int> center;
	double center_angle;
	Vector2<int> begin;
	double begin_angle;
	Vector2<int> end;
	double end_angle;

  double thickness;

	bool valid;
  bool vertical;
  unsigned int runID;
  unsigned int ScanLineID;
};

#endif //__Edgel_H_
