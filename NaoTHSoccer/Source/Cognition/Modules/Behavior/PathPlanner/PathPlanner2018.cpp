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
    // Calculate the number of geometrically possible steps towards the ball

    // ASSUMPTION: the limiting of a step translation regarding the step rotation
    // is a sphere (this is wrong, it actually is an ellipse)
    // TODO: Calculate this in a more realistic way (is this necessary?)
    // ASSUMPTION 2: Calculating this with hip coordinates will be accurate enough
    // TODO: Check assumption 2 and if this isn't the case, fix this!
    // ASSUMPTION 3: No matter what, the last 3 steps will be a kick, a zero step and then a retracting walk step
    numStepsGeometric = Math::clamp(static_cast<int>(getBallModel().positionPreview.abs() / params.stepLength), 0, 10) + 2;

    // This variable is only for convenience right now
    numStepsPlanned = numStepsGeometric + numStepsCorrection;

    if (forwardKick())
    {
      // TODO: forward kick is done, what now?
    }
    break;
  }

  // Always executed last
  executeStepBuffer();
}

bool PathPlanner2018::forwardKick()
{
  Vector2d ball_pos        = getBallModel().positionPreviewInRFoot;
  const double ball_radius = getFieldInfo().ballRadius;

  if (stepBuffer.size() > numStepsPlanned)
  {
    stepBuffer.clear();
  }

  if (numStepsPlanned > 0)
  {
    double translation_x = std::min(std::abs(ball_pos.x), params.stepLength) * (ball_pos.x < 0 ? -1 : 1);
    double translation_y = std::min(std::abs(ball_pos.y), params.stepLength) * (ball_pos.y < 0 ? -1 : 1);
    double rotation = ball_pos.angle();

    Pose2D pose = { rotation, translation_x, translation_y };

    // Mechanism to make sure that the right foot executes the kick
    // This has to come after clearing the step buffer if buffer size > num_steps so it doesn't get deleted!
    // When we are 6 steps away from a kick we check that the right foot is movable
    // TODO: make this generic for right and left foot
    if (getMotionStatus().stepControl.moveableFoot == MotionStatus::StepControlStatus::RIGHT
      && numStepsGeometric == 6
      && numStepsCorrection == 0)
    {
      Pose2D correction_pose = { rotation, translation_x / 2, translation_y / 2 };

      StepBufferElement correction_step;
      correction_step.setPose(correction_pose);
      correction_step.setStepType(StepType::WALKSTEP);
      correction_step.setCharacter(0.7);
      correction_step.setScale(1.0);
      correction_step.setCoordinate(WalkRequest::RFoot);
      correction_step.setFoot(Foot::LEFT);
      correction_step.setSpeedDirection(Math::fromDegrees(0.0));
      correction_step.setRestriction(WalkRequest::StepControlRequest::HARD);
      correction_step.setProtected(false);
      correction_step.setTime(300);

      addStep(correction_step);

      correction_step.setFoot(Foot::LEFT);

      addStep(correction_step);

      numStepsCorrection = numStepsCorrection + 2;
    }
    else
    {
      numStepsCorrection = 0;
    }

    // TODO: Is this kind of comment overkill? - y030
    //                             { ------------------------------------ numStepsPlanned -------------------------------- }
    //                             { ----- numStepsCorrection ------ }  { ------------ numStepsGeometric ----------------- }
    // The buffer looks like this: ( correction step, correction step), [geometric, ... x9, kick step, zero step, walk step]
    // We only want to loop over the second part in square brackets and leave the first part in brackets alone
    // That is why we start with numStepsCorrection - 1
    for (unsigned int i = numStepsCorrection - 1; i < numStepsPlanned; ++i)
    {
      StepBufferElement new_step;
      new_step.setPose(pose);
      new_step.setStepType(StepType::WALKSTEP);
      new_step.setCharacter(0.7);
      new_step.setScale(1.0);
      new_step.setCoordinate(WalkRequest::RFoot);
      new_step.setFoot(Foot::NONE);
      new_step.setSpeedDirection(Math::fromDegrees(0.0));
      new_step.setRestriction(WalkRequest::StepControlRequest::HARD);
      new_step.setProtected(false);
      new_step.setTime(300);

      if (i == numStepsPlanned - 3) // last three steps are kickstep -> zerostep -> retracting walkstep
      {
        pose = { 0.0, 500.0, 0.0 };
        new_step.setPose(pose);
        new_step.setStepType(StepType::KICKSTEP);
        new_step.setFoot(Foot::RIGHT);
        new_step.setCharacter(0.7);
        new_step.setScale(1.0);
        new_step.setRestriction(WalkRequest::StepControlRequest::SOFT);
        new_step.setProtected(true);
      }
      else if (i == numStepsPlanned - 2)
      {
        new_step.setStepType(StepType::ZEROSTEP);
        new_step.setFoot(Foot::RIGHT);
      }
      else if (i == numStepsPlanned - 1)
      {
        pose = { 0.0, 0.0, 0.0 };
        new_step.setPose(pose);
        new_step.setStepType(StepType::WALKSTEP);
        new_step.setFoot(Foot::RIGHT);
      }

      // We either fill the stepBuffer up till we have numStepsPlanned steps
      // or we update the steps inside stepBuffer
      if (stepBuffer.empty() || stepBuffer.size() <= i)
      {
        addStep(new_step);
      }
      // We do not want to update the correction steps
      // TODO: Do we really not want to ?
      else if (stepBuffer.size() > i && i >= numStepsCorrection)
      {
        updateSpecificStep(i, new_step);
      }
    }
  }
  else
  {
    stepBuffer.clear();
    return true;
  }

  return false;
}

// Stepcontrol
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

// TODO: This mechanism has become obsolete (has it?)
//       since steps get erased from the buffer when buffer size is bigger than
//       number of steps possible geometrically and added handcrafted ones
//       but we still have to update last_stepRequestID
//       ---!!! THIS HAS TO BE THOUGHT THROUGH AGAIN !!!---
void PathPlanner2018::manageStepBuffer()
{
  if (stepBuffer.empty()) 
  {
    return;
  }

  // requested step has been accepted
  if (lastStepRequestID == getMotionStatus().stepControl.stepRequestID)
  {
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
