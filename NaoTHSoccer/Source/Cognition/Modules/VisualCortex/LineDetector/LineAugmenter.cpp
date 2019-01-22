
#include "LineAugmenter.h"

LineAugmenter::LineAugmenter()
{
  
}

LineAugmenter::~LineAugmenter()
{
}

void LineAugmenter::execute()
{
  getLinePerceptAugmented().reset();

  calculateCenterLine();
 

}


void LineAugmenter::calculateCenterLine()
{
  if(getRansacCirclePercept2018().wasSeen)
  {
    for(size_t i = 0; i < getRansacLinePercept().fieldLineSegments.size(); i++)
    {
      double d = getRansacLinePercept().fieldLineSegments[i].minDistance(getRansacCirclePercept2018().center);
      if(d < 50.0) {
        getLinePerceptAugmented().centerLine = getRansacLinePercept().fieldLineSegments[i];
        getLinePerceptAugmented().middleCircleOrientationWasSeen = true;
      }
    }

    if (!getLinePerceptAugmented().middleCircleOrientationWasSeen)
    {
      for(size_t i = 0; i < getShortLinePercept().fieldLineSegments.size(); i++)
      {
        double d = getShortLinePercept().fieldLineSegments[i].minDistance(getRansacCirclePercept2018().center);
        if(d < 50.0) {
          getLinePerceptAugmented().centerLine = getShortLinePercept().fieldLineSegments[i];
          getLinePerceptAugmented().middleCircleOrientationWasSeen = true;
        }
      }
    }

    // provide the circle which was used to determine the center line
    if(getLinePerceptAugmented().middleCircleOrientationWasSeen) {
      getLinePerceptAugmented().middleCircleCenter = getRansacCirclePercept2018().center;
    }
  }
}

