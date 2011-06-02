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
  FootStepPlanner();
  
  // the typical walk step
  FootStep nextStep(const FootStep& lastStep, Pose2D step) const;
  
  FootStep firstStep(InverseKinematic::FeetPose pose, Pose2D step, double feetSepWidth) const;
  
  void restrictStepSize(Pose2D& step, const FootStep& lastStep) const;
  
  void addStep(FootStep& footStep, const Pose2D& step) const;
  
private:
  double theMaxTurnInner;
  double theMaxStepTurn;
  double theMaxStepLength;
  double theMaxStepLengthBack;
  double theMaxStepWidth;
  double theFootOffsetY;
};

#endif // _FOOT_STEP_PLANNER_H
