#ifndef VEvent_h
#define VEvent_h

#include <iostream>
#include "VParabola.h"

/*
The class for storing place / circle event in event queue.

point		: the point at which current event occurs (top circle point for circle event, focus point for place event)
pe			: whether it is a place event or not
y			: y coordinate of "point", events are sorted by this "y"
arch		: if "pe", it is an arch above which the event occurs
*/

class VEvent
{
public:
  Vector2d*	point;
  bool pe;
  VParabola* arch;
  Vector2d intersecitonPoint;
  
  /*
  Constructor for the class

  p		: point, at which the event occurs
  pev		: whether it is a place event or not
  */

  VEvent(Vector2d* p, bool pev)
  {
    point	= p;
    pe		= pev;
    arch	= 0;
  }

  /*
  function for comparing two events (by "y" property)
  */

  struct CompareEvent : public std::binary_function<VEvent*, VEvent*, bool>
  {
    bool operator()(const VEvent* l, const VEvent* r) const
    {
      if (l->point->y < r->point->y){
        return true;
      }

      if ( (l->point->y == r->point->y) && (l->point->x < r->point->x)){
        return true;
      }

      return false; 
    }
  };
};

#endif
