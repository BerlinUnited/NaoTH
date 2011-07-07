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

void FootStepPlanner::addStep(FootStep& footStep, Pose2D step, const Pose2D& offset) const
{
  Pose3D& footEnd = footStep.footEnd();
  footEnd = footStep.supFoot();

  switch ( footStep.liftingFoot() )
  {
    case FootStep::RIGHT:
    {
      footEnd.rotateZ(-offset.rotation);
      footEnd.translate(-offset.translation.x, -offset.translation.y, 0);
      step -= offset;

      // calculate footstep for the RIGHT foot
      footEnd.translate(step.translation.x, -theFootOffsetY + min(0.0, step.translation.y), 0);
      footEnd.rotateZ(min(theMaxTurnInner, step.rotation));
      footEnd.translate(0, -theFootOffsetY, 0);
      break;
    }
    case FootStep::LEFT:
    {
      footEnd.rotateZ(offset.rotation);
      footEnd.translate(offset.translation.x, offset.translation.y, 0);
      step += offset;

      footEnd.translate(step.translation.x, theFootOffsetY + max(0.0, step.translation.y), 0);
      footEnd.rotateZ(max(-theMaxTurnInner, step.rotation));
      footEnd.translate(0, theFootOffsetY, 0);
      break;
    }
  default: ASSERT(false);
  }//end switch
}

FootStep FootStepPlanner::nextStep(const FootStep& lastStep, const WalkRequest& req)
{
  if ( lastStep.liftingFoot() == FootStep::NONE )
  {
    return firstStep(lastStep.end(), req);
  }
  else
  {
    Pose2D step = calculateStep(lastStep, req);
    return nextStep(lastStep, step, req);
  }
}
FootStep FootStepPlanner::controlStep(const FootStep& lastStep, const WalkRequest& req)
{
  WalkRequest myReq = req;
  myReq.target = req.stepControl.target;//HACK
  Pose2D step = calculateStep(lastStep, myReq);
  restrictStepSize(step, lastStep, req.character);

  FeetPose newFeetStepBegin = lastStep.end();
  FootStep newStep(newFeetStepBegin, (req.stepControl.moveLeftFoot?FootStep::LEFT:FootStep::RIGHT) );
  addStep(newStep, step, req.offset);
  theLastStepSize = step;

  ASSERT(newStep.liftingFoot() == FootStep::LEFT || newStep.liftingFoot() == FootStep::RIGHT );
  return newStep;
}

