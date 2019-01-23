/**
 * @file h
 *
 * Declaration of class LinesTable
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _LinesTable_h_
#define _LinesTable_h_

#include <limits>
#include <vector>

#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Color.h"
#include "Tools/Debug/NaoTHAssert.h"
#include "Tools/Math/Line.h"


//NOTE: this should only be used with field lines.
// LineIntersection is meant to describe intersections between field lines but not in general mathematical sense.
// The type of an intersection only makes sense for field lines.
class LineIntersection
{
public:
  enum Type
  {
    unknown, //equals ColorClasses::none
    T, //1 line intersect an other, equals ColorClasses::orange
    L, //2 lines touch, equals ColorClasses::yellow
    C, //lines intersect on/with circle, equals ColorClasses::skyblue
    E, //line extends each other, equals ColorClasses::white
    X, //2 intersect each other, equals ColorClasses::red
    none //the line segments don't intersect
  };

  LineIntersection(const Math::LineSegment& segmentOne, const Math::LineSegment& segmentTwo)
    //: segmentOne(segmentOne),
    //  segmentTwo(segmentTwo)
  {
    double s = segmentOne.Line::intersection(segmentTwo);
    double t = segmentTwo.Line::intersection(segmentOne);
    
    double angleDiff = fabs(Math::normalize(segmentOne.getDirection().angle() - segmentTwo.getDirection().angle()));
    angleDiff = std::min(angleDiff, Math::pi - angleDiff);

    if(segmentOne.getLength() < s || s < 0 || 
       segmentTwo.getLength() < t || t < 0 ||
       angleDiff < Math::pi_2 - 0.85 || 
       angleDiff > Math::pi_2 + 0.85 ) // 0.85 = 5deg
      type = none;
    else if(angleDiff > 0.85 * 2.0 && angleDiff < Math::pi_2 - 0.85 * 2.0 )
      type = C;
    else if(0 < s && s < segmentOne.getLength() && 0 < t && t < segmentTwo.getLength())
      type = X;
    else if((0 < s && s < segmentOne.getLength()) || (0 < t && t < segmentTwo.getLength()))
      type = T;
    else
      type = L;

    pos = segmentOne.point(s);
  }

  Type type;
  Vector2i pos; /**< The fieldcoordinates of the intersection */
  //Math::LineSegment segmentOne;
  //Math::LineSegment segmentTwo;
};


class LinesTable
{
public:
   
  // HACK: it could be done with int but then we don't have a special type
  enum LinesTableType
  {
    long_lines    =  1,
    short_lines   =  2,
    along_lines   =  4,
    across_lines  =  8,
    circle_lines  = 16,
    numberOfLinesTableType = 5, 
    all_lines     =  long_lines | short_lines | along_lines | across_lines | circle_lines
  };

  /*
  line_type[0] = long_lines | along_lines;
  line_type[1] = long_lines | across_lines;
  line_type[2] = short_lines | along_lines;
  line_type[3] = short_lines | across_lines;
  line_type[4] = circle_lines;*/
  enum LinesTableIndex
  {
    idx_long_along    =  0,
    idx_long_across   =  1,
    idx_short_along   =  2,
    idx_short_across  =  3,
    idx_circle        =  4,
    numberOfLinesTableIndex = 5
  };

  class NamedPoint
  {
  public:
    NamedPoint() : id(-1) {}
    Vector2d position;
    int id;
  };

private:

  static const int ySize = 20;
  static const int xSize = 30;

  // the size of a grid cell is (2*xWidth) x (2*yWidth)
  double yWidth;
  double xWidth;

  int line_type[numberOfLinesTableType];
  //static const int numberOfTables;
  /**
    long_lines & along_lines
    long_lines & across_lines
    short_lines & along_lines
    short_lines & across_lines
    circle_lines
  */

  /** table of the line points */
  NamedPoint closestPoints[xSize][ySize][numberOfLinesTableType];

  /** table of typical combinations for fast access,
  i.e., short|across, long|across, short|along, long|along*/
  NamedPoint typicalClosestPoints[xSize][ySize][4];

  /** table of all corner points */
  NamedPoint closestCornerPoints[xSize][ySize];

  /** table of the crossings (without L-Corners) */
  NamedPoint closestTCrossings[xSize][ySize];

  /** list of lines */
  std::vector<Math::LineSegment> lines;

  /** list of intersections between the lines */
  std::vector<LineIntersection> intersections;

public:

  // HACK: remove it
  double circle_radius;
  double penalty_area_width;

