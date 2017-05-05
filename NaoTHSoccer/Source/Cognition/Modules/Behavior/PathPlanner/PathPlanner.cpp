/**
 * @file PathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class PathPlanner
 */

#include "PathPlanner.h"

PathPlanner::PathPlanner()
:
step_buffer({}),
foot_to_use(Foot::RIGHT),
last_stepcontrol_stepID(0),
kick_planned(false)
{
  DEBUG_REQUEST_REGISTER("PathPlanner:walk_forward", "Walks forward with y=0 and x=40", false);
}

void PathPlanner::execute()
{
  getPathModel().kick_executed = false;

  DEBUG_REQUEST("PathPlanner:walk_forward",
    Pose2D pose = Pose2D(0.0, 40.0, 0.0);
    WalkRequest::Coordinate coordinate = WalkRequest::Hip;
    if (step_buffer.empty())
    {
      add_step(pose, StepType::WALKSTEP, coordinate);
    }
  );

  STOPWATCH_START("PathPlanner");
  // Always executed first
  manage_step_buffer();


  if(kick_planned && step_buffer.empty()) {
    getPathModel().kick_executed = true;
    kick_planned = false;
  }

  switch (getPathModel().path_routine)
  {
  case PathModel::PathRoutine::NONE:
    // Reset routine_executed, so that XABSL
    // can jump out of option (PathRoutine) that is
    // being executed
    //kick_planned = false;

    if (step_buffer.empty()) {
      return;
    }
    break;
  case PathModel::PathRoutine::GO_TO_BALL_FAST:
    walk_to_ball(Foot::NONE, true);
    break;
  case PathModel::PathRoutine::GO_TO_BALL_SLOW:
    walk_to_ball(Foot::NONE);
    break;
  case PathModel::PathRoutine::MOVE_AROUND_BALL:
    move_around_ball(getPathModel().direction, getPathModel().radius);
    break;
  case PathModel::PathRoutine::APPROACH_BALL_LEFT:
    approach_ball(Foot::LEFT);
    break;
  case PathModel::PathRoutine::APPROACH_BALL_RIGHT:
    approach_ball(Foot::RIGHT);
    break;
  case PathModel::PathRoutine::SHORT_KICK_LEFT:
    short_kick(Foot::LEFT);
    break;
  case PathModel::PathRoutine::SHORT_KICK_RIGHT:
    short_kick(Foot::RIGHT);
    break;
  case PathModel::PathRoutine::LONG_KICK_LEFT:
    long_kick(Foot::LEFT);
    break;
  case PathModel::PathRoutine::LONG_KICK_RIGHT:
    long_kick(Foot::RIGHT);
    break;
  case PathModel::PathRoutine::SIDEKICK_LEFT:
    sidekick(Foot::LEFT);
    break;
  case PathModel::PathRoutine::SIDEKICK_RIGHT:
    sidekick(Foot::RIGHT);
    break;
  }

  // Always executed last
  execute_step_buffer();

  STOPWATCH_STOP("PathPlanner");
}

// Primitive Maneuvers
void PathPlanner::walk_to_ball(const Foot foot, const bool go_fast)
{
  Vector2d ballPos                   = Vector2d();
  WalkRequest::Coordinate coordinate = WalkRequest::Hip;
  switch (foot) {
    case Foot::LEFT:  
      ballPos    = getBallModel().positionPreviewInLFoot; 
      coordinate = WalkRequest::LFoot;
      break;
    case Foot::RIGHT: 
      ballPos    = getBallModel().positionPreviewInRFoot;
      coordinate = WalkRequest::RFoot;
      break;
    case Foot::NONE:  
      ballPos    = getBallModel().positionPreview;
      coordinate = WalkRequest::Hip;
      break;
  }
  double ballRotation = ballPos.angle();

  Pose2D pose = { ballRotation, 0.7*(ballPos.x - getPathModel().distance), ballPos.y };
  if (step_buffer.empty())
  {
    if (go_fast)
    {
      add_step(pose, StepType::WALKSTEP, coordinate, 1.0);
    }
    else
    {
      add_step(pose, StepType::WALKSTEP, coordinate, 0.3);
    }
  }
  else
  {
    update_step(pose);
  }
}

void PathPlanner::move_around_ball(const double direction, const double radius)
{
  Vector2d ballPos                   = getBallModel().positionPreview;
  double ballRotation                = ballPos.angle();
  double ballDistance                = ballPos.abs();
  WalkRequest::Coordinate coordinate = WalkRequest::Hip;

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
  double stepY = Math::clamp(radius * std::tan(Math::clamp(Math::toDegrees(-direction), min1, max1)), min2, max2) * std::cos(ballRotation);
  Pose2D pose = { ballRotation, stepX, stepY };

  if (step_buffer.empty())
  {
    add_step(pose, StepType::WALKSTEP, coordinate, 0.7);
  }
  else
  {
    update_step(pose);
  }
}

