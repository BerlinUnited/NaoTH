#ifndef VEdge_h
#define VEdge_h

#include <iostream>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Line.h>
/*
  A class that stores an edge in Voronoi diagram

  start     : pointer to start point
  end       : pointer to end point
  left      : pointer to Voronoi place on the left side of edge
  right     : pointer to Voronoi place on the right side of edge

  neighbour : some edges consist of two parts, so we add the pointer to another part to connect them at the end of an algorithm

  direction : directional vector, from "start", points to "end", normal of |left, right|
  f, g      : directional coeffitients satisfying equation y = f*x + g (edge lies on this line)
*/

class VEdge
{
public:

  Vector2d   start;
  Vector2d   end;
  Vector2d*  direction;
  Vector2d*  left;
  Vector2d*  right;
  Math::Line* line;
  
  double    f;
  double    g;

  VEdge * neighbour;

  /*
    Constructor of the class

    s    : pointer to start
    a    : pointer to left place
    b    : pointer to right place
  */

  VEdge(Vector2d* s, Vector2d* a, Vector2d* b)
  {
    start     = *s;
    left      = a;
    right     = b;
    neighbour = 0;

    f = (b->x - a->x) / (a->y - b->y) ;
    g = s->y - f * s->x ;

    direction = new Vector2d(b->y - a->y, -(b->x - a->x));
    line = new Math::Line(*s,*direction);
  }

  /*
    Destructor of the class
    direction bes only to the current edge, other pointers can be shared by other edges
  */

  ~VEdge()
  {
    delete direction;
    delete line;
  }
  
};

#endif
