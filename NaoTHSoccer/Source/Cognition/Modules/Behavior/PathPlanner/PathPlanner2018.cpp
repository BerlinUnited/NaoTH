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
  kickPlanned(false),
  numPossibleSteps(0.0),
  numPossibleStepsX(0.0),
  numPossibleStepsY(0.0),
  numRotationStepsNecessary(0.0)
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
    moveAroundBall(getPathModel().direction, getPathModel().radius);
    break;
  case PathModel::PathPlanner2018Routine::FORWARDKICK_LEFT:
    //if (farApproach())
    {
      if (nearApproach_forwardKick(Foot::LEFT, 0.0, 0.0))
      {
        forwardKick(Foot::LEFT);
      }
    }
    break;
  case PathModel::PathPlanner2018Routine::FORWARDKICK_RIGHT:
    //if (farApproach())
    {
      if (nearApproach_forwardKick(Foot::RIGHT, 0.0, 0.0))
      {
        forwardKick(Foot::RIGHT);
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
  }

  // Always executed last
  executeStepBuffer();
}

void PathPlanner2018::moveAroundBall(const double direction, const double radius)
{
 if (stepBuffer.empty())
 {
	Vector2d ballPos    = getBallModel().positionPreview;
	double ballRotation = ballPos.angle();
	double ballDistance = ballPos.abs();

	double min1;
	double min2;
	double max1;
	double max2;
	if (direction <= 0)
	{
	  min1 = 0.0;
	  min2 = 0.0;
	  max1 = 45.0;
	  max2 = 100.0;
	}
	else {
	  min1 = -45;
	  min2 = -100;
	  max1 = 0;
	  max2 = 0;
	}

	double stepX = (ballDistance - radius) * std::cos(ballRotation);
	double stepY = Math::clamp(radius * std::tan(Math::fromDegrees(Math::clamp(Math::toDegrees(-direction), min1, max1))), min2, max2) * std::cos(ballRotation);

	Pose2D pose = { ballRotation, stepX, stepY };
  
  StepBufferElement new_step;
  new_step.setPose(pose);
  new_step.setStepType(StepType::WALKSTEP);
  new_step.setCharacter(params.moveAroundBallCharacter);
  new_step.setScale(1.0);
  new_step.setCoordinate(Coordinate::Hip);
  new_step.setFoot(Foot::NONE);
  new_step.setSpeedDirection(Math::fromDegrees(0.0));
  new_step.setRestriction(RestrictionMode::SOFT);
  new_step.setProtected(false);
  new_step.setTime(250);

  addStep(new_step);
  }
}

bool PathPlanner2018::farApproach()
{
  if (stepBuffer.empty())
  {
    Vector2d ballPos = getBallModel().positionPreview;
    numPossibleSteps = ballPos.abs() / params.stepLength;

    if (numPossibleSteps > params.farToNearApproachThreshold)
    {
      double translation_xy = params.stepLength;

      StepBufferElement new_step;
      new_step.setPose({ ballPos.angle(), translation_xy, std::min(translation_xy, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1) });
      new_step.setStepType(StepType::WALKSTEP);
      new_step.setCharacter(0.7);
      new_step.setScale(1.0);
      new_step.setCoordinate(Coordinate::Hip);
      new_step.setFoot(Foot::NONE);
      new_step.setSpeedDirection(Math::fromDegrees(0.0));
      new_step.setRestriction(RestrictionMode::HARD);
      new_step.setProtected(false);
      new_step.setTime(250);

      addStep(new_step);
    }
    else
    {
      return true;
    }
  }

  return false;
}

bool PathPlanner2018::nearApproach_forwardKick(const Foot& foot, const double offsetX, const double offsetY)
{
  // Always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Vector2d ballPos;
    Coordinate coordinate = Coordinate::Hip;

    if (foot == Foot::RIGHT)
    {
      ballPos    = getBallModel().positionPreviewInRFoot;
      coordinate = Coordinate::RFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::LFoot;
      ballPos    = getBallModel().positionPreviewInLFoot;
    }
    else
    {
      ASSERT(false);
    }
    // add the desired offset
    ballPos.x += offsetX;
    ballPos.y += offsetY;

    // TODO: Are there better ways to calculate this?
    numPossibleStepsX = std::abs(ballPos.x) / params.stepLength;
    numPossibleStepsY = std::abs(ballPos.y) / params.stepLength;

    // Am I ready for a kick or still walking to the ball?
    // In other words: Can I only perform one step before touching the ball or more steps?
    if (numPossibleStepsX > params.readyForForwardKickThresholdX
      || numPossibleStepsY > params.readyForForwardKickThresholdY)
    {
	    // generate a correction step
      double translation_xy = params.stepLength;

      //TODO why - std::abs(ballPos.y) => das heißt doch wenn der ball in der y richtung springt wird ein schritt zurück geplant und ausgeführt
      double translation_x = std::min(translation_xy, ballPos.x - getFieldInfo().ballRadius - params.nearApproachForwardKickBallPosOffsetX - std::abs(ballPos.y));
      double translation_y = std::min(translation_xy, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1);

      StepBufferElement new_step;
      new_step.setPose({ 0.0, translation_x, translation_y });
      new_step.setStepType(StepType::WALKSTEP);
      new_step.setCharacter(0.3);
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
      return true;
    }
  }

  return false;
}

