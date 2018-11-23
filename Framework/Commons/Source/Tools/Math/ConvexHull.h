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
  // Note: the last point in the returned list is the same as the first one.
  template<typename T, class ComperatorType = Less<T> >
  vector<T> convexHull(vector<T> P)
  {
    // TODO: guard against overflow with std::limits<int>
    // TODO: this should be completly changed 2017.07.14
    int n = static_cast<int> (P.size());
    int k = 0;

    //Anmerkung: mit 3 oder weniger punkten haben
    //wir den trivialen Fall
    if(n < 2)
    {
      // Check if we don't have to do anything and provide a (non-crashing) fallback in case the static_cast<int>
      // did indeed overflow and i is less than zero.
      return P;
    }

    // Worst-case scenario is that an value is assgined to n*2 points at least temporary
    vector<T> H(n*2);
 
    // Sort points lexicographically
    sort(P.begin(), P.end(), ComperatorType());
 
    // Build lower hull
    for (int i = 0; i < n; i++) {
      while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
      H[k++] = P[i];
    }

    // Build upper hull
    for (int i = n-2, t = k+1; i >= 0; i--){
      while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
      H[k++] = P[i];
    }

	//NOTE: This Algorithm resiszes of the size k. The first and the last element are Equal. 
	//Some algorithm implementations cut that last element off but we need it for the field polygon
    H.resize(k);
    return H;
  }

}

#endif // _CONVEX_HULL_H
