/**
* @file PathPlanner2018.cpp
*
* @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
* Implementation of class PathPlanner2018
*/

#include "PathPlanner2018.h"

PathPlanner2018::PathPlanner2018()
  :
  stepBuffer({}),
  footToUse(Foot::RIGHT),
  lastStepRequestID(getMotionStatus().stepControl.stepRequestID + 1),      // WalkRequest stepRequestID starts at 0, we have to start at 1
  kickPlanned(false)
{
  getDebugParameterList().add(&params);
}

PathPlanner2018::~PathPlanner2018()
{
  getDebugParameterList().remove(&params);
}

void PathPlanner2018::execute()
{
  getPathModel().kick_executed = false;

  // Always executed first
  manageStepBuffer();

  // The kick has been executed
  // Tells XABSL to jump into next state
  if (kickPlanned && stepBuffer.empty())
  {
    getPathModel().kick_executed = true;
  }

  // HACK: xabsl set a forced motion request => clear everything
  if (getPathModel().path_routine == PathModel::PathRoutine::NONE && getMotionRequest().forced) {
    stepBuffer.clear();
    return;
  }

  switch (getPathModel().path2018_routine)
  {
  case PathModel::PathPlanner2018Routine::NONE:
    if (kickPlanned)
    {
      kickPlanned = false;
    }

    // TODO: should the stepBuffer just be cleared here no matter what?
    if (stepBuffer.empty())
    {
      return;
    }
    break;
  case PathModel::PathPlanner2018Routine::MOVE_AROUND_BALL:
    moveAroundBall(getPathModel().direction, getPathModel().radius, getPathModel().stable);
    break;
  case PathModel::PathPlanner2018Routine::FORWARDKICK:
    //if (farApproach())
    {
      if (nearApproach_forwardKick(getPathModel().xOffset, getPathModel().yOffset))
      {
        forwardKick();
      }
    }
    break;
  case PathModel::PathPlanner2018Routine::SIDEKICK_LEFT:
    if (farApproach())
    {
      if (nearApproach_sideKick(Foot::LEFT, 0.0, params.sidekickOffsetY))
      {
        sideKick(Foot::LEFT);
      }
    }
    break;
  case PathModel::PathPlanner2018Routine::SIDEKICK_RIGHT:
    if (farApproach())
    {
      if (nearApproach_sideKick(Foot::RIGHT, 0.0, -1 * params.sidekickOffsetY))
      {
        sideKick(Foot::RIGHT);
      }
    }
    break;
  case PathModel::PathPlanner2018Routine::SIDESTEP:
    sidesteps(Foot::RIGHT, getPathModel().direction);
  }//end switch

  // Always executed last
  executeStepBuffer();
}

void PathPlanner2018::moveAroundBall(const double direction, const double radius, const bool stable)
{
 if (stepBuffer.empty())
 {
    Vector2d ballPos    = getBallModel().positionPreview;
    double ballRotation = ballPos.angle();
    double ballDistance = ballPos.abs();

    double direction_deg = Math::toDegrees(direction);
    if (direction_deg > -10 && direction_deg <= 0){
      direction_deg = -10;
    }
    if (direction_deg < 10 && direction_deg > 0){
      direction_deg = 10;
    }

    double min1;
    double min2;
    double max1;
    double max2;
    if (direction_deg <= 0)
    {
      // turn left
	    min1 = 0.0;
	    min2 = 0.0;
	    max1 = 45.0;
	    max2 = 100.0;
    }
    else {
      // turn right
	    min1 = -45;
	    min2 = -100;
	    max1 = 0;
	    max2 = 0;
    }

    double stepX = (ballDistance - radius) * std::cos(ballRotation);
    // Math::clamp(-direction, min1, max1) ==> safe guard for xabsl input
    // outer clamp geht von -radius zu 0 
    double stepY = Math::clamp(radius * std::tan(Math::fromDegrees(Math::clamp(-direction_deg, min1, max1))), min2, max2) * std::cos(ballRotation);

    Pose2D pose = { ballRotation, stepX, stepY };
  
    StepBufferElement move_around_step;
    move_around_step.debug_name = "move_around_step";
    move_around_step.setPose(pose);
    move_around_step.setStepType(StepType::WALKSTEP);

    if (stable){
      move_around_step.setCharacter(params.moveAroundBallCharacterStable);
    }
    else{
      move_around_step.setCharacter(params.moveAroundBallCharacter);
    }
  
    move_around_step.setScale(1.0);
    move_around_step.setCoordinate(Coordinate::Hip);
    move_around_step.setFoot(Foot::NONE);
    move_around_step.setSpeedDirection(Math::fromDegrees(0.0));
    move_around_step.setRestriction(RestrictionMode::SOFT);
    move_around_step.setProtected(false);
    move_around_step.setTime(250);

    addStep(move_around_step);
  }
}

