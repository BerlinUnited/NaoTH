/**
 * @file h
 *
 * Declaration of class LinesTable
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef __LinesTable_h_
#define __LinesTable_h_

#include <limits>
#include <vector>

#include "Core/Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Math/Line.h"

class LinesTable
{
private:

  static const int ySize = 20;
  static const int xSize = 30;
  double yWidth;
  double xWidth;

  Vector2<double> closestPoints[xSize][ySize];

  Vector2<double> closestCornerPoints[xSize][ySize];
  Vector2<double> closestTCrossings[xSize][ySize];

  enum
  {
    maxNumberOfEntries = 106 /**< The maximum number of lines in the table. */
  };

public:

  Math::LineSegment lines[maxNumberOfEntries];
  std::vector<Math::Intersection> intersections;

  int numberOfEntries; /**< The number of corners in the table. */

  LinesTable() :
  numberOfEntries(0)
  {
  }

  ~LinesTable()
  {
  }

  void push(const Vector2<double>& begin, const Vector2<double>& end)
  {
    //ASSERT(numberOfEntries < maxNumberOfEntries);
    if (numberOfEntries < maxNumberOfEntries)
    {
      lines[numberOfEntries++] = Math::LineSegment(begin, end);
    }
  }//end push


  const Math::LineSegment& operator[] (unsigned idx) const
  {
    return lines[idx];
  }


  Math::LineSegment getNearestLine(const Pose2D& pose) const
  {
    return getNearestLine(pose.translation);
  }//end getNearestLine


  const Math::LineSegment& getNearestLine(const Vector2<double>& point) const
  {
    ASSERT(numberOfEntries > 0);

    double minDistance = std::numeric_limits<double>::infinity();
    int minIdx = 0;

    for (int i = 0; i < numberOfEntries; i++)
    {
      // calculate the distance :)
      double dist = lines[i].minDistance(point);

      if (dist >= 0 && dist <= minDistance)
      {
        minDistance = dist;
        minIdx = i;
      }
    }//end for

    return lines[minIdx];
  }//end getNearestLine

  const Math::Intersection& getNearestIntersection(const Vector2<double>& point) const
  {
    ASSERT(intersections.size() > 0);

    double minDistance = std::numeric_limits<double>::infinity();
    unsigned int minIdx = 0;

    for (unsigned int i = 0; i < intersections.size(); i++)
    {
      // calculate the distance :)
      double dist = (intersections[i].pos - point).abs();

      if (dist >= 0 && dist <= minDistance)
      {
        minDistance = dist;
        minIdx = i;
      }
    }//end for

    return intersections[minIdx];
  }//end getNearestIntersection

  const Math::Intersection& getNearestTCrossing(const Vector2<double>& point) const
  {
    ASSERT(intersections.size() > 0);

    double minDistance = std::numeric_limits<double>::infinity();
    unsigned int minIdx = 0;

    for (unsigned int i = 0; i < intersections.size(); i++)
    {
      //is a T or X crossing
      bool isXorT = ((intersections[i].type == Math::Intersection::T) ||(intersections[i].type == Math::Intersection::X));

      if(!isXorT)
          continue;
      // calculate the distance :)
      double dist = (intersections[i].pos - point).abs();

      if (dist >= 0 && dist <= minDistance)
      {
        minDistance = dist;
        minIdx = i;
      }
    }//end for

    return intersections[minIdx];
  }//end getNearestTCrossing

  void findIntersections()
  {
    for (int i = 0; i < numberOfEntries; i++)
    {
      for (int j = i + 1; j < numberOfEntries; j++)
      {
        Math::Intersection intersection(lines[i], lines[j]);
        if (intersection.type != Math::Intersection::none)
        {
          intersections.push_back(intersection);
        }
      }//end for
    }//end for
  }//end findIntersections

  /**
   * Draws a debug field drawing that displays the set of lines.
   */
  void draw() const
  {
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 20);
    for (int i = 0; i < numberOfEntries; i++)
    {
      LINE(lines[i].begin().x, lines[i].begin().y,
        lines[i].end().x, lines[i].end().y);
    }//end for


    PEN("FF0000", 20);
    for (unsigned int i = 0; i < intersections.size(); i++)
    {
      FILLOVAL(intersections[i].pos.x, intersections[i].pos.y, 20, 20);
      switch(intersections[i].type)
      {
        case Math::Intersection::L:
          TEXT_DRAWING(intersections[i].pos.x+30, intersections[i].pos.y+30,"L");
          break;
        case Math::Intersection::T:
          TEXT_DRAWING(intersections[i].pos.x+30, intersections[i].pos.y+30,"T");
          break;
        case Math::Intersection::X:
          TEXT_DRAWING(intersections[i].pos.x+30, intersections[i].pos.y+30,"X");
          break;
        default: // shouldn't happen
          TEXT_DRAWING(intersections[i].pos.x+30, intersections[i].pos.y+30,"Unknown krossing type");
      }//end switch
    }
  }//end draw



  void create_colosetPoinsTable(double fieldWidth, double fieldLength)
  {
    yWidth = 0.5*fieldLength/(double)ySize;
    xWidth = 0.5*fieldWidth/(double)xSize;

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2<double> point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));

        const Math::LineSegment& line = getNearestLine(point);
        closestPoints[x][y] = line.projection(point);
      }//end for
    }//end for
  }//end create_colosetPoinsTable


  void create_colosetCornerPoinsTable(double fieldWidth, double fieldLength)
  {
    yWidth = 0.5*fieldLength/(double)ySize;
    xWidth = 0.5*fieldWidth/(double)xSize;

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2<double> point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
        const Math::Intersection& corner = getNearestIntersection(point);
        const Math::Intersection& xort = getNearestTCrossing(point);
        closestCornerPoints[x][y] = corner.pos;
        closestTCrossings[x][y] = xort.pos;
      }//end for
    }//end for
  }//end create_colosetCornerPoinsTable


  Vector2<double> get_closest_corner_point(const Vector2<double>& p) const
  {
    // allready prepared for rounding, i.e. +0.5
    double x = (p.x/xWidth + xSize)*0.5;
    double y = (p.y/yWidth + ySize)*0.5;
    
    if(x > 0 && x < xSize && y > 0 && y < ySize )
      return closestCornerPoints[(int)x][(int)y];
    else
      // TODO: make it better (point outside the field)
      return Vector2<double>(10000,10000);
  }//end get_closest_point


  Vector2<double> get_closest_tcrossing_point(const Vector2<double>& p) const
  {
    // allready prepared for rounding, i.e. +0.5
    double x = (p.x/xWidth + xSize)*0.5;
    double y = (p.y/yWidth + ySize)*0.5;

    if(x > 0 && x < xSize && y > 0 && y < ySize )
      return closestTCrossings[(int)x][(int)y];
    else
      // TODO: make it better (point outside the field)
      return Vector2<double>(10000,10000);
  }//end get_closest_tcrossing_point

  Vector2<double> get_closest_point(const Vector2<double>& p) const
  {
    // allready prepared for rounding, i.e. +0.5
    double x = (p.x/xWidth + xSize)*0.5;
    double y = (p.y/yWidth + ySize)*0.5;
    
    if(x > 0 && x < xSize && y > 0 && y < ySize )
      return closestPoints[(int)x][(int)y];
    else
      // TODO: make it better (point outside the field)
      return Vector2<double>(10000,10000);
  }//end get_closest_point


  void draw_closest_points() const
  {
    FIELD_DRAWING_CONTEXT;
    DebugDrawings::Color white(1.0,1.0,1.0);
    DebugDrawings::Color black(0.0,0.0,0.0);

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2<double> point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
        double d = (point - closestPoints[x][y]).abs();
        double t = Math::clamp(d/(yWidth*ySize),0.0,1.0);
        DebugDrawings::Color color = white*t + black*(1-t);
        PEN(color, 20);
        FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
      }//end for
    }//end for
  }//end draw_closest_points



  void draw_closest_corner_points() const
  {
    FIELD_DRAWING_CONTEXT;
    DebugDrawings::Color white(1.0,1.0,1.0);
    DebugDrawings::Color black(0.0,0.0,0.0);

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2<double> point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
        double d = (point - closestCornerPoints[x][y]).abs();
        double t = Math::clamp(d/(yWidth*ySize),0.0,1.0);
        DebugDrawings::Color color = white*t + black*(1-t);
        PEN(color, 20);
        FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
      }//end for
    }//end for
  }//end draw_closest_points

};//end class LinesTable





#endif //__LinesTable_h_