// TODO: Has to work without rotation (like nearApproach_forwardKick)
bool PathPlanner2018::nearApproach_sideKick(const Foot& foot, const double offsetX, const double offsetY)
{
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
    numPossibleStepsX = std::abs(ballPos.x) / params.stepLength;
    numPossibleStepsY = std::abs(ballPos.y) / params.stepLength;

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

void PathPlanner2018::forwardKick(const Foot& foot)
{
  if (/*stepBuffer.empty() && */!kickPlanned)
  {
    stepBuffer.clear();

    Coordinate coordinate = Coordinate::Hip;
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

    // Correction step if the movable foot is different from the foot that is supposed to kick
    if (getMotionStatus().stepControl.moveableFoot != (foot == Foot::RIGHT ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT))
    {
      StepBufferElement correction_step;
      correction_step.setPose({ 0.0, 100.0, 0.0 });
      correction_step.setStepType(StepType::WALKSTEP);
      correction_step.setCharacter(1.0);
      correction_step.setScale(1.0);
      correction_step.setCoordinate(coordinate);
      correction_step.setFoot(Foot::NONE);
      correction_step.setSpeedDirection(Math::fromDegrees(0.0));
      correction_step.setRestriction(RestrictionMode::HARD);
      correction_step.setProtected(false);
      correction_step.setTime(250);

      addStep(correction_step);
    }

    // The kick
    StepBufferElement new_step;
    new_step.setPose({ 0.0, 500.0, 0.0 });
    new_step.setStepType(StepType::KICKSTEP);
    new_step.setCharacter(1.0);
    new_step.setScale(0.7);
    new_step.setCoordinate(coordinate);
    new_step.setFoot(foot);
    new_step.setSpeedDirection(Math::fromDegrees(0.0));
    new_step.setRestriction(RestrictionMode::SOFT);
    new_step.setProtected(true);
    new_step.setTime(params.forwardKickTime);

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

void PathPlanner2018::addStep(const StepBufferElement& new_step)
{
  stepBuffer.push_back(new_step);
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

  if (stepBuffer.empty()) 
  {
    return;
  }

  getMotionRequest().id                                     = motion::walk;
  getMotionRequest().walkRequest.coordinate                 = stepBuffer.front().coordinate;
  getMotionRequest().walkRequest.character                  = stepBuffer.front().character;
  getMotionRequest().walkRequest.stepControl.scale          = stepBuffer.front().scale;
  getMotionRequest().walkRequest.stepControl.stepID         = getMotionStatus().stepControl.stepID;
  getMotionRequest().walkRequest.stepControl.type           = stepBuffer.front().type;
  getMotionRequest().walkRequest.stepControl.time           = stepBuffer.front().time;
  getMotionRequest().walkRequest.stepControl.speedDirection = stepBuffer.front().speedDirection;
  getMotionRequest().walkRequest.stepControl.target         = stepBuffer.front().pose;
  getMotionRequest().walkRequest.stepControl.restriction    = stepBuffer.front().restriction;
  getMotionRequest().walkRequest.stepControl.isProtected    = stepBuffer.front().isProtected;
  getMotionRequest().walkRequest.stepControl.stepRequestID  = lastStepRequestID;

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
      if (stepBuffer.front().pose.translation.y > 0.0f || stepBuffer.front().pose.rotation > 0.0f)
      {
        footToUse = Foot::LEFT;
      }
      else
      {
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
  // false means right foot
  getMotionRequest().walkRequest.stepControl.moveLeftFoot = (footToUse != Foot::RIGHT);
  STOPWATCH_STOP("PathPlanner2018:execute_steplist");
}
