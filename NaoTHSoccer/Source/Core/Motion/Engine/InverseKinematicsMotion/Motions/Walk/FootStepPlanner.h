/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _FOOT_STEP_PLANNER_H
#define  _FOOT_STEP_PLANNER_H

#include "FootStep.h"
#include "../IKParameters.h"
#include "Representations/Motion/Request/WalkRequest.h"

class FootStepPlanner
{
public:
  FootStepPlanner();
  
  void updateParameters(const IKParameters& parameters);

  FootStep firstStep(const InverseKinematic::FeetPose& pose,const WalkRequest& req);

  FootStep nextStep(const FootStep& lastStep,const WalkRequest& req);

  FootStep controlStep(const FootStep& lastStep,const WalkRequest& req);
  
private:
  Pose2D calculateStep(const FootStep& lastStep,const WalkRequest& req);

  /**
   * @param step: the step in WalkRequest::Hip
   */
  FootStep nextStep(const FootStep& lastStep, Pose2D step, const WalkRequest& req);

  void restrictStepSize(Pose2D& step, const FootStep& lastStep, double character) const;
  
  void restrictStepChange(Pose2D& step, const Pose2D& lastStep) const;
  
  void addStep(FootStep& footStep, Pose2D step, const Pose2D& lastOffset, const Pose2D& offset) const;
  
private:
  Pose2D theLastStepSize;

  // parameters
  double theMaxTurnInner;
  double theMaxStepTurn;
  double theMaxStepLength;
  double theMaxStepLengthBack;
  double theMaxStepWidth;
  double theFootOffsetY;
  double theMaxChangeTurn;
  double theMaxChangeX;
  double theMaxChangeY;
};

#endif // _FOOT_STEP_PLANNER_H
