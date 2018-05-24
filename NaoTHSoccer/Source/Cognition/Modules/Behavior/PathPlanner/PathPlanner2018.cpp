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
  numStepsGeometric(0),
  numStepsCorrection(0),
  numStepsPlanned(0)
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

  // HACK: xabsl set a forced motion request => clear everything
  if (getPathModel().path_routine == PathModel::PathRoutine::NONE && getMotionRequest().forced) {
    stepBuffer.clear();
    return;
  }

  switch (getPathModel().path2018_routine)
  {
  case PathModel::PathPlanner2018Routine::NONE:
    // TODO: should the stepBuffer just be cleared here no matter what?
    if (stepBuffer.empty()) 
    {
      numStepsGeometric  = 0;
      numStepsCorrection = 0;
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
  // TODO: when do I want to flush this if I ever want to?
  if (stepBuffer.empty())
  {
    Vector2d ball_pos;
    Coordinate coordinate;

    if (foot == Foot::RIGHT)
    {
      ball_pos   = getBallModel().positionPreviewInRFoot;
      coordinate = Coordinate::RFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::LFoot;
      ball_pos   = getBallModel().positionPreviewInLFoot;
    }
    const double ball_radius = getFieldInfo().ballRadius;
    const double ball_rot    = ball_pos.angle();

    // TODO: Right now this is a very simple path planned to do step coordination
    //       we want to replace this with more sophisticated ways of acquiring a path
    double numEquidistanSteps = ball_pos.abs() / params.stepLength;

    // Am I ready for a kick or still walking to the ball?
    // In other words: Can I only perform one step before touching the ball or more steps?
    if (numEquidistanSteps > 1 + ball_radius / params.stepLength)
    {
      // If the number of possible steps is even but the foot that is movable
      // is the foot that is supposed to kick we need to make a correction step first
      if (ball_pos.abs() < 400
        && getMotionStatus().stepControl.moveableFoot == foot
        && static_cast<int>(std::ceil(numEquidistanSteps)) % 2 == 0)
      {
        const double translation_xy = params.stepLength * 0.75;
        StepBufferElement correction_step;
        correction_step.setPose({ ball_rot, std::min(translation_xy, ball_pos.x), std::min(translation_xy, ball_pos.y) });
        correction_step.setStepType(StepType::WALKSTEP);
        correction_step.setCharacter(0.7);
        correction_step.setScale(1.0);
        correction_step.setCoordinate(coordinate);
        correction_step.setFoot(foot);
        correction_step.setSpeedDirection(Math::fromDegrees(0.0));
        correction_step.setRestriction(RestrictionMode::HARD);
        correction_step.setProtected(false);
        correction_step.setTime(250);

        addStep(correction_step);
      }
      else
      {
        const double translation_xy = params.stepLength;
        StepBufferElement new_step;
        new_step.setPose({ ball_rot, std::min(translation_xy, ball_pos.x), std::min(translation_xy, ball_pos.y) });
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
    }
    else
    {
      return true;
    }
  }

  return false;
}

void PathPlanner2018::forwardKick(const Foot& foot)
{
  if (stepBuffer.empty())
  {
    Coordinate coordinate;
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
