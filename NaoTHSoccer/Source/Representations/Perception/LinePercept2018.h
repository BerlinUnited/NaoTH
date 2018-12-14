#ifndef LINEPERCEPT2018_H
#define LINEPERCEPT2018_H

#include <vector>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Line.h"

class LinePercept2018
{
public:
  std::vector<Math::LineSegment> fieldLineSegments;

  void reset()
  {
    this->fieldLineSegments.clear();
  }
};

class MiddleCirclePercept
{
public:
  bool wasSeen;
  Vector2d center;

  MiddleCirclePercept():
    wasSeen(false){}

  void set(Vector2d& center) {
    this->wasSeen = true;
    this->center = center;
  }

  void reset()
  {
    this->wasSeen = false;
  }
};

// RansacLineDetector
class RansacLinePercept : public LinePercept2018
{
public:
  std::vector<int> edgelLineIDs;
};

class RansacCirclePercept2018 : public MiddleCirclePercept{};

// RansacLineDetectorOnGraphs
class ShortLinePercept : public LinePercept2018{};

class GraphRansacCirclePercept : public MiddleCirclePercept{};
class GraphRansacCirclePerceptTop : public GraphRansacCirclePercept{};

#endif // LINEPERCEPT2018_H
