/**
 * @file PrimitiveManeuvers.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class PrimitiveManeuvers
 */

#include "PrimitiveManeuvers.h"
#include <iostream>

PrimitiveManeuvers::PrimitiveManeuvers()
:
step_list({}),
foot_to_be_used(PrimitiveManeuvers::Foot::Right),
last_stepcontrol_stepID(0)
{
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:execute_step_list", "Executes the step list", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:walk_to_ball", "Walks to the ball.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:move_around_ball", "Moves around the ball.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:go_to_ball_dynamic_right", "Walks to the ball.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:go_to_ball_dynamic_left", "Walks to the ball.", false);

  // Helpers
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:helper:forward", "Walks forward.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:helper:backward", "Walks backward.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:helper:sidestep_right", "Sidestep to the right.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:helper:sidestep_left", "Sidestep to the left.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:helper:turn_left", "Turns left.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:helper:turn_right", "Turns right.", false);
}

void PrimitiveManeuvers::execute()
{
  STOPWATCH_START("PrimitiveManeuvers");
  manage_step_buffer();


  if (getPathModel().pathType == PathModel::PathType::none)
  {
    if (step_list.empty()) {
      return;
    }
    //getMotionRequest().id = motion::stand;
  }
  else if (getPathModel().pathType == PathModel::PathType::go_to_ball_left)
  {
    PrimitiveManeuvers::MWalk_to_ball(Foot::Left);
  }
  else if (getPathModel().pathType == PathModel::PathType::go_to_ball_right)
  {
    PrimitiveManeuvers::MWalk_to_ball(Foot::Right);
  }
  else if (getPathModel().pathType == PathModel::PathType::go_to_ball)
  {
    PrimitiveManeuvers::MWalk_to_ball(Foot::NONE);
  }
  else if (getPathModel().pathType == PathModel::PathType::move_around_ball)
  {
    PrimitiveManeuvers::MMove_around_ball(getPathModel().move_around_ball_direction, getPathModel().move_around_ball_radius);
  }
  else if (getPathModel().pathType == PathModel::PathType::go_to_ball_dynamic_left)
  {
    PrimitiveManeuvers::MWalk_to_ball_dynamic(Foot::Left);
  }
  else if (getPathModel().pathType == PathModel::PathType::go_to_ball_dynamic_right)
  {
    PrimitiveManeuvers::MWalk_to_ball_dynamic(Foot::Right);
  }
  else if (getPathModel().pathType == PathModel::PathType::prepare_walk_back_approach)
  {
    if (std::abs(getBallModel().positionPreview.y) < 300)
    {
      add(new_step(0.0, 40.0, 0.0));
    }
    else
    {
      add(new_step(40.0, 0.0, 0.0));
    }
  }
  else if (getPathModel().pathType == PathModel::PathType::fast_forward_left)
  {
    PrimitiveManeuvers::MFast_forward_kick(Foot::Left);
  }
  else if (getPathModel().pathType == PathModel::PathType::fast_forward_right)
  {
    PrimitiveManeuvers::MFast_forward_kick(Foot::Right);
  }
  else if (getPathModel().pathType == PathModel::PathType::kick_with_foot_right)
  {
    PrimitiveManeuvers::MKick_with_foot(Foot::Right);
  }
  else if (getPathModel().pathType == PathModel::PathType::kick_with_foot_left)
  {
    PrimitiveManeuvers::MKick_with_foot(Foot::Left);
  }
  else if (getPathModel().pathType == PathModel::PathType::sidekick_left)
  {
    PrimitiveManeuvers::MSidekick(Foot::Left);
  }
  else if (getPathModel().pathType == PathModel::PathType::sidekick_right)
  {
    PrimitiveManeuvers::MSidekick(Foot::Right);
  }
  STOPWATCH_STOP("PrimitiveManeuvers");

  // DEBUG REQUESTS
  // Maneuvers
  DEBUG_REQUEST("PrimitiveManeuvers:walk_to_ball",
	prepare_walk();
	PrimitiveManeuvers::MWalk_to_ball(Foot::Right);
  );
  DEBUG_REQUEST("PrimitiveManeuvers:move_around_ball",
	prepare_walk();
	PrimitiveManeuvers::MMove_around_ball(Foot::Right, true);
  );
  DEBUG_REQUEST("PrimitiveManeuvers:go_to_ball_dynamic_right",
	prepare_walk();
	PrimitiveManeuvers::MWalk_to_ball_dynamic(Foot::Right);
  );
  DEBUG_REQUEST("PrimitiveManeuvers:go_to_ball_dynamic_left",
    prepare_walk();
  PrimitiveManeuvers::MWalk_to_ball_dynamic(Foot::Left);
  );

  // Helpers
  DEBUG_REQUEST("PrimitiveManeuvers:helper:forward",
	add(new_step(40.0f, 0.0f, 0.0f));
  );
  DEBUG_REQUEST("PrimitiveManeuvers:helper:backward",
	add(new_step(-40.0f, 0.0f, 0.0f));
  );
  DEBUG_REQUEST("PrimitiveManeuvers:helper:sidestep_right",
	  add(new_step(0.0f, -40.0f, 0.0f));
  );
  DEBUG_REQUEST("PrimitiveManeuvers:helper:sidestep_left",
	  add(new_step(0.0f, 40.0f, 0.0f));
  );
  DEBUG_REQUEST("PrimitiveManeuvers:helper:turn_left",
	add(new_step(0.0f, 0.0f, 0.5f));
  );
  DEBUG_REQUEST("PrimitiveManeuvers:helper:turn_right",
	add(new_step(0.0f, 0.0f, -0.5f));
  );



  execute_step();
}

