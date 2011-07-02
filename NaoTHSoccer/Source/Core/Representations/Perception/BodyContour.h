/**
* @file BodyContour.h

* The file declares a class that represents the contour of the robot's body in the image.
*
* @author Kirill Yasinovskiy
*/

#ifndef _BodyContour_h_
#define _BodyContour_h_


#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <vector>
#include "Tools/Debug/DebugBufferedOutput.h"


/**
* @class BodyContour
* A class that represents the contour of the robot's body in the image.
*/
class BodyContour : public naoth::Printable
{
private:
 
  virtual void print(ostream& stream) const
  {
    stream << "BodyContour: lines: " << '\n';
    for( vector<Line>::const_iterator iter=lines.begin();
      iter!=lines.end(); ++iter)
      {
        stream <<"Line number: " << iter->lineNumber << " First Point: " << " X coord: " << iter->p1.x << " Y coord: " << iter->p1.y << " Second Point: " << " X coord: " << iter->p2.x << " Y coord: " << iter->p2.y << " LineID: " << iter->id << '\n';
      } // end for
    }// end print

public:
  
  enum bodyPartID
  {
    Torso,
    UpperArmLeft,
    UpperArmRight,
    LowerArmLeft,
    LowerArmRight,
    LegLeft,
    LegRight,
    FootLeft,
    FootRight,
    numOfBodyPart,
  };

  struct Cell {
    int lineNumbers;
    bool occupied;
  };

  
  Vector2<int> returnCellCoord(Vector2<int>& point) const
  {
    Vector2<int> cell;
    cell.x = point.x / stepSize;
    cell.y = point.y / stepSize;
    if (cell.x == xDensity)
    {
      cell.x = cell.x - 1;
    }
    if (cell.y == yDensity)
    {
      cell.y = cell.y - 1;
    }
    if (cell.x == xDensity && cell.y == yDensity)
    {
      cell.x = cell.x - 1;
      cell.y = cell.y - 1;
    }
    return cell;
  }

  bool isOccupied(Vector2<int>& point) const
  {
    Vector2<int> temp = returnCellCoord(point);
    return grid[temp.x][temp.y].occupied;
  }

  
  Vector2<int> returnFirstFreeCell(Vector2<int> start) const
  {
    while (isOccupied(start) && start.y >= 0)
    {
      start.y -= stepSize;
    }
    return start;
  }

  std::vector<std::vector<Cell> > grid;

  /** A class representing a line in 2-D space. */
  class Line
  {
  private:
   

  public:
    
    Vector2<int> p1, /**< The left point of the line. */
                 p2; /**< The right point of the line. */
    int lineNumber; /**< The number of the Line. */
    double lineLenght; /**< The lenght of the Line. */
    bodyPartID id;

    /** Default constructor. */
    Line()
    :
      lineNumber(0),
      lineLenght(0.0),
      id(BodyContour::numOfBodyPart)
    {}

    /**
    * Constructor.
    * @param p1 The first endpoint of the line.
    * @param p2 The second endpoint of the line.
    */
    
    Line(const Vector2<int> p1, const Vector2<int> p2, int lineNumber, bodyPartID id) 
    :
      p1((p1.x < p2.x) || ((p1.x == p2.x) && (p1.y < p2.y)) ? p1 : p2),
      p2((p1.x < p2.x) || ((p1.x == p2.x) && (p1.y < p2.y)) ? p2 : p1),
      lineNumber(lineNumber),
      lineLenght(0.0),
      id(id)
    {}


    /**
    * The method determines the x coordinate of the line for a certain y coordinate 
    * if the is any.
    * @param y The y coordinate.
    * @param x The x coordinate is returned here if it exists.
    * @return Does such an x coordinate exist?
    */
    bool xAt(int y, int& x) const
    {
      if((p1.y <= y && p2.y > y) || (p1.y > y && p2.y <= y))
      {
        x = p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
        return true;
      }
      else
        return false;
    }

    /**
    * The method determines the y coordinate of the line for a certain x coordinate 
    * if the is any.
    * @param x The x coordinate.
    * @param y The y coordinate is returned here if it exists.
    * @return Does such a y coordinate exist?
    */
    bool yAt(int x, int& y) const
    {
      if(p1.x <= x && p2.x > x)
      {
        y = p1.y + (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x);
        return true;
      }
      else
        return false;
    }
    /**
    *
    *
  */
  
    inline void interpolate(Vector2<int>& point) const
    {
      point.x = (p1.x + p2.x)/2;
      point.y = (p1.y + p2.y)/2;
    }
  }; //end class Line

  std::vector<Line> lines; /** The clipping lines. */
  
  Vector2<int> cameraResolution;

  /** Default constructor. */
  BodyContour() 
  {
    lines.reserve(100);
  }

  /**
  * The method clips the bottom y coordinate of a vertical line.
  * @param x The x coordinate of the vertical line.
  * @param y The original y coordinate of the bottom of the vertical line.
  *          It will be replaced if necessary. Note that the resulting point
  *          can be outside the image!
  */
  void clipBottom(int x, int& y) const;

  /**
  * The method clips the top y coordinate of a vertical line.
  * @param x The x coordinate of the vertical line.
  * @param y The original y coordinate of the top of the vertical line.
  *          It will be replaced if necessary. Note that the resulting point
  *          can be outside the image!
  */
  void clipTop(int x, int& y) const;

  /**
  * The method clips the left x coordinate of a horizontal line.
  * It only considers descending clipping lines.
  * @param x The original x coordinate of the left end of the horizontal line.
  *          It will be replaced if necessary. Note that the resulting point
  *          can be outside the image!
  * @param y The y coordinate of the horizontal line.
  */
  void clipLeft(int& x, int y) const;

  /**
  * The method clips the right x coordinate of a horizontal line.
  * It only considers ascending clipping lines.
  * @param x The original x coordinate of the right end of the horizontal line.
  *          It will be replaced if necessary. Note that the resulting point
  *          can be outside the image!
  * @param y The y coordinate of the horizontal line.
  */
  void clipRight(int& x, int y) const;

  int stepSize;
  int xDensity;
  int yDensity;
};
#endif //_BodyContour_h_