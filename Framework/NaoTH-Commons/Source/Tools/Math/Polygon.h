/**
* @file Polygon.h
*
* @author <a href="mailto:rittercn@student.hu-berlin.de">Claas-Norman Ritter</a>
* Implementation of class Polygon
*/

#ifndef __Polygon_H_
#define __Polygon_H_

#include "Tools/Math/PointList.h"
#include "Tools/Math/Moments2.h"
#include "Tools/DataStructures/Area.h"

namespace Math {

template<int MAX_NUMBER_OF_POINTS>
class Polygon: public PointList<MAX_NUMBER_OF_POINTS>, public Area<int>
{
  public:
    Polygon() : PointList<MAX_NUMBER_OF_POINTS>() {};
    virtual ~Polygon(){}

    using PointList<MAX_NUMBER_OF_POINTS>::length;
    using PointList<MAX_NUMBER_OF_POINTS>::points;
    Moments2<1>  moments;
    using PointList<MAX_NUMBER_OF_POINTS>::add;
    using PointList<MAX_NUMBER_OF_POINTS>::clear;
    using PointList<MAX_NUMBER_OF_POINTS>::getClosestPoint;
    using PointList<MAX_NUMBER_OF_POINTS>::getMean;
    using PointList<MAX_NUMBER_OF_POINTS>::merge;
    using PointList<MAX_NUMBER_OF_POINTS>::remove;

    using PointList<MAX_NUMBER_OF_POINTS>::operator [];
    
    //returns the Area surounded by the Polygon defined by the points in the list
    //in their respective order
    double getArea() const
    {
      if(length > 2)
      {
        double area = 0;

        //    [ points[n].x                  points[n].y                ]
        // A =[                                                         ]
        //    [ points[(n + 1) % length].x   points[(n + 1) % length].y ]
        // area += det(A)
        for(int n = 0; n < length; n++)
          area += (points[n].y + points[(n + 1) % length].y) * (points[n].x - points[(n + 1) % length].x);

        return fabs(area) / 2.0;
      }
      else
        return -1;
    };//end getPolygonArea

    virtual bool isInside(const Vector2<int>& point) const
    {
      //return testCrossingNumber(point) == 1;
      return testWindingNumber(point) != 0;
    };

  private:
    // Copyright 2001, softSurfer (www.softsurfer.com)
    // This code may be freely used and modified for any purpose
    // providing that this copyright notice is included with it.
    // SoftSurfer makes no warranty for this code, and cannot be held
    // liable for any real or imagined damage resulting from its use.
    // Users of this code must verify correctness for their application.

    //    a Point is defined by its coordinates {int x, y;}
    //===================================================================

    // tests if a point is Left|On|Right of an infinite line.
    //    Input:  three points P0, P1, and P2
    //    Return: >0 for P2 left of the line through P0 and P1
    //            =0 for P2 on the line
    //            <0 for P2 right of the line
    //    See: the January 2001 Algorithm "Area of 2D and 3D Triangles and Polygons"
    inline int getRelPos(const Vector2<int>& p0, const Vector2<int>& p1, const Vector2<int>& p2 ) const
    {
        return ( (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y) );
    }
    //===================================================================

    /**
    * crossing number test for a point in a polygon
     * This code is patterned after [Franklin, 2000]
    */
    int testCrossingNumber(const Vector2<int>& p) const
    {
      int crossingNr = 0;    // the crossing number counter

      int j = length - 1;
//      bool oddNodes = false;

      for (int i = 0; i < length; i++)
      {
        if (points[i].y < p.y && points[j].y >= p.y || points[j].y < p.y && points[i].y >= p.y)
        {
          if (points[i].x + (p.y - points[i].y) / (points[j].y - points[i].y) * (points[j].x - points[i].x) < p.x)
          {
//            oddNodes = !oddNodes;
            ++crossingNr;
          }
        }
        j = i;
      }
      return (crossingNr & 1);    // 0 if even (out), and 1 if odd (in)
    }
    //===================================================================

    // winding number test for a point in a polygon
    int testWindingNumber(const Vector2<int>& p) const
    {
        int    windingNr = 0;    // the winding number counter

        int j = length - 1;

        // loop through all edges of the polygon
        for (int i = 0; i < length; i++)
        {   // edge from V[i] to V[i+1]
            if (points[j].y <= p.y)
            {         // start y <= P.y
                if (points[i].y > p.y)      // an upward crossing
                {
                    if (getRelPos( points[j], points[i], p) > 0)  // P left of edge
                    {
                        ++windingNr;            // have a valid up intersect
                    }
                }
            }
            else
            {                       // start y > P.y (no test needed)
                if (points[i].y <= p.y)     // a downward crossing
                {
                    if (getRelPos( points[j], points[i], p) < 0)  // P right of edge
                    {
                        --windingNr;            // have a valid down intersect
                    }
                }
            }
          j = i;
        }
        return windingNr;
    }
    //===================================================================

};//end class Polygon

}//end namespace Math

#endif // __Polygon_H_