Pose2D FootStepPlanner::calculateStep(const FootStep& lastStep,const WalkRequest& req)
{
  Pose2D step = req.target;

  if ( req.coordinate == WalkRequest::Hip )
      return step;

  Pose3D supFoot = lastStep.footEnd();
  Pose2D stepCoord = reduceDimen(supFoot);
  switch (lastStep.liftingFoot()) {
  case FootStep::LEFT:
  {
    stepCoord -= req.offset;
    stepCoord.translate(0, -theFootOffsetY);
    break;
  }
  case FootStep::RIGHT:
  {
    stepCoord += req.offset;
    stepCoord.translate(0, theFootOffsetY);
    break;
  }
  default:
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

FootStep FootStepPlanner::nextStep(const FootStep& lastStep, Pose2D step, const WalkRequest& req)
{
  ASSERT(step.rotation <= Math::pi);
  ASSERT(step.rotation > -Math::pi);
  
  restrictStepSize(step, lastStep, req.character);
  restrictStepChange(step, theLastStepSize);
  theLastStepSize = step;

  FeetPose newFeetStepBegin = lastStep.end();
  FootStep::Foot liftingFoot = static_cast<FootStep::Foot>(-lastStep.liftingFoot());

  Pose3D offset;
  offset.translation = Vector3d(-req.offset.translation.x, -req.offset.translation.y, 0);
  offset.rotation = RotationMatrix::getRotationZ(-req.offset.rotation);

  FootStep newStep(newFeetStepBegin, liftingFoot );
  addStep(newStep, step, req.offset);
  ASSERT(newStep.liftingFoot() == FootStep::LEFT || newStep.liftingFoot() == FootStep::RIGHT );
  return newStep;
}

FootStep FootStepPlanner::firstStep(const InverseKinematic::FeetPose& pose,const WalkRequest& req)
{
  FootStep zeroStepLeft(pose, FootStep::LEFT );
  FootStep zeroStepRight(pose, FootStep::RIGHT);
  FootStep firstStepLeft = nextStep(zeroStepRight, req);
  FootStep firstStepRight = nextStep(zeroStepLeft, req);

  Pose3D leftMove = firstStepLeft.footBegin().invert() * firstStepLeft.footEnd();
  Pose3D rightMove = firstStepRight.footBegin().invert() * firstStepRight.footEnd();

  if ( abs(req.target.rotation) > theMaxTurnInner )
  {
    // choose foot by rotation
    double leftTurn = leftMove.rotation.getZAngle();
    double rightTurn = rightMove.rotation.getZAngle();
    if ( abs(leftTurn) > abs(rightTurn) )
    {
      return firstStepLeft;
    }
    else
    {
      return firstStepRight;
    }
  }
  else
  {
    // choose foot by distance
    if ( leftMove.translation.abs2() > rightMove.translation.abs2() )
    {
      return firstStepLeft;
    }
    else
    {
      return firstStepRight;
    }
  }
}

void FootStepPlanner::restrictStepSize(Pose2D& step, const FootStep& lastStep, double character) const
{
  // scale the character: [0, 1] --> [0.5, 1]
  character = 0.5*character + 0.5;

  double maxTurn = theMaxStepTurn * character;
  
  double maxLen = sqrt(theMaxStepLength * theMaxStepLength + theMaxStepWidth * theMaxStepWidth) * character;
  
  if (maxLen > 1)
  {
    Pose3D lastHip = lastStep.supFoot();
    lastHip.translate(0, static_cast<int>(lastStep.liftingFoot())*theFootOffsetY*2, 0);
    double lastStepLen = (lastStep.footEnd().translation - lastHip.translation).abs();
    maxTurn = theMaxStepTurn * Math::clamp(1 - lastStepLen / maxLen, 0.5, 1.0);
  }

  // limit the rotation
  step.rotation = Math::clamp(step.rotation, -maxTurn, maxTurn);

  double maxStepLength = theMaxStepLength * character;
  double maxStepLengthBack = theMaxStepLengthBack * character;
  double maxStepWidth = theMaxStepWidth * character;
  if ( step.translation.x < 0 )
  {
    maxStepLength = maxStepLengthBack;
  }

  if ( maxStepLength > 0.5  && maxStepWidth > 0.5 )
  {
    // restrict the step size in an ellipse
    double alpha = step.translation.angle();
    double cosa = cos(alpha);
    double sina = sin(alpha);

    const double maxStepLength2 = pow(maxStepLength, 2);
    const double maxStepWidth2 = pow(maxStepWidth, 2);
    double length = sqrt((maxStepLength2 * maxStepWidth2) / (maxStepLength2 * pow(sina, 2) + maxStepWidth2 * pow(cosa, 2)));
    if ( step.translation.abs2() > pow(length, 2) )
    {
      step.translation.x = length * cosa;
      step.translation.y = length * sina;
    }

    ASSERT( step.translation.x > -maxStepLengthBack - 1e-5 );
    ASSERT( step.translation.x < maxStepLength + 1e-5 );
    ASSERT( fabs(step.translation.y) < maxStepWidth + 1e-5 );
  }
  else
  {
    if (step.translation.x < -maxStepLengthBack)
    {
      step.translation.x = -maxStepLengthBack;
    }
    else if (step.translation.x > maxStepLength )
    {
      step.translation.x = maxStepLength;
    }
    
    if (theMaxStepWidth > numeric_limits<double>::epsilon())
      step.translation.y = Math::clamp(step.translation.y, -maxStepWidth, maxStepWidth);
    else
      step.translation.y = 0;

    ASSERT( fabs(step.translation.x) <= maxStepLength );
    ASSERT( fabs(step.translation.y) <= maxStepWidth );
  }

  if ( maxTurn > Math::fromDegrees(1.0) )
  {
    step.translation *= cos( step.rotation/maxTurn * Math::pi / 2);
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
