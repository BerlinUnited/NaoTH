/**
* @file FootStepPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#include "FootStepPlanner.h"

using namespace InverseKinematic;
using namespace std;

FootStepPlanner::FootStepPlanner()
:
  theMaxTurnInner(0.0),
  theMaxStepTurn(0.0),
  theMaxStepLength(0.0),
  theMaxStepLengthBack(0.0),
  theMaxStepWidth(0.0),
  theFootOffsetY(0.0),
  theMaxChangeTurn(0.0),
  theMaxChangeX(0.0),
  theMaxChangeY(0.0)
{
}

void FootStepPlanner::updateParameters(const IKParameters& parameters)
{
  theMaxTurnInner = Math::fromDegrees(parameters.walk.limits.maxTurnInner);
  theMaxStepTurn = Math::fromDegrees(parameters.walk.limits.maxStepTurn);
  theMaxStepLength = parameters.walk.limits.maxStepLength;
  theMaxStepLengthBack = parameters.walk.limits.maxStepLengthBack;
  theMaxStepWidth = parameters.walk.limits.maxStepWidth;
  
  // step control
  theMaxCtrlTurn = parameters.walk.limits.maxCtrlTurn;
  theMaxCtrlLength = parameters.walk.limits.maxCtrlLength;
  theMaxCtrlWidth = parameters.walk.limits.maxCtrlWidth;


  theFootOffsetY = NaoInfo::HipOffsetY + parameters.footOffsetY;
  
  theMaxChangeTurn = theMaxStepTurn * parameters.walk.limits.maxStepChange;
  theMaxChangeX = theMaxStepLength * parameters.walk.limits.maxStepChange;
  theMaxChangeY = theMaxStepWidth * parameters.walk.limits.maxStepChange;
}//end updateParameters

void FootStepPlanner::addStep(FootStep& footStep, Pose2D step, const Pose2D& lastOffset, const Pose2D& offset) const
{
  footStep.offset() = offset;

  Pose3D& footEnd = footStep.footEnd();
  footEnd = footStep.supFoot();

  switch ( footStep.liftingFoot() )
  {
    case FootStep::RIGHT:
    {
      footEnd.rotateZ(-lastOffset.rotation);
      footEnd.translate(-lastOffset.translation.x, -lastOffset.translation.y, 0);

      // calculate footstep for the RIGHT foot
      footEnd.translate(step.translation.x, -theFootOffsetY + min(0.0, step.translation.y), 0);
      footEnd.rotateZ(min(theMaxTurnInner, step.rotation)); // TODO: restriction here
      footEnd.translate(0, -theFootOffsetY, 0);

      footEnd.rotateZ(-offset.rotation);
      footEnd.translate(-offset.translation.x, -offset.translation.y, 0);
      break;
    }
    case FootStep::LEFT:
    {
      footEnd.rotateZ(lastOffset.rotation);
      footEnd.translate(lastOffset.translation.x, lastOffset.translation.y, 0);

      footEnd.translate(step.translation.x, theFootOffsetY + max(0.0, step.translation.y), 0);
      footEnd.rotateZ(max(-theMaxTurnInner, step.rotation)); // TODO: restriction here
      footEnd.translate(0, theFootOffsetY, 0);

      footEnd.rotateZ(offset.rotation);
      footEnd.translate(offset.translation.x, offset.translation.y, 0);
      break;
    }
  default: ASSERT(false);
  }//end switch
}//end addStep

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
}//end nextStep

FootStep FootStepPlanner::controlStep(const FootStep& lastStep, const WalkRequest& req)
{
  WalkRequest myReq = req;
  myReq.target = req.stepControl.target;//HACK
  Pose2D step = calculateStep(lastStep, myReq);
  restrictStepSize(step, lastStep, req.character);
  //restrictStepSizeSimple(step, lastStep, req.character);

  FeetPose newFeetStepBegin = lastStep.end();
  FootStep newStep(newFeetStepBegin, (req.stepControl.moveLeftFoot?FootStep::LEFT:FootStep::RIGHT) );
  addStep(newStep, step, lastStep.offset(), req.offset);
  theLastStepSize = step;

  ASSERT(newStep.liftingFoot() == FootStep::LEFT || newStep.liftingFoot() == FootStep::RIGHT );
  return newStep;
}//end controlStep

Pose2D FootStepPlanner::calculateStep(const FootStep& lastStep,const WalkRequest& req)
{
  Pose2D step = req.target;

  if ( req.coordinate == WalkRequest::Hip )
      return step;

  Pose3D supFoot = lastStep.footEnd();
  Pose2D stepCoord = reduceDimen(supFoot);
  switch (lastStep.liftingFoot()) 
  {
  case FootStep::LEFT:
  {
    stepCoord -= lastStep.offset();
    stepCoord.translate(0, -theFootOffsetY);
    break;
  }
  case FootStep::RIGHT:
  {
    stepCoord += lastStep.offset();
    stepCoord.translate(0, theFootOffsetY);
    break;
  }
  default:
    break;
  }//end switch

  FeetPose lastPose = lastStep.end();
  Pose2D target;
  switch (req.coordinate) 
  {
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
}//end calculateStep

FootStep FootStepPlanner::nextStep(const FootStep& lastStep, Pose2D step, const WalkRequest& req)
{
  ASSERT(step.rotation <= Math::pi);
  ASSERT(step.rotation > -Math::pi);
  
  // TODO: correct restriction
  restrictStepSize(step, lastStep, req.character);
  restrictStepChange(step, theLastStepSize);
  
  theLastStepSize = step;

  FeetPose newFeetStepBegin = lastStep.end();
  FootStep::Foot liftingFoot = static_cast<FootStep::Foot>(-lastStep.liftingFoot());

  FootStep newStep(newFeetStepBegin, liftingFoot );
  addStep(newStep, step, lastStep.offset(), req.offset);
  ASSERT(newStep.liftingFoot() == FootStep::LEFT || newStep.liftingFoot() == FootStep::RIGHT );
  return newStep;
}//end nextStep

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
}//end firstStep

void FootStepPlanner::restrictStepSize(Pose2D& step, const FootStep& lastStep, double character) const
{
  //restrictStepSizeSimple(step, lastStep, character);
  //return;

  // scale the character: [0, 1] --> [0.5, 1]
  character = 0.5*character + 0.5;


  double maxTurn = theMaxStepTurn * character;
  
  /*double maxLen = sqrt(theMaxStepLength * theMaxStepLength + theMaxStepWidth * theMaxStepWidth) * character;

  if (maxLen > 1)
  {
    Pose3D lastHip = lastStep.supFoot();
    lastHip.translate(0, static_cast<int>(lastStep.liftingFoot())*theFootOffsetY*2, 0);
    double lastStepLen = (lastStep.footEnd().translation - lastHip.translation).abs();
    maxTurn *= Math::clamp(1 - lastStepLen / maxLen, 0.5, 1.0);
  }*/

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
}//end restrictStepSize


