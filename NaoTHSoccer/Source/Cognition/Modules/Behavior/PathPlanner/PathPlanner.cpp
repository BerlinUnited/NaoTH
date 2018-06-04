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
last_stepRequestID(getMotionStatus().stepControl.stepRequestID + 1),      // WalkRequest stepRequestID starts at 0, we have to start at 1
kick_planned(false)
{
  DEBUG_REQUEST_REGISTER("PathPlanner:walk_to_ball", "Walks to the ball from far.", false);

  getDebugParameterList().add(&params);
}

PathPlanner::~PathPlanner()
{
  getDebugParameterList().remove(&params);
}

void PathPlanner::execute()
{
  // --- DEBUG REQUESTS ---
  DEBUG_REQUEST("PathPlanner:walk_to_ball",
    if (getBallModel().positionPreview.x > 250) {
      walk_to_ball(Foot::NONE);
    }
  );
  // --- DEBUG REQUESTS ---

  getPathModel().kick_executed = false;

  // Always executed first
  manage_step_buffer();

  // The kick has been executed
  // Tells XABSL to jump into next state
  if (kick_planned && step_buffer.empty()) {
    getPathModel().kick_executed = true;
  }

  // HACK: xabsl set a firced motion request => clear everything
  if(getPathModel().path_routine == PathModel::PathRoutine::NONE && getMotionRequest().forced) {
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
  case PathModel::PathRoutine::GO_TO_BALL_FAST:
    walk_to_ball(Foot::NONE , true);
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
}

void PathPlanner::walk_to_ball(const Foot foot, const bool go_fast)
{
  Vector2d ballPos                   = Vector2d();
  double ballRadius                  = getFieldInfo().ballRadius;
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

  Pose2D pose           = { ballRotation, 0.7*(ballPos.x - getPathModel().distance - ballRadius), ballPos.y };

  if (step_buffer.empty())
  {
    StepType type         = StepType::WALKSTEP;
    double scale          = 1.0;
    double speed_direction = Math::fromDegrees(0.0);
		int kick_time = 250;
    if (go_fast)
    {
      double character = 1.0;
      add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::HARD, false, kick_time);
    }
    else
    {
      double character = 0.3;
      add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::HARD, false, kick_time);
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

  Pose2D pose           = { ballRotation, stepX, stepY };

  if (step_buffer.empty())
  {
    StepType type          = StepType::WALKSTEP;
    double character       = 0.7;
    Foot foot              = Foot::NONE;
    double scale           = 1.0;
    double speed_direction = Math::fromDegrees(0.0);
		int kick_time = 250;
		add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::HARD, false, kick_time);
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
  const double ballRadius            = getFieldInfo().ballRadius;
  
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

  // don't rotate close to the ball
  double stepRotation = ballPos.abs() > 250 ? ballPos.angle() : 0;

  //if (ballPos.x < getPathModel().distance + 30 && ballPos.x > getPathModel().distance - 30)
  if (stepX < 0 && ballPos.x > getPathModel().distance + 30) {
    stepX = 0;
  }

  Pose2D pose = { stepRotation, stepX, stepY };

  // damp the steps while aproaching
  // params.approach_ball_adapt_threshold defines a small circle around the ball without damping
  const double slow_down_factor = 0.7;
  if (params.approach_ball_adapt_control && Vector2d(stepX, stepY).abs() > params.approach_ball_adapt_threshold) {
    pose.translation *= slow_down_factor;
  }

  // approach the ball slowly
  double character = 0.7;
  if (Vector2d(stepX, stepY).abs() < params.approach_ball_slow_distance) {
    character = 0.3;
  }

  if (step_buffer.empty())
  {
    StepType type          = StepType::WALKSTEP;
    //double character       = 0.7;
    double scale           = 1.0;
    double speed_direction = Math::fromDegrees(0.0);
		int kick_time = 250; 
		add_step(pose, type, coordinate, character, Foot::NONE, scale, speed_direction, WalkRequest::StepControlRequest::HARD, false, kick_time);
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
    WalkRequest::Coordinate coordinate = WalkRequest::Hip;

    switch (foot) {
    case Foot::LEFT:
      coordinate = WalkRequest::RFoot;
      break;
    case Foot::RIGHT:
      coordinate = WalkRequest::LFoot;
      break;
    case Foot::NONE:
      ASSERT(false);
    }

    if (step_buffer.empty())
    {
      Pose2D pose            = { 0.0, 500 , 0.0 };
      StepType type          = StepType::KICKSTEP;
      double character       = 1.0;
      double scale           = 0.7;
      double speed_direction = Math::fromDegrees(0.0);
			int kick_time = params.short_kick_time;
			add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::SOFT, true, kick_time);

      type = StepType::ZEROSTEP;
			kick_time = 250;
			add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::SOFT, true, kick_time);
      step_buffer.back().time = 100;

      pose = { 0.0, 0.0, 0.0 };
      type = StepType::WALKSTEP;
			kick_time = 250;
			add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::HARD, true, kick_time);

      kick_planned = true;
    }
  }
}

