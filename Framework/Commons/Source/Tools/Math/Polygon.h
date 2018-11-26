/**
* @file Polygon.h
*
* @author <a href="mailto:rittercn@student.hu-berlin.de">Claas-Norman Ritter</a>
* Implementation of class Polygon
*/

#ifndef _Polygon_H_
#define _Polygon_H_

#include "Tools/DataStructures/Area.h"
#include <vector>

namespace Math {

template<typename T>
class Polygon: public Area<T>
{
  private:
    std::vector<Vector2<T> > points;

  public:
    void add(const Vector2<T>& p) {
      points.push_back(p);
    }

    void add(const T x, const T y) {
      points.emplace_back(x,y);
    }

    void clear() {
      points.clear();
    }

    size_t size() const {
      return points.size();
    }

    Vector2<T>& operator[] (int i) { return points[i]; }
    const Vector2<T>& operator[] (int i) const { return points[i]; }

  public:
    Polygon() {}
    virtual ~Polygon(){}
    
    //returns the Area surounded by the Polygon defined by the points in the list
    //in their respective order
    double getArea() const {
      return getArea(points);
    }

    template <typename P>
    static double getArea(const std::vector<Vector2<P> >& points)
    {
      if(points.size() > 2)
      {
        double area = 0;

        //    [ points[n].x                  points[n].y                ]
        // A =[                                                         ]
        //    [ points[(n + 1) % length].x   points[(n + 1) % length].y ]
        // area += det(A)
        for(size_t n = 0; n < points.size(); n++) {
          area += (points[n].y + points[(n + 1) % points.size()].y) * (points[n].x - points[(n + 1) % points.size()].x);
        }

        return fabs(area) / 2.0;
      }
      else
        return -1;
    }

    virtual bool isInside(const Vector2<T>& point) const {
      //return testCrossingNumber(point) == 1;
      return testWindingNumber(points, point) != 0;
    }

    bool isInside_inline(const Vector2<T>& point) const {
      return testWindingNumber(points, point) != 0;
    }


    template<typename P>
    static inline bool isInside(const std::vector<Vector2<P> >& points, const Vector2<T>& point) {
      return testWindingNumber(points, point) != 0;
    }

  private:
    // TODO: the code below is a simple hessial form for a line
    //       it's public domain and no copyright for it can be claimed
    //
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
    inline static T getRelPos(const Vector2<T>& p0, const Vector2<T>& p1, const Vector2<T>& p2 ) {
        return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
    }
    //===================================================================

    /**
    * crossing number test for a point in a polygon
     * This code is patterned after [Franklin, 2000]
    */
    // NOTE: this code is not used, do we need it?
    /*
    template <typename P>
    static int testCrossingNumber(const std::vector<Vector2<P> >& points, const Vector2<T>& p)
    {
      int crossingNr = 0;    // the crossing number counter

      int j = size() - 1;
//      bool oddNodes = false;

      for (size_t i = 0; i < points.size(); i++)
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
    */
    //===================================================================

    // winding number test for a point in a polygon
    template <typename P>
    static int testWindingNumber(const std::vector<Vector2<P> >& points, const Vector2<T>& p)
    {
        int windingNr = 0;    // the winding number counter

        int j = points.size() - 1;

        // loop through all edges of the polygon
        for (size_t i = 0; i < points.size(); i++)
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

#endif // _Polygon_H_
