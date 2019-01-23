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
    fieldLineSegments.clear();
  }
};

// RansacLineDetector
class RansacLinePercept : public LinePercept2018
{
public:
  std::vector<int> edgelLineIDs;
};

// RansacLineDetectorOnGraphs
class ShortLinePercept : public LinePercept2018
{
  public:
    //
};



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


class RansacCirclePercept2018 : public MiddleCircle{};

class GraphRansacCirclePercept : public MiddleCircle{};
class GraphRansacCirclePerceptTop : public GraphRansacCirclePercept{};

#endif // LINEPERCEPT2018_H
