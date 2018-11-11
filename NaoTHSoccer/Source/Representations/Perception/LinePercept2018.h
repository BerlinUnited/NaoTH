#ifndef LINEPERCEPT2018_H
#define LINEPERCEPT2018_H

#include <vector>

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


class RansacLinePercept : public LinePercept2018
{
public:
  std::vector<int> edgelLineIDs;
};

class RansacCirclePercept2018 : public MiddleCirclePercept{};




#endif // LINEPERCEPT2018_H