  LinesTable()
  {
    circle_radius = 600.0;
    penalty_area_width = 600.0;

    line_type[idx_long_along]   = long_lines  | along_lines;
    line_type[idx_long_across]  = long_lines  | across_lines;
    line_type[idx_short_along]  = short_lines | along_lines;
    line_type[idx_short_across] = short_lines | across_lines; // actualy, there are currently no such lines except on the circle 
    line_type[idx_circle]       = circle_lines;
  }

  ~LinesTable()
  {
  }

  void addLine(const Vector2d& begin, const Vector2d& end)
  {
    lines.push_back(Math::LineSegment(begin, end));
  }

  /** some getter */
  const std::vector<Math::LineSegment>& getLines() const { return lines; }
  const std::vector<LineIntersection>& getIntersections() const { return intersections; }
  const Math::LineSegment& operator[] (unsigned idx) const { return lines[idx]; }

  /** calculate the nearest line among all lines*/
  Math::LineSegment getNearestLine(const Pose2D& pose) const
  {
    return lines[getNearestLine(pose.translation, all_lines)];
  }


  /**
   type - list of constraints of the request
   */
  int getNearestLine(const Vector2d& point, int type = all_lines) const
  {
    ASSERT(lines.size() > 0);

    double minDistance = std::numeric_limits<double>::infinity();
    int minIdx = -1;

    for (size_t i = 0; i < lines.size(); i++)
    {
      Vector2d direction = lines[i].getDirection();
      direction.normalize();

      // determine the type of line
      int length_type    = (lines[i].getLength() > penalty_area_width + 100)? long_lines  :short_lines;
      int direction_type = (fabs(direction.x) > fabs(direction.y))          ? along_lines :across_lines;
      int line_type      = (lines[i].getBase().abs() < circle_radius + 100) ? circle_lines:(length_type|direction_type);

      if((type & line_type) != line_type) {
        continue;
      }

      // calculate the distance :)
      double dist = lines[i].minDistance(point);

      if (dist >= 0 && dist <= minDistance) {
        minDistance = dist;
        minIdx = i;
      }
    }//end for

    return minIdx;
  }//end getNearestLine

  int getNearestIntersection(const Vector2d& point) const
  {
    ASSERT(intersections.size() > 0);

    double minDistance = std::numeric_limits<double>::infinity();
    unsigned int minIdx = 0;

    for (size_t i = 0; i < intersections.size(); i++)
    {
      //NOTE: use the square distance because it's faster to calculate
      double dist = (intersections[i].pos - point).abs2();

      if (dist >= 0 && dist <= minDistance)
      {
        minDistance = dist;
        minIdx = i;
      }
    }//end for

    return minIdx;
  }//end getNearestIntersection

  int getNearestTCrossing(const Vector2d& point) const
  {
    ASSERT(intersections.size() > 0);

    double minDistance = std::numeric_limits<double>::infinity();
    unsigned int minIdx = 0;

    for (size_t i = 0; i < intersections.size(); i++)
    {
      //is a T or X crossing
      bool isXorT = ((intersections[i].type == LineIntersection::T) || (intersections[i].type == LineIntersection::X));

      if(!isXorT) {
          continue;
      }

      // calculate the distance :)
      double dist = (intersections[i].pos - point).abs2();

      if (dist >= 0 && dist <= minDistance)
      {
        minDistance = dist;
        minIdx = i;
      }
    }//end for

    return minIdx;
  }//end getNearestTCrossing


  void findIntersections()
  {
    for (size_t i = 0; i < lines.size(); i++)
    {
      for (size_t j = i + 1; j < lines.size(); j++)
      {
        LineIntersection intersection(lines[i], lines[j]);
        if (intersection.type != LineIntersection::none) {
          intersections.push_back(intersection);
        }
      }//end for
    }//end for
  }//end findIntersections


  // NOTE: this is experimental - a general method to calculate intersections
  void calculateIntersections() 
  {
  }