bool PathPlanner2018::farApproach()
{
  if (stepBuffer.empty())
  {
    Vector2d ballPos = getBallModel().positionPreview;
    double numPossibleSteps = ballPos.abs() / params.stepLength;

    if (numPossibleSteps > params.farToNearApproachThreshold)
    {
      double translation_xy = params.stepLength;

      StepBufferElement far_approach_step;
      far_approach_step.debug_name = "far_approach_step";
      far_approach_step.setPose({ ballPos.angle(), translation_xy, std::min(translation_xy, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1) });
      far_approach_step.setStepType(StepType::WALKSTEP);
      far_approach_step.setCharacter(0.7);
      far_approach_step.setScale(1.0);
      far_approach_step.setCoordinate(Coordinate::Hip);
      far_approach_step.setFoot(Foot::NONE);
      far_approach_step.setSpeedDirection(Math::fromDegrees(0.0));
      far_approach_step.setRestriction(RestrictionMode::HARD);
      far_approach_step.setProtected(false);
      far_approach_step.setTime(250);

      addStep(far_approach_step);
    }
    else
    {
      return true;
    }
  }

  return false;
}

bool PathPlanner2018::sidesteps(const Foot& foot, const double direction){
  // Always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Coordinate coordinate = Coordinate::Hip;

    if (foot == Foot::RIGHT)
    {
      coordinate = Coordinate::RFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::LFoot;
    }
    else
    {
      ASSERT(false);
    }

    StepBufferElement side_step;
    side_step.setPose({ 0.0, 0.0, direction > 0.0 ? 100.0 : -100.0});
    side_step.setStepType(StepType::WALKSTEP);
    side_step.setCharacter(0.3);
    side_step.setScale(1.0);
    side_step.setCoordinate(coordinate);
    side_step.setFoot(Foot::NONE);
    side_step.setSpeedDirection(Math::fromDegrees(0.0));
    side_step.setRestriction(RestrictionMode::SOFT);
    side_step.setProtected(false);
    side_step.setTime(400);

    addStep(side_step);
    return true;
  }
  else{
    return false;
  }

}

bool PathPlanner2018::nearApproach_forwardKick(const double offsetX, const double offsetY)
{
  // Always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Vector2d ballPos;
    Vector2d targetPos;
    Coordinate coordinate = Coordinate::Hip;

    //if (foot == Foot::RIGHT)
    if (getBallModel().positionPreview.y < 0)
    {
      ballPos    = getBallModel().positionPreviewInRFoot;
      coordinate = Coordinate::RFoot;
    }
    //else if (foot == Foot::LEFT)
    else if (getBallModel().positionPreview.y >= 0)
    {
      coordinate = Coordinate::LFoot;
      ballPos    = getBallModel().positionPreviewInLFoot;
    }
    else
    {
      ASSERT(false);
    }
    // add the desired offset
    targetPos.x = ballPos.x - getFieldInfo().ballRadius - offsetX;
    targetPos.y = ballPos.y - offsetY;

    // Am I ready for a kick or still walking to the ball?
    // In other words: Can I only perform one step before touching the ball or more steps?
    //TODO rewrite the condition from plan step if possible to dont plan steps if to close already
    if (std::abs(targetPos.x)  > params.forwardKickThreshold.x || std::abs(targetPos.y) > params.forwardKickThreshold.y)
    {
        // generate a correction step
      double translation_xy = params.stepLength;

      //std::abs(targetPos.y) => das heißt doch wenn der ball in der y richtung springt wird ein schritt zurück geplant und ausgeführt
      // das ist dafür das das er an den ball anlaufen kann ohne zu rotieren. Wenn man nah am ball ist wird angenommen das die Rotation
      //stimmt und dann soll diese auch nicht korrigiert werden
      double translation_x = std::min(translation_xy, targetPos.x - std::abs(targetPos.y));
      double translation_y = std::min(translation_xy, std::abs(targetPos.y)) * (targetPos.y < 0 ? -1 : 1);

      StepBufferElement near_approach_forward_step;
      near_approach_forward_step.debug_name = "near_approach_forward_step";
      near_approach_forward_step.setPose({ 0.0, translation_x, translation_y });
      near_approach_forward_step.setStepType(StepType::WALKSTEP);
      near_approach_forward_step.setCharacter(0.3);
      near_approach_forward_step.setScale(1.0);
      near_approach_forward_step.setCoordinate(coordinate);
      near_approach_forward_step.setFoot(Foot::NONE);
      near_approach_forward_step.setSpeedDirection(Math::fromDegrees(0.0));
      near_approach_forward_step.setRestriction(RestrictionMode::HARD);
      near_approach_forward_step.setProtected(false);
      near_approach_forward_step.setTime(250);

      addStep(near_approach_forward_step);
    }
    else
    {
      return true;
    }
  }

  return false;
}
   
