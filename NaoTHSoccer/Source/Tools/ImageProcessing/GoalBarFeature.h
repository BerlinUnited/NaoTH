/**
* @file GoalBarFeature.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of struct GoalBarFeature
*/

#ifndef _GoalBarFeature_H_
#define _GoalBarFeature_H_

#include "Edgel.h"

class GoalBarFeature : public EdgelT<double>
{
public:
  //Vector2i begin;
  //Vector2i end;
  double width;

  GoalBarFeature()
  :
    //begin(-1,-1),
    //end(-1, -1),
    width(0.0)
  {}
};

#endif //_GoalBarFeature_H_
