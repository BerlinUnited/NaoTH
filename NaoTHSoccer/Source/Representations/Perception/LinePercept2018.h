#ifndef LINEPERCEPT2018_H
#define LINEPERCEPT2018_H

#include <vector>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Line.h"
#include "Tools/LinesTable.h"

class MiddleCircle
{
public: 
  bool wasSeen;
  Vector2d center;

public:
  MiddleCircle() :
    wasSeen(false)
  {}

  void set(const Vector2d& center) {
    this->wasSeen = true;
    this->center = center;
  }

  void reset() {
    wasSeen = false;
  }
};

class LinePercept2018
{
public:
  std::vector<Math::LineSegment> fieldLineSegments;

  void reset()
  {
    fieldLineSegments.clear();
  }
};

// RansacLineDetector
class RansacLinePercept : public LinePercept2018
{
public:
  std::vector<int> edgelLineIDs;

  void reset()
  {
    LinePercept2018::reset();
    edgelLineIDs.clear();
  }
};

// RansacLineDetectorOnGraphs
class ShortLinePercept : public LinePercept2018
{
  public:
    //
};

// RansacLineDetectorOnGraphs
class VirtualLinePercept : public LinePercept2018
{
public:
  /** 
    NOTE: copied from the old LinePercept
    currently used only in 3DSim to represent the seen corners.
  */
  class Flag
  {
  public:
    Flag(const Vector2d& seenPosOnField, const Vector2d& absolutePosOnField)
      : 
      seenPosOnField(seenPosOnField),
      absolutePosOnField(absolutePosOnField)
    {}
    Vector2d seenPosOnField;
    Vector2d absolutePosOnField; // model of the flag (i.e. its known absolute osition on the field)
  };

  void reset()
  {
    LinePercept2018::reset();
    flags.clear();
    intersections.clear();
  }

public:
  // seen flags (only S3D)
  std::vector<Flag> flags;
  std::vector<LineIntersection> intersections;
  MiddleCircle middleCircle;
};






class RansacCirclePercept2018 : public MiddleCircle{};

class GraphRansacCirclePercept : public MiddleCircle{};
class GraphRansacCirclePerceptTop : public GraphRansacCirclePercept{};

#endif // LINEPERCEPT2018_H
