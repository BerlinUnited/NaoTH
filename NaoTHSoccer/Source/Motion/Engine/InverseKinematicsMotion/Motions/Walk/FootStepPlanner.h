/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _FOOT_STEP_PLANNER_H
#define _FOOT_STEP_PLANNER_H

#include "FootStep.h"
#include "../IKParameters.h"
#include "Representations/Motion/Request/WalkRequest.h"

class FootStepPlanner
{
public:
  FootStepPlanner();
  
  void updateParameters(const IKParameters& parameters);

  FootStep nextStep(const FootStep& lastStep, const WalkRequest& req);

  FootStep controlStep(const FootStep& lastStep, const WalkRequest& req);
  
private:
  /**
   * @param step: the step in WalkRequest::Hip
   */
  FootStep calculateNextWalkStep(const FootStep& lastStep, const WalkRequest& req);

  FootStep firstStep(const InverseKinematic::FeetPose& pose,const WalkRequest& req);

  void restrictStepSize(Pose2D& step, const FootStep& lastStep, double character) const;
  void restrictStepSizeControlStep(Pose2D& step, const FootStep& lastStep, double character) const;
  
  void restrictStepChange(Pose2D& step, const Pose2D& lastStep) const;
  

private: // low level calculations
  /** transform the requested target pose to the hip coordinates */
  Pose2D calculateStepRequestInHip(const FootStep& lastStep, const WalkRequest& req) const;

  void calculateFootStep(FootStep& footStep, Pose2D step, const Pose2D& lastOffset, const Pose2D& offset) const;
  

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

  double theMaxCtrlTurn;
  double theMaxCtrlLength;
  double theMaxCtrlWidth;
};

#endif // _FOOT_STEP_PLANNER_H
