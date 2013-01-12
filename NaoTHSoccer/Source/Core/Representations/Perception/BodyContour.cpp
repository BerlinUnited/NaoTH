/**
* @file BodyContour.h

* The file implements a class that represents the contour of the robot's body in the image.
*
* @author Kirill Yasinovskiy
*/

#include "BodyContour.h"

void BodyContour::clipBottom(int x, int& y) const
{
  int yIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->yAt(x, yIntersection) && yIntersection < y)
      y = yIntersection;
}// end clipBottom

void BodyContour::clipTop(int x, int& y) const
{
  int yIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->yAt(x, yIntersection) && yIntersection > y)
      y = yIntersection;
}// end clipTop

void BodyContour::clipLeft(int& x, int y) const
{
  int xIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->p1.y > i->p2.y)
    {
      if(i->xAt(y, xIntersection) && xIntersection > x)
        x = xIntersection;
      else if(i->p2.y <= y && i->p2.x > x)
        x = i->p2.x;
    }// end if
}// end clipLeft

void BodyContour::clipRight(int& x, int y) const
{
  int xIntersection;
  for(std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
    if(i->p1.y < i->p2.y)
    {
      if(i->xAt(y, xIntersection) && xIntersection < x)
        x = xIntersection;
      else if(i->p1.y <= y && i->p1.x < x)
        x = i->p1.x;
    }// end if
}//end clipRight