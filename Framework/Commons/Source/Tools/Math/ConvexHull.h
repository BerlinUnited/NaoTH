// Implementation of Monotone Chain Convex Hull algorithm.
// http://www.algorithmist.com/index.php/Monotone_Chain_Convex_Hull

#ifndef _CONVEX_HULL_H
#define _CONVEX_HULL_H

#include <algorithm>
#include <vector>

namespace ConvexHull 
{
  using namespace std;
 
  // 2D cross product.
  // Return a positive value, if OAB makes a counter-clockwise turn,
  // negative for clockwise turn, and zero if the points are collinear.
  template<typename T>
  double cross(const T &O, const T &A, const T &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
  }

  /*
  // NOTE: this might be a better solution to avoid variable casting
  template<typename T>
  bool cross_negative(const T &O, const T &A, const T &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x) <= 0;
  }
  */

  /*
  template<typename T>
  bool graphical_less(const T &A, const T &B) {
      return A.y < B.y || (A.y == B.y && A.x < B.x);
  }
  */

  template<typename T>
  struct Less {
    bool operator()(const T& A, const T& B) const {   
        return A.y < B.y || (A.y == B.y && A.x < B.x);
    }   
  };

  // Returns a list of points on the convex hull in counter-clockwise order.
  // NOTE: the last point in the returned list is the same as the first one.
  template<typename T, class ComperatorType = Less<T> >
  void convexHull(vector<T> points, vector<T>& result)
  {
    //Anmerkung: mit 3 oder weniger punkten haben
    //wir den trivialen Fall
    if(points.size() < 2)
    {
      // Check if we don't have to do anything and provide a (non-crashing) fallback in case the static_cast<int>
      // did indeed overflow and i is less than zero.
      result = points;
      return;
    }

    // Worst-case scenario is that an value is assgined to n*2 points at least temporary
    //vector<T> result(n*2);
    result.resize(points.size()*2);
 
    // Sort points lexicographically
    sort(points.begin(), points.end(), ComperatorType());
 
    size_t k = 0;

    // Build lower hull
    for (size_t i = 0; i < points.size(); i++) {
      while (k >= 2 && cross(result[k-2], result[k-1], points[i]) <= 0) k--;
      result[k++] = points[i];
    }

    // Build upper hull
    for (size_t i = points.size()-1, t = k+1; i > 0; i--) {
      while (k >= t && cross(result[k-2], result[k-1], points[i-1]) <= 0) k--;
      result[k++] = points[i-1];
    }

    // NOTE: This Algorithm resiszes of the size k. The first and the last element are Equal. 
    //       Some algorithm implementations cut that last element off but we need it for the field polygon.
    result.resize(k);
  }


  template<typename T, class ComperatorType = Less<T> >
  vector<T> convexHull(vector<T> points)
  {
    vector<T> result;
    convexHull(points, result);
    return result;
  }

}

#endif // _CONVEX_HULL_H
