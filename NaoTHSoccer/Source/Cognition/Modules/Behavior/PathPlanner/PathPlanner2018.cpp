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
  numPossibleSteps(0.0)
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
  if (kickPlanned && stepBuffer.empty()) {
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
  case PathModel::PathPlanner2018Routine::GO:
    if (acquireBallControl(Foot::LEFT))
    {
      forwardKick(Foot::LEFT);
    }
    break;
  }

  // Always executed last
  executeStepBuffer();
}

bool PathPlanner2018::acquireBallControl(const Foot& foot)
{
  // I always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Vector2d ballPos;
    Coordinate coordinate = Coordinate::Hip;

    if (foot == Foot::RIGHT)
    {
      ballPos   = getBallModel().positionPreviewInRFoot;
      coordinate = Coordinate::RFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::LFoot;
      ballPos   = getBallModel().positionPreviewInLFoot;
    }
    else
    {
      ASSERT(false);
    }
    const double ballRot = ballPos.angle();

    // TODO: Are there better ways to calculate this?
    numPossibleSteps = ballPos.abs() / params.stepLength;
    double numRotationsNecessary = ballRot / params.rotationLength;

    // Am I ready for a kick or still walking to the ball?
    // In other words: Can I only perform one step before touching the ball or more steps?
    if (numPossibleSteps > params.readyForKickThreshold
      || numRotationsNecessary > numPossibleSteps)
    {
      double translation_xy = params.stepLength;
      // TODO: Are there better ways to incorporate the rotation?
      if (numRotationsNecessary > numPossibleSteps)
      {
        translation_xy = 0.0;
      }
      StepBufferElement new_step;
      new_step.setPose({ ballRot, std::min(translation_xy, ballPos.x), std::min(translation_xy, ballPos.y) });
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
      // Correction step if the movable foot is different from the foot that is supposed to kick
      if (getMotionStatus().stepControl.moveableFoot != (foot == Foot::RIGHT ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT))
      {
        const double translation_xy = params.stepLength;
        StepBufferElement correction_step;
        correction_step.setPose({ ballRot, std::min(translation_xy, ballPos.x), std::min(translation_xy, ballPos.y) });
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
      return true;
    }
  }

  return false;
}

void PathPlanner2018::forwardKick(const Foot& foot)
{
  if (stepBuffer.empty() && !kickPlanned)
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

    switch (foot)
    {
    case Foot::LEFT:
      coordinate = Coordinate::RFoot;
      break;
    case Foot::RIGHT:
      coordinate = Coordinate::LFoot;
      break;
    case Foot::NONE:
      ASSERT(false);
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
    new_step.setTime(300);

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
    
    std::cout << "Last executed step: " << lastStepType << " -- " << numPossibleSteps << " > " << params.readyForKickThreshold + getFieldInfo().ballRadius / params.stepLength << std::endl;
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