void PathPlanner::long_kick(const Foot foot)
{
  if (!kick_planned)
  {
    WalkRequest::Coordinate coordinate = WalkRequest::Hip;

    switch (foot) {
    case Foot::LEFT:
      coordinate = WalkRequest::RFoot;
      break;
    case Foot::RIGHT:
      coordinate = WalkRequest::LFoot;
      break;
    case Foot::NONE:
      ASSERT(false);
    }

    if (step_buffer.empty())
    {
      Pose2D pose            = { 0.0, 500, 0.0 };
      StepType type          = StepType::KICKSTEP;
      double character       = 1.0;
      double scale           = 0.7;
      double speed_direction = Math::fromDegrees(0.0);
			int kick_time = params.long_kick_time;
			add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::SOFT, true, kick_time);

      type = StepType::ZEROSTEP;
			kick_time = 250;
			add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::SOFT, true, kick_time);

      pose = { 0.0, 0.0, 0.0 };
      type = StepType::WALKSTEP;
			kick_time = 250;
			add_step(pose, type, coordinate, character, foot, scale, speed_direction, WalkRequest::StepControlRequest::HARD, true, kick_time);

      kick_planned = true;
    }
  }
}

void PathPlanner::sidekick(const Foot foot)
{
  if (!kick_planned)
  {
    double speed_direction             = Math::fromDegrees(0.0);
    double stepY                       = 0.0;
    WalkRequest::Coordinate coordinate = WalkRequest::Hip;

    switch (foot) {
    case Foot::LEFT:
      coordinate     = WalkRequest::LFoot;
      speed_direction = Math::fromDegrees(90);
      stepY          = 100;
      break;
    case Foot::RIGHT:
      coordinate     = WalkRequest::RFoot;
      speed_direction = Math::fromDegrees(-90);
      stepY          = -100;
      break;
    case Foot::NONE:
      ASSERT(false);
    }

    if (step_buffer.empty())
    {
      Pose2D pose = { 0.0, 500, stepY };
      StepType type = StepType::KICKSTEP;
      double character = 1.0;
      Foot step_foot = foot == Foot::RIGHT ? Foot::LEFT : Foot::RIGHT;
      double scale = params.sidekick_scale;

			int kick_time = 250;
			add_step(pose, type, coordinate, character, step_foot, scale, speed_direction, WalkRequest::StepControlRequest::SOFT, true, kick_time);

      type = StepType::ZEROSTEP;
			kick_time = 250;
			add_step(pose, type, coordinate, character, step_foot, scale, speed_direction, WalkRequest::StepControlRequest::SOFT, true, kick_time);
      step_buffer.back().time = 100;

      pose = { 0.0, 0.0, 0.0 };
      type = StepType::WALKSTEP;
			kick_time = 250;
			add_step(pose, type, coordinate, character, step_foot, scale, speed_direction, WalkRequest::StepControlRequest::HARD, true, kick_time);

      kick_planned = true;
    }
  }
}

// Stepcontrol
void PathPlanner::add_step(Pose2D &pose, const StepType &type, const WalkRequest::Coordinate &coordinate, const double character, const Foot foot, const double scale, const double speedDirection, const WalkRequest::StepControlRequest::RestrictionMode restriction, bool isProtected, int kick_time)
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
                                              isProtected}));
}
void PathPlanner::update_step(Pose2D &pose)
{
  ASSERT(step_buffer.size() > 0);

  step_buffer.front().pose = pose;
}

void PathPlanner::manage_step_buffer()
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

void PathPlanner::execute_step_buffer()
{
  STOPWATCH_START("PathPlanner:execute_steplist");

  if (step_buffer.empty()) {
    return;
  }

  getMotionRequest().id                                        = motion::walk;
  getMotionRequest().walkRequest.coordinate                    = step_buffer.front().coordinate;
  getMotionRequest().walkRequest.character                     = step_buffer.front().character;
  getMotionRequest().walkRequest.stepControl.scale             = step_buffer.front().scale;
  getMotionRequest().walkRequest.stepControl.stepID            = getMotionStatus().stepControl.stepID;
  getMotionRequest().walkRequest.stepControl.type              = step_buffer.front().type;
  getMotionRequest().walkRequest.stepControl.time              = step_buffer.front().time;
  getMotionRequest().walkRequest.stepControl.speedDirection    = step_buffer.front().speedDirection;
  getMotionRequest().walkRequest.stepControl.target            = step_buffer.front().pose;
  getMotionRequest().walkRequest.stepControl.restriction       = step_buffer.front().restriction;
  getMotionRequest().walkRequest.stepControl.isProtected       = step_buffer.front().isProtected;
  getMotionRequest().walkRequest.stepControl.stepRequestID     = last_stepRequestID;

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
  STOPWATCH_STOP("PathPlanner:execute_steplist");
}