bool PathPlanner2018::nearApproach_sideKick(const Foot& foot, const double offsetX, const double offsetY)
{
  // TODO: Has to work without rotation (like nearApproach_forwardKick)
  // Always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Vector2d ballPos;
    Coordinate coordinate = Coordinate::Hip;

    if (foot == Foot::RIGHT)
    {
      ballPos = getBallModel().positionPreviewInRFoot;
      coordinate = Coordinate::RFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::LFoot;
      ballPos = getBallModel().positionPreviewInLFoot;
    }
    else
    {
      ASSERT(false);
    }
    // add the desired offset
    ballPos.x += offsetX;
    ballPos.y += offsetY;

    // TODO: Are there better ways to calculate this?
    double numPossibleStepsX = std::abs(ballPos.x) / params.stepLength;
    double numPossibleStepsY = std::abs(ballPos.y) / params.stepLength;

    // Am I ready for a kick ?
    if (numPossibleStepsX > params.readyForSideKickThresholdX
      || numPossibleStepsY > params.readyForSideKickThresholdY)
    {
      double translation_x = std::min(params.stepLength, ballPos.x - getFieldInfo().ballRadius - params.nearApproachSideKickBallPosOffsetX - std::abs(ballPos.y));
      double translation_y = std::min(params.stepLength, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1);

      StepBufferElement new_step;
      new_step.setPose({ 0.0, translation_x, translation_y });
      new_step.setStepType(StepType::WALKSTEP);
      new_step.setCharacter(0.7);
      new_step.setScale(1.0);
      new_step.setCoordinate(coordinate);
      new_step.setFoot(Foot::NONE);
      new_step.setSpeedDirection(Math::fromDegrees(0.0));
      new_step.setRestriction(RestrictionMode::HARD);
      new_step.setProtected(false);
      new_step.setTime(250);

      addStep(new_step);
    }
    else
    {
      MotionStatus::StepControlStatus::MoveableFoot movableFoot = getMotionStatus().stepControl.moveableFoot;

      if (movableFoot != (foot == Foot::RIGHT ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT)
        && movableFoot != MotionStatus::StepControlStatus::BOTH)
      {
        double translation_x = std::min(params.stepLength, ballPos.x - getFieldInfo().ballRadius - params.nearApproachSideKickBallPosOffsetX - std::abs(ballPos.y));
        double translation_y = std::min(params.stepLength, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1);

        StepBufferElement correction_step;
        correction_step.setPose({ 0.0, translation_x, translation_y });
        correction_step.setStepType(StepType::WALKSTEP);
        correction_step.setCharacter(0.7);
        correction_step.setScale(1.0);
        correction_step.setCoordinate(coordinate);
        correction_step.setFoot(Foot::NONE);
        correction_step.setSpeedDirection(Math::fromDegrees(0.0));
        correction_step.setRestriction(RestrictionMode::HARD);
        correction_step.setProtected(false);
        correction_step.setTime(250);

        addStep(correction_step);
      }
      else if (getMotionStatus().stepControl.moveableFoot == (foot == Foot::RIGHT ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT)
        && movableFoot != MotionStatus::StepControlStatus::BOTH)
      {
        // HACKY
        Vector2d ballPosLeftFoot  = getBallModel().positionPreviewInLFoot;
        Vector2d ballPosRightFoot = getBallModel().positionPreviewInRFoot;
        if (foot == Foot::RIGHT
          && ballPosLeftFoot.abs() < ballPosRightFoot.abs())
        {
          StepBufferElement correction_step;
          correction_step.setPose({ 0.0, 0.0, 0.0 });
          correction_step.setStepType(StepType::WALKSTEP);
          correction_step.setCharacter(0.7);
          correction_step.setScale(1.0);
          correction_step.setCoordinate(Coordinate::LFoot);
          correction_step.setFoot(Foot::NONE);
          correction_step.setSpeedDirection(Math::fromDegrees(0.0));
          correction_step.setRestriction(RestrictionMode::HARD);
          correction_step.setProtected(false);
          correction_step.setTime(250);

          addStep(correction_step);

          correction_step.setCoordinate(Coordinate::RFoot);

          addStep(correction_step);
        }
        else if (foot == Foot::LEFT
          && ballPosRightFoot.abs() < ballPosLeftFoot.abs())
        {
          StepBufferElement correction_step;
          correction_step.setPose({ 0.0, 0.0, 0.0 });
          correction_step.setStepType(StepType::WALKSTEP);
          correction_step.setCharacter(0.7);
          correction_step.setScale(1.0);
          correction_step.setCoordinate(Coordinate::RFoot);
          correction_step.setFoot(Foot::NONE);
          correction_step.setSpeedDirection(Math::fromDegrees(0.0));
          correction_step.setRestriction(RestrictionMode::HARD);
          correction_step.setProtected(false);
          correction_step.setTime(250);

          addStep(correction_step);

          correction_step.setCoordinate(Coordinate::LFoot);

          addStep(correction_step);
        }
      }
      return true;
    }
  }

  return false;
}

