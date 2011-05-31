/**
* @file FootStepPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#include "FootStepPlanner.h"

using namespace InverseKinematic;

FootStep FootStepPlanner::nextStep(const FootStep& lastStep, const Pose2D& step, double feetSepWidth)
{
  FeetPose newFeetStepBegin = lastStep.end();
  FootStep newStep(newFeetStepBegin, static_cast<FootStep::Foot>(-lastStep.liftingFoot()) );
  // TODO: move foot
  return newStep;
}

FootStep FootStepPlanner::firstStep(const FeetPose& pose, const Pose2D& step, double feetSepWidth)
{
  FootStep newStep(pose, FootStep::LEFT );
  // TODO: move foot
  return newStep;
}