// Primitive Maneuvers
void PrimitiveManeuvers::MWalk_to_ball(Foot foot)
{
  Vector2d ballPos;
  switch (foot) {
    case Foot::Left:  ballPos = getBallModel().positionPreviewInLFoot; break;
    case Foot::Right: ballPos = getBallModel().positionPreviewInRFoot; break;
    case Foot::NONE:  ballPos = getBallModel().positionPreview; break;
  }
  double ballRotation = ballPos.angle();
  //double ballDistance = ballPos.abs();

  Vector3d step = limit_step(Vector3d(ballPos.x - getPathModel().goto_distance, 0.0f, ballRotation));

  add(new_step(step.x, step.y, step.z));

  /*// Add a correcting step if neccessary
  if ((getBallModel().positionPreviewInRFoot.x < getBallModel().positionPreviewInLFoot.x && foot == Foot::Right) 
        || (getBallModel().positionPreviewInLFoot.x < getBallModel().positionPreviewInRFoot.x && foot == Foot::Left))
  {
    add(new_step(step));
  }*/
}

void PrimitiveManeuvers::MMove_around_ball(double direction, double radius)
{
  Vector2d ballPos = getBallModel().positionPreview;
 
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
  double stepY = Math::clamp(radius * std::tan(Math::clamp(Math::toDegrees(-direction), min1, max1)), min2, max2) * std::cos(ballRotation);

  Vector3d step = limit_step(Vector3d(stepX, stepY, ballRotation));

  add(new_step(step.x, step.y, step.z));
}

void PrimitiveManeuvers::MWalk_to_ball_dynamic(Foot foot)
{
  Vector2d ballPos;
  double stepX;
  double stepY;
  double ballRadius = getPathModel().ballRadius; 
  double stepRotation = ballPos.abs() > 250 ? ballPos.angle() : 0;

  switch (foot) {
  case Foot::Left:  
    ballPos = getBallModel().positionPreviewInLFoot;
    stepX = ballPos.x - std::abs(ballPos.y - getPathModel().goto_yOffset) - getPathModel().goto_distance - ballRadius;
    stepY = ballPos.y - getPathModel().goto_yOffset;
    break;
  case Foot::Right: 
    ballPos = getBallModel().positionPreviewInRFoot;
    stepX = ballPos.x - std::abs(ballPos.y + getPathModel().goto_yOffset) - getPathModel().goto_distance - ballRadius;
    stepY = ballPos.y + getPathModel().goto_yOffset;
    break;
  }

  Vector3d step = limit_step(Vector3d(stepX, stepY, stepRotation));

  add(new_step(step.x, step.y, step.z));
}