void PathPlanner2018::forwardKick()
{
  if (!kickPlanned)
  {
    stepBuffer.clear();
    
    // 2019 version - makes sure to kick with the foot that is behind the ball
    Vector2d ballPos;
    Foot actual_foot;
    Coordinate coordinate = Coordinate::Hip;
    if (getBallModel().positionPreview.y < 0)
    {
      coordinate = Coordinate::LFoot;
      actual_foot = Foot::RIGHT;
      ballPos    = getBallModel().positionPreviewInRFoot;
    }
    else
    {
      coordinate = Coordinate::RFoot;
      actual_foot = Foot::LEFT;
      ballPos    = getBallModel().positionPreviewInLFoot;
    }

    /*
    // this was used in 2018
    if (foot == Foot::RIGHT)
    {
      coordinate = Coordinate::LFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::RFoot;
    }
    else
    {
      ASSERT(false);
    }
    */
    
    // Correction step if the movable foot is different from the foot that is supposed to kick
    if (getMotionStatus().stepControl.moveableFoot != (getBallModel().positionPreview.y < 0 ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT))
    {
      StepBufferElement forward_correction_step("forward_correction_step");
      forward_correction_step
        .setPose({ 0.0, 100.0, 0.0 })
        .setStepType(StepType::WALKSTEP)
        .setCharacter(1.0)
        .setScale(1.0)
        .setCoordinate(coordinate)
        .setFoot(Foot::NONE)
        .setSpeedDirection(Math::fromDegrees(0.0))
        .setRestriction(RestrictionMode::HARD)
        .setProtected(false)
        .setTime(250);

      addStep(forward_correction_step);
    }
    
    // The kick
    StepBufferElement forward_kick_step;
    forward_kick_step
      .setPose({ 0.0, 500.0, 0.0 }) // kick straight forward
      .setStepType(StepType::KICKSTEP)
      .setCharacter(1.0)
      .setScale(0.7)
      .setCoordinate(coordinate)
      .setFoot(actual_foot)
      .setSpeedDirection(Math::fromDegrees(0.0))
      .setRestriction(RestrictionMode::SOFT)
      .setProtected(true)
      .setTime(params.forwardKickTime);

    // NOTE: change the kick pose if the parameter is set
    if(params.forwardKickAdaptive) {
      forward_kick_step.setPose({ 0.0, ballPos.x, ballPos.y }); // kick towards the ball
    }

    addStep(forward_kick_step);

    // The zero step
    forward_kick_step.setStepType(StepType::ZEROSTEP);
    addStep(forward_kick_step);

    // The retracting walk step
    forward_kick_step.setPose({ 0.0, 0.0, 0.0 });
    forward_kick_step.setStepType(StepType::WALKSTEP);
    addStep(forward_kick_step);

    kickPlanned = true;
  }
}

void PathPlanner2018::sideKick(const Foot& foot) // Foot == RIGHT means that we want to kick with the right foot to the left side
{
  if (stepBuffer.empty() && !kickPlanned)
  {
    Coordinate coordinate = Coordinate::Hip;
    double stepY          = 0.0;
    double speedDirection = 0.0;

    if (foot == Foot::RIGHT)
    {
      coordinate     = Coordinate::LFoot;
      stepY          = 100.0;
      speedDirection = Math::fromDegrees(90);
    }
    else if (foot == Foot::LEFT)
    {
      coordinate     = Coordinate::RFoot;
      stepY          = -100.0;
      speedDirection = Math::fromDegrees(-90);
    }
    else
    {
      ASSERT(false);
    }

    // The kick
    StepBufferElement new_step;
    new_step.setPose({ 0.0, 500.0, stepY });
    new_step.setStepType(StepType::KICKSTEP);
    new_step.setCharacter(1.0);
    new_step.setScale(1.0);
    new_step.setCoordinate(coordinate);
    new_step.setFoot(foot);
    new_step.setSpeedDirection(speedDirection);
    new_step.setRestriction(RestrictionMode::SOFT);
    new_step.setProtected(true);
    new_step.setTime(params.sideKickTime);

    addStep(new_step);

    // The zero step
    new_step.setStepType(StepType::ZEROSTEP);
    addStep(new_step);

    // The retracting walk step
    new_step.setPose({ 0.0, 0.0, 0.0 });
    new_step.setStepType(StepType::WALKSTEP);
    addStep(new_step);

    kickPlanned = true;
  }
}