  void estimateIntersection(const Math::LineSegment& segmentOne, const Math::LineSegment& segmentTwo) 
  {
    const double maxAngleDiff = 0.85;
    const double maxDistanceToIntersection = 100.0; // 10cm

    double t1 = segmentOne.Line::intersection(segmentTwo);
    double t2 = segmentTwo.Line::intersection(segmentOne);

    //---------o=====[===========]=====o-------------
    bool insideSegmentOne = maxDistanceToIntersection < t1 && t1 < segmentOne.getLength() - maxDistanceToIntersection;
    bool insideSegmentTwo = maxDistanceToIntersection < t2 && t2 < segmentTwo.getLength() - maxDistanceToIntersection;

    //---[-----o=======================o-----]-------
    bool nearSegmentOne = -maxDistanceToIntersection < t1 && t1 < segmentOne.getLength() + maxDistanceToIntersection;
    bool nearSegmentTwo = -maxDistanceToIntersection < t2 && t2 < segmentTwo.getLength() + maxDistanceToIntersection;


    double angleDiff = acos(segmentOne.getDirection()*segmentTwo.getDirection());

    LineIntersection::Type type = LineIntersection::none;

    if(!nearSegmentOne || !nearSegmentOne) { // there is no meaningful intersection
      type = LineIntersection::none;
    } else if( insideSegmentOne && insideSegmentTwo) {
      type = LineIntersection::X;
    } else if(insideSegmentOne || insideSegmentTwo) {
      type = LineIntersection::T;
    } else {
      type = LineIntersection::L;
    }

    // check the angle of the intersection
    if( angleDiff < maxAngleDiff || Math::pi - maxAngleDiff < angleDiff) {
      if(type == LineIntersection::L) {
        type = LineIntersection::E;
      }
    } else if (angleDiff < 2.0*maxAngleDiff || Math::pi - 2.0*maxAngleDiff < angleDiff) {
      if(type == LineIntersection::L) {
        type = LineIntersection::C;
      }
    } else if( angleDiff < Math::pi_2 - maxAngleDiff || Math::pi_2 + maxAngleDiff < angleDiff) { // not right angle => weird angle
      type = LineIntersection::unknown;
    }
  }

  /**
   * Draws a debug field drawing that displays the set of lines.
   */
  void draw(DrawingCanvas2D& canvas) const
  {
    canvas.pen("0000FF", 20);
    for (size_t i = 0; i < lines.size(); ++i) {
      canvas.drawLine(lines[i].begin().x, lines[i].begin().y,
                      lines[i].end().x, lines[i].end().y);
    }


    canvas.pen("FF0000", 20);
    for (size_t i = 0; i < intersections.size(); ++i)
    {
      canvas.fillOval(intersections[i].pos.x, intersections[i].pos.y, 20, 20);
      switch(intersections[i].type)
      {
        case LineIntersection::L:
          canvas.drawText(intersections[i].pos.x+30, intersections[i].pos.y+30,"L");
          break;
        case LineIntersection::T:
          canvas.drawText(intersections[i].pos.x+30, intersections[i].pos.y+30,"T");
          break;
        case LineIntersection::X:
          canvas.drawText(intersections[i].pos.x+30, intersections[i].pos.y+30,"X");
          break;
        default: // shouldn't happen
          canvas.drawText(intersections[i].pos.x+30, intersections[i].pos.y+30,"Unknown krossing type");
      }//end switch
    }
  }//end draw



