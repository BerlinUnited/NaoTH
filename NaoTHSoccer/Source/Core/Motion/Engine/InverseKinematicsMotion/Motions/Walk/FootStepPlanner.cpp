/**
* @file FootStepPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#include "FootStepPlanner.h"

using namespace InverseKinematic;


FootStepPlanner::FootStepPlanner()
{
  theMaxTurnInner = Math::fromDegrees(10);
  theMaxStepTurn = Math::fromDegrees(30);;
  theMaxStepLength = 50;
  theMaxStepLengthBack = 50;
  theMaxStepWidth = 40;
  theFootOffsetY = NaoInfo::HipOffsetY;
}

void FootStepPlanner::addStep(FootStep& footStep, const Pose2D& step) const
{
  Pose3D& footEnd = footStep.footEnd();
  footEnd = footStep.supFoot();
  
  switch ( footStep.liftingFoot() )
  {
    case FootStep::RIGHT:
    {
      // calculate footstep for the RIGHT foot
      footEnd.translate(step.translation.x, -theFootOffsetY + min(0.0, step.translation.y), 0);
      footEnd.rotateZ(min(theMaxTurnInner, step.rotation));
      footEnd.translate(0, -theFootOffsetY, 0);
      break;
    }
    case FootStep::LEFT:
    {
      footEnd.translate(step.translation.x, theFootOffsetY + max(0.0, step.translation.y), 0);
      footEnd.rotateZ(max(-theMaxTurnInner, step.rotation));
      footEnd.translate(0, theFootOffsetY, 0);
      break;
    }
  }//end switch
}

FootStep FootStepPlanner::nextStep(const FootStep& lastStep, Pose2D step) const
{
  ASSERT(step.rotation <= Math::pi);
  ASSERT(step.rotation > -Math::pi);
  
  restrictStepSize(step, lastStep);
  
  //TODO
  //restrictStepChange
  
  //TODO
  //adapt step size
  
  FeetPose newFeetStepBegin = lastStep.end();
  FootStep newStep(newFeetStepBegin, static_cast<FootStep::Foot>(-lastStep.liftingFoot()) );
  addStep(newStep, step);
  
  return newStep;
}

FootStep FootStepPlanner::firstStep(FeetPose pose, Pose2D step, double feetSepWidth) const
{
  pose.localInRightFoot();
  FootStep newStep(pose, FootStep::LEFT );
  FootStep zeroStep(pose, FootStep::RIGHT );
  restrictStepSize(step, zeroStep);
  addStep(newStep, step);
  return newStep;
}

void FootStepPlanner::restrictStepSize(Pose2D& step, const FootStep& lastStep) const
{
  double maxTurn = theMaxStepTurn;
  
  double maxLen = sqrt(theMaxStepLength * theMaxStepLength + theMaxStepWidth * theMaxStepWidth);
  
  if (maxLen > 1)
  {
    double lastStepLen = (lastStep.footEnd().translation - lastStep.footBegin().translation).abs();
    maxTurn = theMaxStepTurn * (1 - Math::clamp(lastStepLen / maxLen, 0.5, 1.0));
  }

  // limit the rotation
  step.rotation = Math::clamp(step.rotation, -maxTurn, maxTurn);

  double maxStepLenth = theMaxStepLength;
  if ( step.translation.x < 0 )
  {
    maxStepLenth = theMaxStepLengthBack;
  }

  if ( maxStepLenth > 0.5  && theMaxStepWidth > 0.5 )
  {
    // restrict the step size in an ellipse
    double alpha = step.translation.angle();
    double cosa = cos(alpha);
    double sina = sin(alpha);

    const double maxStepLength2 = pow(maxStepLenth, 2);
    const double maxStepWidth2 = pow(theMaxStepWidth, 2);
    double length = sqrt((maxStepLength2 * maxStepWidth2) / (maxStepLength2 * pow(sina, 2) + maxStepWidth2 * pow(cosa, 2)));
    if ( step.translation.abs2() > pow(length, 2) )
    {
      step.translation.x = length * cosa;
      step.translation.y = length * sina;
    }

    ASSERT( step.translation.x > -theMaxStepLengthBack - 1e-5 );
    ASSERT( step.translation.x < theMaxStepLength + 1e-5 );
    ASSERT( fabs(step.translation.y) < theMaxStepWidth + 1e-5 );
  }
  else
  {
    if (step.translation.x < -theMaxStepLengthBack)
    {
      step.translation.x = -theMaxStepLengthBack;
    }
    else if (step.translation.x > theMaxStepLength )
    {
      step.translation.x = theMaxStepLength;
    }
    
    if (theMaxStepWidth > numeric_limits<double>::epsilon())
      step.translation.y = Math::clamp(step.translation.y, -theMaxStepWidth, theMaxStepWidth);
    else
      step.translation.y = 0;

    ASSERT( fabs(step.translation.x) <= theMaxStepLength );
    ASSERT( fabs(step.translation.y) <= theMaxStepWidth );
  }

  if ( theMaxStepTurn > Math::fromDegrees(1.0) )
  {
    step.translation *= cos( step.rotation/theMaxStepTurn * Math::pi / 2);
  }
}//end restrictStepSize