void PrimitiveManeuvers::MFast_forward_kick(Foot foot)
{
  Vector2d ballPos;
  switch (foot) {
  case Foot::Left:  ballPos = getBallModel().positionPreviewInLFoot; break;
  case Foot::Right: ballPos = getBallModel().positionPreviewInRFoot; break;
  }

  add(new_step(ballPos.x + 500, ballPos.y, 0.0, 0.7, stepType::kickstep, 0.0));
}

void PrimitiveManeuvers::MKick_with_foot(Foot foot)
{
  Vector2d ballPos;
  switch (foot) {
  case Foot::Left:  ballPos = getBallModel().positionPreviewInLFoot; break;
  case Foot::Right: ballPos = getBallModel().positionPreviewInRFoot; break;
  }

  add(new_step(ballPos.x + 500, 0.0, 0.0, 0.7, stepType::kickstep, 0.0));
}

void PrimitiveManeuvers::MSidekick(Foot foot)
{
  //Vector2d ballPos;
  double speedDirection;
  double stepY;
  switch (foot) {
    case Foot::Left:  
      //ballPos = getBallModel().positionPreviewInLFoot; 
      speedDirection = 90; 
      stepY = 100; 
      break;
    case Foot::Right: 
      //ballPos = getBallModel().positionPreviewInRFoot; 
      speedDirection = -90; 
      stepY = -100; 
      break;
  }

  if (foot == Foot::Right && getMotionStatus().stepControl.moveableFoot == MotionStatus::StepControlStatus::BOTH)
  {
    foot_to_be_used = Foot::Left;
  }
  else if (foot == Foot::Left && getMotionStatus().stepControl.moveableFoot == MotionStatus::StepControlStatus::BOTH)
  {
    foot_to_be_used = Foot::Right;
  }

  if ( foot == Foot::Right && foot_to_be_used == Foot::Left
    || foot == Foot::Left && foot_to_be_used == Foot::Right)
  {
    if (step_list.empty()) {
      add(new_step(500, stepY, 0, 0.7, stepType::kickstep, speedDirection));

      step_list.push_back(new_step(0.0, 0.0, 0, 0.7, stepType::kickstep, 0.0));

      getPathModel().kick_executed = true;
    }
  }
  else {
    int x = 0;
  }
}

Vector3d PrimitiveManeuvers::limit_step(Vector3d step)
{
  // taken out of the stepplanner
  // 0.75 because 0.5 * character(usually 0.5) + 0.5 (also out of stepplanner)
  double maxStepTurn     = Math::fromDegrees(30) * 0.75;
  double maxStep         = 40.0f;
  double stepTurn        = Math::clamp(step.z, -maxStepTurn, maxStepTurn);
  double stepX           = Math::clamp(step.x, -maxStep, maxStep) * cos(stepTurn/maxStepTurn * Math::pi/2);
  double stepY           = Math::clamp(step.y, -maxStep, maxStep) * cos(stepTurn/maxStepTurn * Math::pi/2);

  return Vector3d(stepX, stepY, stepTurn);
}

// Stepcontrol
void PrimitiveManeuvers::prepare_walk()
{
  // Look at the ball
  if (getMultiBallPercept().wasSeen())
  {
    Vector2d pos = (*getMultiBallPercept().begin()).positionOnField;
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if(pos.abs() > (*iter).positionOnField.abs()) {
        pos = (*iter).positionOnField;
      }
    }
    getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
    getHeadMotionRequest().targetPointInTheWorld.x = pos.x;
    getHeadMotionRequest().targetPointInTheWorld.y = pos.y;
    getHeadMotionRequest().targetPointInTheWorld.z = getFieldInfo().ballRadius;
  }


  // Put arms to the back
  getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_back;
}

