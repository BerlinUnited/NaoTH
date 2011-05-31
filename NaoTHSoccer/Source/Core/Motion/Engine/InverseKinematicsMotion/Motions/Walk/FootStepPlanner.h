/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _FOOT_STEP_PLANNER_H
#define	_FOOT_STEP_PLANNER_H

#include "FootStep.h"

class FootStepPlanner
{
public:
  // the typical walk step
  static FootStep nextStep(const FootStep& lastStep, const Pose2D& step, double feetSepWidth);
  
  static FootStep firstStep(const InverseKinematic::FeetPose& pose, const Pose2D& step, double feetSepWidth);
};

#endif // _FOOT_STEP_PLANNER_H
