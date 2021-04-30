/**
* @file Polygon.h
*
* @author <a href="mailto:rittercn@student.hu-berlin.de">Claas-Norman Ritter</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class Polygon
*/

#ifndef _Polygon_H_
#define _Polygon_H_

#include "Tools/Math/Vector2.h"

#include "ConvexHull.h"

#include <vector>

namespace Math {

template<typename T>
class Polygon
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

    Vector2<T>& operator[] (size_t i) { return points[i]; }
    const Vector2<T>& operator[] (size_t i) const { return points[i]; }

  public:
    Polygon() {}

    Polygon(const std::vector<Vector2<T> >& point_set) {
      makeFromPointSet(point_set);
    }

    // returns the Area surounded by the Polygon defined by the points in the list
    // in their respective order
    double getArea() const {
      return getArea(points);
    }

    const std::vector<Vector2<T>>& getPoints()  const {
      return points;
    }

    void makeFromPointSet(const std::vector<Vector2<T> >& point_set) {
      ConvexHull::convexHull(point_set, points);
    }

    // NOTE: the returned value is 0 if points.size() <= 2.
    template <typename P>
    static double getArea(const std::vector<Vector2<P> >& points)
    {
      //    [ points[n].x                  points[n].y                ]
      // A =[                                                         ]
      //    [ points[(n + 1) % length].x   points[(n + 1) % length].y ]
      // area += det(A)
      double area = 0;
      for(size_t n = 0; n < points.size(); n++) {
        area += (points[n].y + points[(n + 1) % points.size()].y) * (points[n].x - points[(n + 1) % points.size()].x);
      }

      return fabs(area) / 2.0;
    }

    inline bool isInside(const Vector2<T>& point) const {
      return testWindingNumber(points, point) != 0;
    }

    template<typename P>
    static inline bool isInside(const std::vector<Vector2<P> >& points, const Vector2<T>& point) {
      return testWindingNumber(points, point) != 0;
    }

  private:
    // Calculate the signed distance of the point 'p' to the line defined by the points 'a' and 'b'.
    //
    // Input:  three points a, b, and p
    // Return: >0 for p left of the line through a and b
    //         =0 for p on the line
    //         <0 for p right of the line
    inline static T getRelPos(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& p ) {
        return (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
    }

    //
    // calculate the winding number test for a point in a polygon
    //
    template <typename P>
    static int testWindingNumber(const std::vector<Vector2<P> >& points, const Vector2<T>& p)
    {
        // the counter for the winding number
        int windingNr = 0;

        // NOTE: j points to the the last element.
        //       If points.size() == 0 the loop below doesnt do anything
        //       and j is not used.
        size_t j = points.size() - 1;

        // loop through all edges of the polygon
        for (size_t i = 0; i < points.size(); i++)
        {
            // consider edge from V[i] to V[i+1]
            if (points[j].y <= p.y) {                               // start y <= P.y
                if (points[i].y > p.y) {                            // an upward crossing
                    if (getRelPos( points[j], points[i], p) > 0) {  // P left of the edge
                        ++windingNr;                                // have a valid up intersect
                    }
                }
            } else {                                                // start y >= P.y (no test needed)
                if (points[i].y <= p.y) {                           // a downward crossing
                    if (getRelPos( points[j], points[i], p) < 0) {  // P right of the edge
                        --windingNr;                                // have a valid down intersect
                    }
                }
            }
            j = i;
        }

        return windingNr;
    }

};//end class Polygon

}//end namespace Math

#endif // _Polygon_H_
