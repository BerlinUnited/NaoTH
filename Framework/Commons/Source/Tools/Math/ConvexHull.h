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
    int cross(const T &O, const T &A, const T &B)
    {
        return (int)((A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x));
    }//end cross

    template<typename T>
    bool graphical_less(const T &A, const T &B)
    {
        return A.y < B.y || (A.y == B.y && A.x < B.x);
    }//end graphical_less
 
// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
    template<typename T>
    vector<T> convexHull(vector<T> P)
    {
        int n = static_cast<int> (P.size()), k = 0;
        vector<T> H(n+1);
 
        // Sort points lexicographically
        sort(P.begin(), P.end(), graphical_less<T>);
 
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
 
        H.resize(k);
        return H;
    }

}

#endif // _CONVEX_HULL_H