void PathPlanner::approach_ball(const Foot foot)
{
  Vector2d ballPos                   = Vector2d();
  WalkRequest::Coordinate coordinate = WalkRequest::Hip;
  double stepX                       = 0.0;
  double stepY                       = 0.0;
  double ballRadius                  = getFieldInfo().ballRadius;
  double stepRotation                = ballPos.abs() > 250 ? ballPos.angle() : 0;

  switch (foot) {
  case Foot::LEFT:  
    ballPos    = getBallModel().positionPreviewInLFoot;
    coordinate = WalkRequest::LFoot;
    stepX      = ballPos.x - std::abs(ballPos.y - getPathModel().yOffset) - getPathModel().distance - ballRadius;
    stepY      = ballPos.y - getPathModel().yOffset;
    break;
  case Foot::RIGHT: 
    ballPos    = getBallModel().positionPreviewInRFoot;
    coordinate = WalkRequest::RFoot;
    stepX      = ballPos.x - std::abs(ballPos.y + getPathModel().yOffset) - getPathModel().distance - ballRadius;
    stepY      = ballPos.y + getPathModel().yOffset;
    break;
  case Foot::NONE:
    ASSERT(false);
  }

  Pose2D pose = { stepRotation, 0.7 * stepX, 0.7 * stepY };

  if (step_buffer.empty())
  {
    add_step(pose, StepType::WALKSTEP, coordinate, 0.7);
  }
  else
  {
    update_step(pose);
  }
}

void PathPlanner::short_kick(const Foot foot)
{
  if (!kick_planned)
  {
    Vector2d ballPos                   = Vector2d();
    WalkRequest::Coordinate coordinate = WalkRequest::Hip;

    switch (foot) {
    case Foot::LEFT:  
      ballPos    = getBallModel().positionPreviewInLFoot;
      coordinate = WalkRequest::LFoot;
      break;
    case Foot::RIGHT:
      ballPos    = getBallModel().positionPreviewInRFoot;
      coordinate = WalkRequest::RFoot;
      break;
    case Foot::NONE:
      ASSERT(false);
    }

    if (step_buffer.empty())
    {
      Pose2D pose = { 0.0, ballPos.x + 500 , 0.0 };
      add_step(pose, StepType::KICKSTEP, coordinate, 1.0, foot, 0.7);

      add_step(pose, StepType::ZEROSTEP, coordinate, foot);

      pose = { 0.0, 0.0, 0.0 };
      add_step(pose, StepType::WALKSTEP, coordinate, foot);

      kick_planned = true;
    }
  }
}

void PathPlanner::long_kick(const Foot foot)
{
  if (!kick_planned)
  {
    Vector2d ballPos                   = Vector2d();
    WalkRequest::Coordinate coordinate = WalkRequest::Hip;

    switch (foot) {
    case Foot::LEFT:
      ballPos    = getBallModel().positionPreviewInLFoot;
      coordinate = WalkRequest::LFoot;
      break;
    case Foot::RIGHT:
      ballPos    = getBallModel().positionPreviewInRFoot;
      coordinate = WalkRequest::RFoot;
      break;
    case Foot::NONE:
      ASSERT(false);
    }

    if (step_buffer.empty())
    {
      Pose2D pose = { 0.0, ballPos.x + 500, 0.0 };
      add_step(pose, StepType::KICKSTEP, coordinate, 1.0, foot, 0.7);

      add_step(pose, StepType::ZEROSTEP, coordinate, foot);

      pose = { 0.0, 0.0, 0.0 };
      add_step(pose, StepType::WALKSTEP, coordinate, foot);

      kick_planned = true;
    }
  }
}

void PathPlanner::sidekick(const Foot foot)
{
  if (!kick_planned)
  {
    double speedDirection              = 0.0;
    double stepY                       = 0.0;
    Vector2d ballPos                   = Vector2d(); 
    WalkRequest::Coordinate coordinate = WalkRequest::Hip;

    switch (foot) {
    case Foot::LEFT:
      ballPos        = getBallModel().positionPreviewInLFoot; 
      coordinate     = WalkRequest::LFoot;
      speedDirection = 90;
      stepY          = 100;
      break;
    case Foot::RIGHT:
      ballPos        = getBallModel().positionPreviewInRFoot; 
      coordinate     = WalkRequest::RFoot;
      speedDirection = -90;
      stepY          = -100;
      break;
    case Foot::NONE:
      ASSERT(false);
    }

    if (step_buffer.empty())
    {
      Pose2D pose = { 0.0, 500, stepY };
      add_step(pose, StepType::KICKSTEP, coordinate, 1.0, foot == Foot::RIGHT ? Foot::LEFT : Foot::RIGHT, 1.0, speedDirection);

      add_step(pose, StepType::ZEROSTEP, coordinate, foot == Foot::RIGHT ? Foot::LEFT : Foot::RIGHT);

      pose = { 0.0, 0.0, 0.0 };
      add_step(pose, StepType::WALKSTEP, coordinate, foot == Foot::RIGHT ? Foot::LEFT : Foot::RIGHT);

      kick_planned = true;
    }
  }
}

