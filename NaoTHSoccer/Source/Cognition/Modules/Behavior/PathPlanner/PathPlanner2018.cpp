/**
* @file PathPlanner2018.cpp
*
* @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
* Implementation of class PathPlanner2018
*/

#include "PathPlanner2018.h"

PathPlanner2018::PathPlanner2018()
  :
  step_buffer({}),
  foot_to_use(Foot::RIGHT),
  last_stepRequestID(getMotionStatus().stepControl.stepRequestID + 1),      // WalkRequest stepRequestID starts at 0, we have to start at 1
  kick_planned(false)
{
  //DEBUG_REQUEST_REGISTER("PathPlanner2018:walk_to_ball", "Walks to the ball from far.", false);

  getDebugParameterList().add(&params);
}

PathPlanner2018::~PathPlanner2018()
{
  getDebugParameterList().remove(&params);
}

void PathPlanner2018::execute()
{
  // --- DEBUG REQUESTS ---
  /*DEBUG_REQUEST("PathPlanner2018:walk_to_ball",
    if (getBallModel().positionPreview.x > 250) {
      walk_to_ball(Foot::NONE);
    }
  );*/
  // --- DEBUG REQUESTS ---

  getPathModel().kick_executed = false;

  // Always executed first
  manage_step_buffer();

  // The kick has been executed
  // Tells XABSL to jump into next state
  if (kick_planned && step_buffer.empty()) {
    getPathModel().kick_executed = true;
  }

  // HACK: xabsl set a forced motion request => clear everything
  if (getPathModel().path_routine == PathModel::PathRoutine::NONE && getMotionRequest().forced) {
    step_buffer.clear();
    return;

  }

  switch (getPathModel().path_routine)
  {
  case PathModel::PathRoutine::NONE:
    // There is no kick planned, since the kick has been executed
    // and XABSL is in a different state now
    if (kick_planned) {
      kick_planned = false;
    }

    if (step_buffer.empty()) {
      return;
    }
    break;
  case PathModel::PathPlanner2018Routine::GO:
    acquire_ball_control();
    break;
  }

  // Always executed last
  execute_step_buffer();
}

// STEP 1: If too far away from the ball,
// we want to approach the ball and acquire control over it
bool PathPlanner2018::acquire_ball_control()
{
  return false;
}

// STEP 2: If we have control over the ball,
// we want to maintain it; if it moves, we move with it
// and stay close enough to it
void PathPlanner2018::maintain_ball_control()
{

}

// STEP 3: If we have control over the ball,
// we want to interact with it
bool PathPlanner2018::execute_action()
{
  return false;
}


// Stepcontrol
void PathPlanner2018::add_step(Pose2D &pose, const StepType &type, const WalkRequest::Coordinate &coordinate, const double character, const Foot foot, const double scale, const double speedDirection, const WalkRequest::StepControlRequest::RestrictionMode restriction, bool isProtected, int kick_time)
{
  step_buffer.push_back(Step_Buffer_Element({ pose,
    speedDirection,
    type,
    kick_time,
    character,
    scale,
    foot,
    coordinate,
    restriction,
    isProtected }));
}
void PathPlanner2018::update_step(Pose2D &pose)
{
  ASSERT(step_buffer.size() > 0);

  step_buffer.front().pose = pose;
}

void PathPlanner2018::manage_step_buffer()
{
  if (step_buffer.empty()) {
    return;
  }

  // requested step has been accepted
  if (last_stepRequestID == getMotionStatus().stepControl.stepRequestID)
  {
    step_buffer.erase(step_buffer.begin());
    last_stepRequestID = getMotionStatus().stepControl.stepRequestID + 1;
  }
}

void PathPlanner2018::execute_step_buffer()
{
  STOPWATCH_START("PathPlanner2018:execute_steplist");

  if (step_buffer.empty()) {
    return;
  }

  getMotionRequest().id = motion::walk;
  getMotionRequest().walkRequest.coordinate = step_buffer.front().coordinate;
  getMotionRequest().walkRequest.character = step_buffer.front().character;
  getMotionRequest().walkRequest.stepControl.scale = step_buffer.front().scale;
  getMotionRequest().walkRequest.stepControl.stepID = getMotionStatus().stepControl.stepID;
  getMotionRequest().walkRequest.stepControl.type = step_buffer.front().type;
  getMotionRequest().walkRequest.stepControl.time = step_buffer.front().time;
  getMotionRequest().walkRequest.stepControl.speedDirection = step_buffer.front().speedDirection;
  getMotionRequest().walkRequest.stepControl.target = step_buffer.front().pose;
  getMotionRequest().walkRequest.stepControl.restriction = step_buffer.front().restriction;
  getMotionRequest().walkRequest.stepControl.isProtected = step_buffer.front().isProtected;
  getMotionRequest().walkRequest.stepControl.stepRequestID = last_stepRequestID;

  // normal walking WALKSTEPs use Foot::NONE, for KICKSTEPs the foot to use has to be specified
  if (step_buffer.front().foot == Foot::NONE)
  {
    switch (getMotionStatus().stepControl.moveableFoot)
    {
    case MotionStatus::StepControlStatus::LEFT:
      foot_to_use = Foot::LEFT;
      break;
    case MotionStatus::StepControlStatus::RIGHT:
      foot_to_use = Foot::RIGHT;
      break;
    case MotionStatus::StepControlStatus::BOTH:
      if (step_buffer.front().pose.translation.y > 0.0f || step_buffer.front().pose.rotation > 0.0f)
      {
        foot_to_use = Foot::LEFT;
      }
      else
      {
        foot_to_use = Foot::RIGHT;
      }
      break;
    case MotionStatus::StepControlStatus::NONE:
      foot_to_use = Foot::RIGHT;
      break;
    }
  }
  else
  {
    foot_to_use = step_buffer.front().foot;
  }
  // false means right foot
  getMotionRequest().walkRequest.stepControl.moveLeftFoot = (foot_to_use != Foot::RIGHT);
  STOPWATCH_STOP("PathPlanner2018:execute_steplist");
}
