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
}

void FootStepPlanner::updateParameters(const IKParameters& parameters)
{
  theMaxTurnInner = Math::fromDegrees(parameters.walk.maxTurnInner);
  theMaxStepTurn = Math::fromDegrees(parameters.walk.maxStepTurn);
  theMaxStepLength = parameters.walk.maxStepLength;
  theMaxStepLengthBack = parameters.walk.maxStepLengthBack;
  theMaxStepWidth = parameters.walk.maxStepWidth;
  
  theFootOffsetY = NaoInfo::HipOffsetY + parameters.footOffsetY;
  
  theMaxChangeTurn = theMaxStepTurn * parameters.walk.maxStepChange;
  theMaxChangeX = theMaxStepLength * parameters.walk.maxStepChange;
  theMaxChangeY = theMaxStepWidth * parameters.walk.maxStepChange;
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

FootStep FootStepPlanner::nextStep(const FootStep& lastStep, const WalkRequest& req)
{
  Pose2D step = calculateStep(lastStep, req);
  return nextStep(lastStep, step);
}

Pose2D FootStepPlanner::calculateStep(const FootStep& lastStep,const WalkRequest& req)
{
  Pose2D step = req;

  if ( req.coordinate == WalkRequest::Hip )
      return step;

  Pose3D supFoot = lastStep.footEnd();
  Pose2D stepCoord = reduceDimen(supFoot);
  switch (lastStep.liftingFoot()) {
  case FootStep::LEFT:
    stepCoord.translate(0, -theFootOffsetY);
    break;
  case FootStep::RIGHT:
    stepCoord.translate(0, theFootOffsetY);
    break;
  default:
    ASSERT(false);
    break;
  }//end switch

  FeetPose lastPose = lastStep.end();
  Pose2D target;
  switch (req.coordinate) {
  case WalkRequest::LFoot:
  {
    Pose2D lfoot = reduceDimen(lastPose.left);
    target = lfoot + step;
    target.translate(0, -theFootOffsetY);
    break;
  }
  case WalkRequest::RFoot:
  {
    Pose2D rfoot = reduceDimen(lastPose.right);
    target = rfoot + step;
    target.translate(0, theFootOffsetY);
    break;
  }
  default:
    ASSERT(false);
    break;
  }//end switch

  step = target - stepCoord;
  return step;
}

FootStep FootStepPlanner::nextStep(const FootStep& lastStep, Pose2D step)
{
  ASSERT(step.rotation <= Math::pi);
  ASSERT(step.rotation > -Math::pi);
  
  restrictStepSize(step, lastStep);
  restrictStepChange(step, theLastStepSize);
  
  //TODO
  //adapt step size
  
  FeetPose newFeetStepBegin = lastStep.end();
  FootStep newStep(newFeetStepBegin, static_cast<FootStep::Foot>(-lastStep.liftingFoot()) );
  addStep(newStep, step);
  theLastStepSize = step;
  
  return newStep;
}

FootStep FootStepPlanner::firstStep(InverseKinematic::FeetPose pose,const WalkRequest& req)
{
  // choose foot, TODO: consider current com?
  FootStep::Foot startFoot;
  if (req.rotation >= theMaxTurnInner || ( abs(req.rotation) < theMaxTurnInner && req.translation.y > 0))
  {
      startFoot = FootStep::LEFT;
      pose.localInRightFoot();
  }
  else
  {
      startFoot = FootStep::RIGHT;
      pose.localInLeftFoot();
  }

  FootStep zeroStep(pose, static_cast<FootStep::Foot>(-startFoot) );
  return nextStep(zeroStep, req);
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
}

void FootStepPlanner::restrictStepChange(Pose2D& step, const Pose2D& lastStep) const
{
  Pose2D change;
  change.translation = step.translation - lastStep.translation;
  change.rotation = Math::normalizeAngle(step.rotation - lastStep.rotation);
  double maxX = theMaxChangeX;
  double maxY = theMaxChangeY;
  double maxT = theMaxChangeTurn;

  change.translation.x = Math::clamp(change.translation.x, -maxX, maxX);
  change.translation.y = Math::clamp(change.translation.y, -maxY, maxY);
  change.rotation = Math::clamp(change.rotation, -maxT, maxT);
  step.translation = lastStep.translation + change.translation;
  step.rotation = Math::normalizeAngle(lastStep.rotation + change.rotation);
}
