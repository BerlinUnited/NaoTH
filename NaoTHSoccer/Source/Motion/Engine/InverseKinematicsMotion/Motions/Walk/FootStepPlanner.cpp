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
  theMaxChangeY(0.0),

  theMaxCtrlTurn(0.0),
  theMaxCtrlLength(0.0),
  theMaxCtrlWidth(0.0)
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
}


FootStep FootStepPlanner::nextStep(const FootStep& lastStep, const WalkRequest& req)
{
  if ( lastStep.liftingFoot() == FootStep::NONE ) {
    return firstStep(lastStep.end(), req);
  } else {
    return calculateNextWalkStep(lastStep, req);
  }
}//end nextStep


FootStep FootStepPlanner::controlStep(const FootStep& lastStep, const WalkRequest& req)
{
  WalkRequest myReq = req;
  myReq.target = req.stepControl.target;//HACK
  Pose2D step = calculateStepRequestInHip(lastStep, myReq);
  restrictStepSizeControlStep(step, lastStep, req.character);

  FeetPose newFeetStepBegin = lastStep.end();
  FootStep newStep(newFeetStepBegin, (req.stepControl.moveLeftFoot?FootStep::LEFT:FootStep::RIGHT) );
  calculateFootStep(newStep, step, lastStep.offset(), req.offset);
  theLastStepSize = step;

  ASSERT(newStep.liftingFoot() == FootStep::LEFT || newStep.liftingFoot() == FootStep::RIGHT );
  return newStep;
}//end controlStep


FootStep FootStepPlanner::firstStep(const InverseKinematic::FeetPose& pose, const WalkRequest& req)
{
  FootStep zeroStepLeft(pose, FootStep::LEFT );
  FootStep firstStepRight = calculateNextWalkStep(zeroStepLeft, req);


  FootStep zeroStepRight(pose, FootStep::RIGHT);
  FootStep firstStepLeft = calculateNextWalkStep(zeroStepRight, req);


  Pose3D leftMove = firstStepLeft.footBegin().invert() * firstStepLeft.footEnd();
  Pose3D rightMove = firstStepRight.footBegin().invert() * firstStepRight.footEnd();


  if ( fabs(req.target.rotation) > theMaxTurnInner )
  {
    // choose foot by rotation
    double leftTurn = leftMove.rotation.getZAngle();
    double rightTurn = rightMove.rotation.getZAngle();
    if ( fabs(leftTurn) > fabs(rightTurn) ) {
      return firstStepLeft;
    } else {
      return firstStepRight;
    }
  }
  else
  {
    // choose foot by distance
    if ( leftMove.translation.abs2() > rightMove.translation.abs2() ) {
      return firstStepLeft;
    } else {
      return firstStepRight;
    }
  }
}//end firstStep

// TODO: parameter for the foot to move
FootStep FootStepPlanner::calculateNextWalkStep(const FootStep& lastStep, const WalkRequest& req)
{
  Pose2D stepRequest = calculateStepRequestInHip(lastStep, req);
  ASSERT(stepRequest.rotation <= Math::pi);
  ASSERT(stepRequest.rotation >= -Math::pi);
  
  // TODO: correct restriction
  restrictStepSize(stepRequest, lastStep, req.character);
  restrictStepChange(stepRequest, theLastStepSize);
  
  theLastStepSize = stepRequest;


  FeetPose newFeetStepBegin = lastStep.end();
  FootStep::Foot liftingFoot = static_cast<FootStep::Foot>(-lastStep.liftingFoot());
  FootStep newStep(newFeetStepBegin, liftingFoot );

  calculateFootStep(newStep, stepRequest, lastStep.offset(), req.offset);
  ASSERT(newStep.liftingFoot() == FootStep::LEFT || newStep.liftingFoot() == FootStep::RIGHT );

  return newStep;
}//end calculateNextWalkStep


void FootStepPlanner::restrictStepSize(Pose2D& step, const FootStep& /*lastStep*/, double character) const
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
    if (step.translation.x < -maxStepLengthBack) {
      step.translation.x = -maxStepLengthBack;
    } else if (step.translation.x > maxStepLength ) {
      step.translation.x = maxStepLength;
    }
    
    if (theMaxStepWidth > numeric_limits<double>::epsilon()) {
      step.translation.y = Math::clamp(step.translation.y, -maxStepWidth, maxStepWidth);
    } else {
      step.translation.y = 0;
    }

    ASSERT( fabs(step.translation.x) <= maxStepLength );
    ASSERT( fabs(step.translation.y) <= maxStepWidth );
  }

  if ( maxTurn > Math::fromDegrees(1.0) ) {
    step.translation *= cos( step.rotation/maxTurn * Math::pi / 2);
  }
}//end restrictStepSize