// Stepcontrol
void PathPlanner::add_step(Pose2D &pose, const StepType &type, const WalkRequest::Coordinate &coordinate, const double character, const Foot foot, const double scale, const double speedDirection)
{
  // taken out of the stepplanner
  // limiting the steps if walksteps
  // 0.75 because 0.5 * character(usually 0.5) + 0.5 (taken from stepplanner)
  if (type == StepType::WALKSTEP)
  {
    double maxStepTurn = Math::fromDegrees(30) * 0.75;
    double maxStep     = 40.0f;
    pose.rotation      = Math::clamp(pose.rotation, -maxStepTurn, maxStepTurn);
    pose.translation.x = Math::clamp(pose.translation.x, -maxStep, maxStep) * cos(pose.rotation / maxStepTurn * Math::pi / 2);
    pose.translation.y = Math::clamp(pose.translation.y, -maxStep, maxStep) * cos(pose.rotation / maxStepTurn * Math::pi / 2);
  }

  step_buffer.push_back(Step_Buffer_Element({ pose, 
                                              speedDirection, 
                                              type, 
                                              type == StepType::KICKSTEP ? 300 : 250, 
                                              character,
                                              scale,
                                              foot,
                                              coordinate}));
}
void PathPlanner::update_step(Pose2D &pose)
{
  ASSERT(step_buffer.size() > 0);
  // taken out of the stepplanner
  // limiting the steps if walksteps
  // 0.75 because 0.5 * character(usually 0.5) + 0.5 (taken from stepplanner)
  if (step_buffer.front().type == StepType::WALKSTEP)
  {
    double maxStepTurn = Math::fromDegrees(30) * 0.75;
    double maxStep     = 40.0f;
    pose.rotation      = Math::clamp(pose.rotation, -maxStepTurn, maxStepTurn);
    pose.translation.x = Math::clamp(pose.translation.x, -maxStep, maxStep) * cos(pose.rotation / maxStepTurn * Math::pi / 2);
    pose.translation.y = Math::clamp(pose.translation.y, -maxStep, maxStep) * cos(pose.rotation / maxStepTurn * Math::pi / 2);
  }

  Step_Buffer_Element updatedElement = step_buffer.front();
  updatedElement.pose                = pose;
  step_buffer.front()                = updatedElement;
}

void PathPlanner::manage_step_buffer() 
{
  if (step_buffer.empty()) {
    return;
  }

  // stepID higher than the last one means, stepControl request with the
  // last_stepcontrol_stepID has been accepted
  if (last_stepcontrol_stepID < getMotionStatus().stepControl.stepID)
  {
    step_buffer.erase(step_buffer.begin());
    last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
  }

  // Correct last_stepcontrol_stepID if neccessary
  if (getMotionStatus().stepControl.stepID < last_stepcontrol_stepID) {
    last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
  }
}

void PathPlanner::execute_step_buffer()
{
  if (step_buffer.empty()) {
    return;
  }

  STOPWATCH_START("PathPlanner:execute_steplist");
  getMotionRequest().id = motion::walk;
  getMotionRequest().standardStand = false;
  getMotionRequest().walkRequest.coordinate                 = step_buffer.front().coordinate;
  getMotionRequest().walkRequest.character                  = step_buffer.front().character;
  getMotionRequest().walkRequest.stepControl.scale          = step_buffer.front().scale;
  getMotionRequest().walkRequest.stepControl.stepID         = getMotionStatus().stepControl.stepID;
  getMotionRequest().walkRequest.stepControl.type           = step_buffer.front().type;
  getMotionRequest().walkRequest.stepControl.time           = step_buffer.front().time;
  getMotionRequest().walkRequest.stepControl.speedDirection = step_buffer.front().speedDirection;
  getMotionRequest().walkRequest.stepControl.target         = step_buffer.front().pose;

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
      // TODO: choose foot more intelligently when both feet are usable
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
  getMotionRequest().walkRequest.stepControl.moveLeftFoot = foot_to_use == Foot::RIGHT ? false : true;
  STOPWATCH_STOP("PathPlanner:execute_steplist");
}