/**
* @file PointList.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class PointList
*/

#ifndef __PointList_H_
#define __PointList_H_

#include "Tools/Math/Vector2.h"
#include "Tools/Debug/NaoTHAssert.h"

template<int MAX_NUMBER_OF_POINTS>
class PointList
{
  public:
    PointList() : length(0)
    {
      for(int i=0; i < MAX_NUMBER_OF_POINTS; i++)
      {
        points[i].x = 0;
        points[i].y = 0;
      }
    };

    Vector2<int> points[MAX_NUMBER_OF_POINTS]; /**< The points. */
    int length; /**< The actual number of points. */

    void add(const Vector2<int>& point)
    {
      if(length < MAX_NUMBER_OF_POINTS)
      {
        points[length++] = point;
      }//end if
    }//end add

    void clear()
    {
      length = 0;
    }//end clear

    //removes a point from the list (this destroys any given order in the list)
    void remove(int index)
    {
      if(index<length && index>=0)
      {
        points[index]=points[length-1];
        length--;
      }
    }//end remove

    //concatenates another list with the given one
    void merge(const PointList& other)
    {
      for(int n=0; n<other.length; n++)
        add(other[n]);
    }//end merge

    //returns the point in the list closest to the reference point
    const Vector2<int>& getClosestPoint(const Vector2<int>& reference) const
    {
      //if(length==0) return null;

      double minDistance=((Vector2<double>)(points[0]-reference)).abs();
      int closest = 0;

      for(int i = 1; i < length; i++)
      {
        double tmpDist = ((Vector2<double>)(points[i]-reference)).abs();
        if(tmpDist < minDistance)
        {
          minDistance = tmpDist;
          closest = i;
        }
      }//end for

      return points[closest];
    }//end getClosestPoint


    //returns the mean point
    Vector2<double> getMean() const
    {
      Vector2<double> mean;

      for(int i = 0; i < length; i++)
      {
        mean += points[i];
      }//end for
      mean /= (double)length;

      return mean;
    }//end getMean

    Vector2<int>& operator[] (int i) { ASSERT(i>=0&&i<MAX_NUMBER_OF_POINTS); return points[i]; }
    const Vector2<int>& operator[] (int i) const { ASSERT(i>=0&&i<MAX_NUMBER_OF_POINTS); return points[i]; }
    size_t size() const { return length; }
};//end class pointList


#endif // __PointList_H_
