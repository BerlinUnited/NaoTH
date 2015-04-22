/**
* @file GoalBarFeature.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of struct GoalBarFeature
*/

#ifndef _GoalBarFeature_H_
#define _GoalBarFeature_H_

#include "Edgel.h"

class GoalBarFeature : public EdgelD
{
public:
  EdgelD begin;
  EdgelD end;
  double width;

  GoalBarFeature()
  :
    width(0.0)
  {}
};

#endif //_GoalBarFeature_H_
