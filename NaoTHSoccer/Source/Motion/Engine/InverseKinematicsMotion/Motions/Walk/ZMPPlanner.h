/**
* @file ZMPPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#ifndef _ZMP_PLANNER_H
#define  _ZMP_PLANNER_H

#include "FootStep.h"

class ZMPPlanner
{
public:
  /**
   * @brief just set the ZMP on support foot
   */
  static Vector2d simplest(const FootStep& step, double offsetX, double offsetY);

  /**
   * @brief a little better than the simplest(..) one
   */
  static Vector2d betterOne(const FootStep& step, double offsetX, double offsetY,
    double cycle, double samplesDoubleSupport, double samplesSingleSupport, double extendDoubleSupport);
};

#endif // _FOOT_STEP_PLANNER_H
