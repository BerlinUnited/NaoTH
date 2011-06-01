/**
* @file ZMPPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the ZMP position according to foot step
*/

#ifndef _ZMP_PLANNER_H
#define	_ZMP_PLANNER_H

#include "FootStep.h"

class ZMPPlanner
{
public:
  /**
   * @brief just set the ZMP on support foot
   */
  static Pose3D simplest(const FootStep& step, const InverseKinematic::FeetPose& feet, double height);
};

#endif // _FOOT_STEP_PLANNER_H