void FootStepPlanner::restrictStepSizeSimple(Pose2D& step, const FootStep& lastStep, double character) const
{
  character = 0.5*character + 0.5;

  double maxTurn =  theMaxCtrlTurn * character;
  double maxStepLength = theMaxCtrlLength * character;
  double maxStepWidth = theMaxCtrlWidth * character;

  // limit the rotation
  step.rotation = Math::clamp(step.rotation, -maxTurn, maxTurn);

  // limit translation
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

    //ASSERT( step.translation.x > -maxStepLengthBack - 1e-5 );
    ASSERT( step.translation.x < maxStepLength + 1e-5 );
    ASSERT( fabs(step.translation.y) < maxStepWidth + 1e-5 );
  }
}//end restrictStepSizeSimple


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

void FootStepPlanner::restrictStepChangeNew(Pose2D& step, const Pose2D& lastStep) const
{
  Pose2D change;
  change.translation = step.translation - lastStep.translation;
  change.rotation = Math::normalizeAngle(step.rotation - lastStep.rotation);
  double maxX = theMaxChangeX;
  double maxY = theMaxChangeY;
  double maxT = theMaxChangeTurn;

  // just clamp?!
  Pose2D change_restricted;
  change_restricted.translation.x = Math::clamp(change.translation.x, -maxX, maxX);
  change_restricted.translation.y = Math::clamp(change.translation.y, -maxY, maxY);
  change_restricted.rotation = Math::clamp(change.rotation, -maxT, maxT);

  
  // calculate the largest restriction factor
  double f = 1.0;
  if( std::fabs(change.translation.x) > std::fabs(change_restricted.translation.x) + 1 )
  {
    f = std::min(f, change_restricted.translation.x / change.translation.x);
  }
  if( std::fabs(change.translation.y) > std::fabs(change_restricted.translation.y) + 1 )
  {
    f = std::min(f, change_restricted.translation.y / change.translation.y);
  }
  // 0.02 ~ 1.5°
  if( std::fabs(change.rotation) > std::fabs(change_restricted.rotation) + 0.02 )
  {
    f = std::min(f, change_restricted.rotation / change.rotation);
  }

  // apply the restriction uniformly and preserve the motion direction
  change.translation.x = f*change_restricted.translation.x;
  change.translation.y = f*change_restricted.translation.y;
  change.rotation = f*change_restricted.rotation;


  step.translation = lastStep.translation + change.translation;
  step.rotation = Math::normalizeAngle(lastStep.rotation + change.rotation);
}//end restrictStepChange
