
#ifndef _LinePerceptAugmented_H
#define _LinePerceptAugmented_H

#include <vector>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Line.h"

class LinePerceptAugmented
{
public:
  // 
  bool middleCircleOrientationWasSeen = false;
  Vector2d middleCircleCenter;
  Math::LineSegment centerLine;

public:

  void reset() {
    middleCircleOrientationWasSeen = false;
  }
};

#endif // _LinePerceptAugmented_H