PrimitiveManeuvers::Step PrimitiveManeuvers::new_step(double x,
                                                      double y,
                                                      double rotation) {
  Step newStep = {x, y, rotation, 0.5, stepType::walkstep, 0.0};
  return newStep;
}
PrimitiveManeuvers::Step PrimitiveManeuvers::new_step(double x,
                                                      double y,
                                                      double rotation,
                                                      double character,
                                                      WalkRequest::StepControlRequest::stepType type,
                                                      double speedDirection) {
  Step newStep = {x, y, rotation, character, type, speedDirection};
  return newStep;
}


bool PrimitiveManeuvers::add(PrimitiveManeuvers::Step step) {
  if (step_list.size() == 0) {
    step_list.push_back(step);
    return true;
  }
  return false;
}


void PrimitiveManeuvers::manage_step_buffer() 
{
  if (step_list.empty()) {
    getPathModel().kick_executed = false;
    return;
  }

  // stepID higher than the last one means, stepControl request with the
  // last_stepcontrol_stepID has been accepted
  // switch the foot_to_be_used
  if (last_stepcontrol_stepID < getMotionStatus().stepControl.stepID)
  {
    step_list.erase(step_list.begin());
    last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
    foot_to_be_used = foot_to_be_used == Right ? Left : Right;
  }

  // Correct last_stepcontrol_stepID if neccessary
  if (getMotionStatus().stepControl.stepID < last_stepcontrol_stepID) {
    last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
  }
}

void PrimitiveManeuvers::execute_step() 
{
  if (step_list.empty()) {
    return;
  }

  STOPWATCH_START("PrimitiveManeuvers:execute_steplist");
  getMotionRequest().walkRequest.stepControl.stepID = getMotionStatus().stepControl.stepID;

  getMotionRequest().id = motion::walk;
  getMotionRequest().standardStand                                = false;

  // hack: in case of a walk step this time should taken from the parameters(?)
  getMotionRequest().walkRequest.stepControl.time                 = 250;
  if (step_list.front().type == stepType::kickstep) {
    getMotionRequest().walkRequest.stepControl.time = 300;
  }

  getMotionRequest().walkRequest.coordinate                       = WalkRequest::Hip;
  getMotionRequest().walkRequest.stepControl.type                 = step_list.at(0).type;
  getMotionRequest().walkRequest.stepControl.target.translation.x = step_list.at(0).x;
  getMotionRequest().walkRequest.stepControl.target.translation.y = step_list.at(0).y;
  getMotionRequest().walkRequest.stepControl.target.rotation      = step_list.at(0).rotation;
  getMotionRequest().walkRequest.character                        = step_list.at(0).character;

  // kicks only
  getMotionRequest().walkRequest.stepControl.speedDirection       = step_list.at(0).speedDirection;

  // sync foot_to_be_used with the movableFoot
  switch (getMotionStatus().stepControl.moveableFoot)
  {
    case MotionStatus::StepControlStatus::LEFT:
      foot_to_be_used = Foot::Left;
      break;
    case MotionStatus::StepControlStatus::RIGHT:
      foot_to_be_used = Foot::Right;
      break;
      // TODO: choose foot more intelligently when both feet are usable
    case MotionStatus::StepControlStatus::BOTH:
      if (step_list.front().type == stepType::kickstep)
      {
        
      }
      else if (step_list.front().y > 0.0f || step_list.at(0).rotation > 0.0f)
			{
				foot_to_be_used = Foot::Left;
			}
			else
			{
				foot_to_be_used = Foot::Right;
			}
			break;
    case MotionStatus::StepControlStatus::NONE:
      foot_to_be_used = Foot::Right;
      break;
  }

  // false means right foot
  getMotionRequest().walkRequest.stepControl.moveLeftFoot = foot_to_be_used == Right ? false : true;
  STOPWATCH_STOP("PrimitiveManeuvers:execute_steplist");
}