void PathPlanner2018::updateSpecificStep(const unsigned int index, StepBufferElement& step)
{
  ASSERT(stepBuffer.size() > 0);
  ASSERT(stepBuffer.size() >= index);

  stepBuffer[index] = step;
}

void PathPlanner2018::manageStepBuffer()
{
  if (stepBuffer.empty())
  {
    return;
  }

  // requested step has been accepted
  if (lastStepRequestID == getMotionStatus().stepControl.stepRequestID)
  {
    /*std::string lastStepType = "";
    if (stepBuffer[0].type == StepType::KICKSTEP)
    {
      lastStepType = "KICKSTEP";
    }
    else if (stepBuffer[0].type == StepType::WALKSTEP)
    {
      lastStepType = "WALKSTEP";
    }
    else if (stepBuffer[0].type == StepType::ZEROSTEP)
    {
      lastStepType = "ZEROSTEP";
    }

    std::cout << "Last executed step: " << lastStepType << " -- " << numPossibleSteps << " > " << params.readyForKickThreshold << " or " << numRotationStepsNecessary << " > " << numPossibleSteps << std::endl;
    */

    stepBuffer.erase(stepBuffer.begin());
    lastStepRequestID = getMotionStatus().stepControl.stepRequestID + 1;
  }
}

void PathPlanner2018::executeStepBuffer()
{
  STOPWATCH_START("PathPlanner2018:execute_steplist");

  if (stepBuffer.empty()) {
    return;
  }

  // normal walking WALKSTEPs use Foot::NONE, for KICKSTEPs the foot to use has to be specified
  if (stepBuffer.front().foot == Foot::NONE)
  {
    switch (getMotionStatus().stepControl.moveableFoot)
    {
    case MotionStatus::StepControlStatus::LEFT:
      footToUse = Foot::LEFT;
      break;
    case MotionStatus::StepControlStatus::RIGHT:
      footToUse = Foot::RIGHT;
      break;
    case MotionStatus::StepControlStatus::BOTH:
      if (stepBuffer.front().pose.translation.y > 0.0f || stepBuffer.front().pose.rotation > 0.0f) {
        footToUse = Foot::LEFT;
      } else {
        footToUse = Foot::RIGHT;
      }
      break;
    case MotionStatus::StepControlStatus::NONE:
      footToUse = Foot::RIGHT;
      break;
    }
  }
  else
  {
    footToUse = stepBuffer.front().foot;
  }

  //set motion request
  getMotionRequest().id                                     = motion::walk;
  getMotionRequest().walkRequest.stepControl.stepID         = getMotionStatus().stepControl.stepID;

  getMotionRequest().walkRequest.coordinate                 = stepBuffer.front().coordinate;
  getMotionRequest().walkRequest.character                  = stepBuffer.front().character;

  getMotionRequest().walkRequest.stepControl.scale          = stepBuffer.front().scale;
  getMotionRequest().walkRequest.stepControl.type           = stepBuffer.front().type;
  getMotionRequest().walkRequest.stepControl.time           = stepBuffer.front().time;
  getMotionRequest().walkRequest.stepControl.speedDirection = stepBuffer.front().speedDirection;
  getMotionRequest().walkRequest.stepControl.target         = stepBuffer.front().pose;
  getMotionRequest().walkRequest.stepControl.restriction    = stepBuffer.front().restriction;
  getMotionRequest().walkRequest.stepControl.isProtected    = stepBuffer.front().isProtected;
  getMotionRequest().walkRequest.stepControl.stepRequestID  = lastStepRequestID;
  getMotionRequest().walkRequest.stepControl.moveLeftFoot   = (footToUse != Foot::RIGHT); // false means right foot
  
  //std::cout << stepBuffer.front().debug_name << " - " << getMotionRequest().walkRequest.stepControl.moveLeftFoot  << std::endl;
  STOPWATCH_STOP("PathPlanner2018:execute_steplist");
}