  void create_closestPointsTable(double fieldWidth, double fieldLength)
  {
    yWidth = 0.5*fieldLength/(double)(ySize-1);
    xWidth = 0.5*fieldWidth/(double)(xSize-1);

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));

        for(int i = 0; i < numberOfLinesTableType; i++)
        {
          const int idx = getNearestLine(point, line_type[i]);
          closestPoints[x][y][i].id = idx;
          if(idx != -1) {
            closestPoints[x][y][i].position = lines[idx].projection(point);
          } else {
            closestPoints[x][y][i].position = point;
          }
        }
      }//end for
    }//end for
  }//end create_closestPoinsTable


  void create_closestCornerPoinsTable(double fieldWidth, double fieldLength)
  {
    yWidth = 0.5*fieldLength/(double)(ySize-1);
    xWidth = 0.5*fieldWidth/(double)(xSize-1);

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));

        const int idx_corner = getNearestIntersection(point);
        closestCornerPoints[x][y].position = intersections[idx_corner].pos;
        closestCornerPoints[x][y].id = idx_corner;

        const int idx_xort = getNearestTCrossing(point);
        closestTCrossings[x][y].position = intersections[idx_xort].pos;
        closestTCrossings[x][y].id = idx_xort;
      }//end for
    }//end for
  }//end create_closestCornerPoinsTable


  const NamedPoint& get_closest_corner_point(const Vector2d& p) const
  {
    // allready prepared for rounding, i.e. +0.5
    int x = Math::clamp((int)((p.x/xWidth + xSize)*0.5),0,xSize-1);
    int y = Math::clamp((int)((p.y/yWidth + ySize)*0.5),0,ySize-1);
    
    //ASSERT(x >= 0 && x < xSize && y >= 0 && y < ySize );
    return closestCornerPoints[x][y];
  }//end get_closest_point


  const NamedPoint& get_closest_tcrossing_point(const Vector2d& p) const
  {
    int x = Math::clamp((int)((p.x/xWidth + xSize)*0.5),0,xSize-1);
    int y = Math::clamp((int)((p.y/yWidth + ySize)*0.5),0,ySize-1);
    
    //ASSERT(x >= 0 && x < xSize && y >= 0 && y < ySize );
    return closestTCrossings[x][y];
  }//end get_closest_tcrossing_point


  const NamedPoint& get_closest_point_direct(const Vector2d& p, LinesTableIndex idx) const
  {
    ASSERT(idx >= 0 && idx < numberOfLinesTableIndex);

    // allready prepared for rounding, i.e., +0.5
    int x = Math::clamp((int)((p.x/xWidth + xSize)*0.5),0,xSize-1);
    int y = Math::clamp((int)((p.y/yWidth + ySize)*0.5),0,ySize-1);

    return closestPoints[x][y][idx];
  }//end get_closest_point_direct


  const NamedPoint& get_closest_line_point_fast(const Vector2d& p, int length, int direction) const
  {
    ASSERT((length&short_lines)|(length&long_lines));
    ASSERT((across_lines&direction)|(along_lines&direction));
    /*
    line_type[0] = long_lines | along_lines;
    line_type[1] = long_lines | across_lines;
    line_type[2] = short_lines | along_lines;
    line_type[3] = short_lines | across_lines;
    */
    unsigned int idx = ((length&short_lines) != 0)*2 + ((across_lines&direction) != 0);
    ASSERT(idx < 4);

    // allready prepared for rounding, i.e., +0.5
    int x = Math::clamp((int)((p.x/xWidth + xSize)*0.5),0,xSize-1);
    int y = Math::clamp((int)((p.y/yWidth + ySize)*0.5),0,ySize-1);

    return closestPoints[x][y][idx];
  }//end get_closest_point_fast


  NamedPoint get_closest_point(const Vector2d& p, int type = all_lines) const
  {
    // allready prepared for rounding, i.e., +0.5
    int x = Math::clamp((int)((p.x/xWidth + xSize)*0.5),0,xSize-1);
    int y = Math::clamp((int)((p.y/yWidth + ySize)*0.5),0,ySize-1);
    
    //ASSERT(x >= 0 && x < xSize && y >= 0 && y < ySize );

    double minDist = std::numeric_limits<double>::infinity();
    NamedPoint closestPoint;

    for(int i = 0; i < numberOfLinesTableType; i++)
    {
      if((type & line_type[i]) == line_type[i] && closestPoints[x][y][i].id != -1)
      {
        //NOTE: use the square distance because it's faster to calculate
        double dist = (closestPoints[x][y][i].position - p).abs2(); 
        if(dist < minDist)
        {
          closestPoint = closestPoints[x][y][i];
          minDist = dist;
        }
      }
    }//end for
   
    return closestPoint;
  }//end get_closest_point


  void draw_closest_points(DrawingCanvas2D& canvas, int type) const
  {
    Color white(1.0,1.0,1.0,0.0); // transparent
    Color black(0.0,0.0,0.0,1.0);

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
        NamedPoint np = get_closest_point(point, type);
        
        if(np.id != -1)
        {
          double d = (np.position - point).abs();
          double t = Math::clamp(d/(yWidth*ySize),0.0,1.0);
          Color color = white*t + black*(1-t);
          canvas.pen(color, 20);
        } else {
          // no closest line
          canvas.pen(white, 20);
        }
        canvas.fillBox(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
      }
    }
  }//end draw_closest_points



  void draw_closest_corner_points(DrawingCanvas2D& canvas) const
  {
    Color white(1.0,1.0,1.0,0.0); // transparent
    Color black(0.0,0.0,0.0,1.0);

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
        double d = (point - get_closest_corner_point(point).position).abs();
        double t = Math::clamp(d/(yWidth*ySize),0.0,1.0);
        Color color = white*t + black*(1-t);
        canvas.pen(color, 20);
        canvas.fillBox(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
      }
    }
  }//end draw_closest_points


  void draw_closest_tcrossing_points(DrawingCanvas2D& canvas) const
  {
    Color white(1.0,1.0,1.0,0.0); // transparent
    Color black(0.0,0.0,0.0,1.0);

    for (int x = 0; x < xSize; x++)
    {
      for (int y = 0; y < ySize; y++)
      {
        Vector2d point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
        double d = (point - get_closest_tcrossing_point(point).position).abs();
        double t = Math::clamp(d/(yWidth*ySize),0.0,1.0);
        Color color = white*t + black*(1-t);
        canvas.pen(color, 20);
        canvas.fillBox(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
      }
    }
  }//end draw_closest_points

};//end class LinesTable





#endif //__LinesTable_h_