void FootStepPlanner::restrictStepSizeControlStep(Pose2D& step, const FootStep& /*lastStep*/, double character) const
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
}//end restrictStepSizeControlStep


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


Pose2D FootStepPlanner::calculateStepRequestInHip(const FootStep& lastStep, const WalkRequest& req) const
{
  Pose2D stepRequest = req.target;

  if ( req.coordinate == WalkRequest::Hip ) {
    return stepRequest;
  }

  // end pose of the foot that moved in the last step
  // => current support foot
  Pose2D originCoordinates = lastStep.footEnd().projectXY();
  switch (lastStep.liftingFoot()) 
  {
    case FootStep::LEFT:
    {
      originCoordinates -= lastStep.offset(); // stepCoord = stepCoord + lastStep.offset().invert()
      originCoordinates.translate(0, -theFootOffsetY); // stepCoord = stepCoord + Rose2D(0, -theFootOffsetY);
      // stepCoord = stepCoord+lastStep.offset().invert()+Rose2D(0, -theFootOffsetY);
      break;
    }
    case FootStep::RIGHT:
    {
      originCoordinates += lastStep.offset();
      originCoordinates.translate(0, theFootOffsetY);
      break;
    }
    default:
      break;
  }//end switch


  Pose2D targetCoordinates;
  switch (req.coordinate) 
  {
    case WalkRequest::LFoot:
    {
      Pose2D lfoot = lastStep.end().left.projectXY();
      targetCoordinates = lfoot + stepRequest;
      targetCoordinates.translate(0, -theFootOffsetY);
      break;
    }
    case WalkRequest::RFoot:
    {
      Pose2D rfoot = lastStep.end().right.projectXY();
      targetCoordinates = rfoot + stepRequest;
      targetCoordinates.translate(0, theFootOffsetY);
      break;
    }
    default:
      ASSERT(false);
      break;
  }//end switch

  return targetCoordinates - originCoordinates;
}//end calculateStepRequestInHip


void FootStepPlanner::calculateFootStep(FootStep& footStep, Pose2D step, const Pose2D& supportOffset, const Pose2D& targetOffset) const
{
  // calculate the new position for the moving foot based on the support foot
  Pose2D supFootCoordinates = footStep.supFoot().projectXY();

  switch ( footStep.liftingFoot() )
  {
    case FootStep::RIGHT:
    {
      // transform to support origin coordinates
      // supFootCoordinates = supFootCoordinates * lastOffset.invert();
      supFootCoordinates.rotate(-supportOffset.rotation);
      supFootCoordinates.translate(-supportOffset.translation.x, -supportOffset.translation.y);

      // supFootCoordinates = supFootCoordinates * Rose2D(0, -theFootOffsetY)
      supFootCoordinates.translate(0, -theFootOffsetY);

      // TODO: no restriction here
      // transform support origin to target origin
      // supFootCoordinates = supFootCoordinates *  Rose2D(min(theMaxTurnInner, step.rotation), step.translation.x, min(0.0, step.translation.y))
      supFootCoordinates.translate(step.translation.x, min(0.0, step.translation.y));
      supFootCoordinates.rotate(min(theMaxTurnInner, step.rotation)); 


      // supFootCoordinates = supFootCoordinates * Rose2D(0, -theFootOffsetY)
      supFootCoordinates.translate(0, -theFootOffsetY);
      
      // supFootCoordinates = supFootCoordinates * targetOffset.invert();
      supFootCoordinates.rotate(-targetOffset.rotation);
      supFootCoordinates.translate(-targetOffset.translation.x, -targetOffset.translation.y);
      break;
    }
    case FootStep::LEFT:
    {
      supFootCoordinates.rotate(supportOffset.rotation);
      supFootCoordinates.translate(supportOffset.translation.x, supportOffset.translation.y);
      supFootCoordinates.translate(0, theFootOffsetY);

      // TODO: no restriction here
      supFootCoordinates.translate(step.translation.x, max(0.0, step.translation.y));
      supFootCoordinates.rotate(max(-theMaxTurnInner, step.rotation)); 
      
      supFootCoordinates.translate(0, theFootOffsetY);
      supFootCoordinates.rotate(targetOffset.rotation);
      supFootCoordinates.translate(targetOffset.translation.x, targetOffset.translation.y);
      break;
    }
    default: ASSERT(false);
  }//end switch

  footStep.offset() = targetOffset;
  footStep.footEnd() = Pose3D::embedXY(supFootCoordinates);
}//end calculateFootStep