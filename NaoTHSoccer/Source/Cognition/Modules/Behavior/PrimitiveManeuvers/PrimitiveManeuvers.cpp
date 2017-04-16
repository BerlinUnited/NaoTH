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
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:execute_step_list", "Executes the step list", true);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:walk_to_ball", "Walks to the ball.", true);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:move_around_ball", "Moves around the ball.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:walk_back_approach", "Walks back till ball seen, then approaches", false);

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
  // Execute step list
  DEBUG_REQUEST("PrimitiveManeuvers:execute_step_list",
	PrimitiveManeuvers::execute_step_list();
  );
  // Maneuvers
  DEBUG_REQUEST("PrimitiveManeuvers:walk_to_ball",
	prepare_walk();
	PrimitiveManeuvers::MWalk_to_ball(Foot::Right);
  );
  DEBUG_REQUEST("PrimitiveManeuvers:move_around_ball",
	prepare_walk();
	PrimitiveManeuvers::MMove_around_ball(Foot::Right, true);
  );
  DEBUG_REQUEST("PrimitiveManeuvers:walk_back_approach",
	prepare_walk();
	PrimitiveManeuvers::MWalk_back_approach(Foot::Right);
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
}

// Primitive Maneuvers
bool PrimitiveManeuvers::MWalk_to_ball(Foot foot)
{
  Vector2d ballPos;
  switch (foot) {
    case Foot::Left:  ballPos = getBallModel().positionPreviewInLFoot; break;
    case Foot::Right: ballPos = getBallModel().positionPreviewInRFoot; break;
    case Foot::NONE:  ballPos = getBallModel().positionPreview; break;
  }
  double ballRotation = ballPos.angle();
  double ballDistance = ballPos.abs();

  Vector3d step = limit_step(Vector3d(ballPos.x, 0.0f, ballRotation));

  if (ballDistance > 220.0f)
  {
    add(new_step(step));
  }
  else if ((getBallModel().positionPreviewInRFoot.x < getBallModel().positionPreviewInLFoot.x && foot == Foot::Right) 
        || (getBallModel().positionPreviewInLFoot.x < getBallModel().positionPreviewInRFoot.x && foot == Foot::Left))
  {
    // Add a correcting step if neccessary
    add(new_step(step));
  }
  else
  {
    return true;
  }
  return false;
}

// TODO: needs a case where it returns true, or maybe control that from another place (if (...) {Move_around_ball()})
bool PrimitiveManeuvers::MMove_around_ball(Foot foot, bool go_right)
{
  Vector2d ballPos;
  switch (foot) {
  case Foot::Left:  ballPos = getBallModel().positionPreviewInLFoot; break;
  case Foot::Right: ballPos = getBallModel().positionPreviewInRFoot; break;
  case Foot::NONE:  ballPos = getBallModel().positionPreview; break;
  }
  double ballRotation = ballPos.angle();
  double ballDistance = ballPos.abs();
  double radius       = ballDistance > 200 ? ballDistance : 200.0f;		// 200 is close to the ball
  double direction    = go_right == true ? -100.0f : 100.0f;			// Move around the ball right or left way
  
  Vector3d step = limit_step(Vector3d((ballDistance - radius) * std::cos(ballRotation),
                                      direction,
                                      ballRotation));
  add(new_step(step));

  return false;
}

bool PrimitiveManeuvers::MWalk_back_approach(Foot foot)
{
	Vector3d step;
	Vector2d ballPos;
	switch (foot) {
	case Foot::Left:  ballPos = getBallModel().positionPreviewInLFoot; break;
	case Foot::Right: ballPos = getBallModel().positionPreviewInRFoot; break;
	case Foot::NONE:  ballPos = getBallModel().positionPreview; break;
	}

	if (!getBallPercept().ballWasSeen || ballPos.x < 250)
	{
		step = limit_step(Vector3d(-40.0f, 0.0f, 0.0f));
		add(new_step(step));
	}
	else if (ballPos.x >= 250 && std::abs(ballPos.y) > 10.0f)
	{
		step = limit_step(Vector3d(0.0f, ballPos.y, 0.0f));
		add(new_step(step));
	}
	else
	{
		return true;
	}
  return false;
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

PrimitiveManeuvers::Step PrimitiveManeuvers::new_step(Vector3d step) {
  Step newStep = {step.x, step.y, step.z, 0.5};
  return newStep;
}
PrimitiveManeuvers::Step PrimitiveManeuvers::new_step(double x,
                                                      double y,
                                                      double rotation) {
  Step newStep = {x, y, rotation, 0.5};
  return newStep;
}
PrimitiveManeuvers::Step PrimitiveManeuvers::new_step(double x,
                                                      double y,
                                                      double rotation,
                                                      double character) {
  Step newStep = {x, y, rotation, character};
  return newStep;
}
bool PrimitiveManeuvers::add(PrimitiveManeuvers::Step step) {
  if (step_list.size() == 0) {
    step_list.push_back(step);
    return true;
  }
  return false;
}
void PrimitiveManeuvers::pop_step() {
  if (!step_list.empty())
  {
    step_list.erase(step_list.begin());
  }
}

void PrimitiveManeuvers::execute_step_list() {
  if (step_list.size() > 0) {
    if (getMotionRequest().id == motion::stand) {
      last_stepcontrol_stepID = 0;
    }
    getMotionRequest().walkRequest.stepControl.stepID = getMotionStatus().stepControl.stepID;

    getMotionRequest().id = motion::walk;
    getMotionRequest().standardStand                                = false;
    getMotionRequest().walkRequest.stepControl.time                 = 300;
    getMotionRequest().walkRequest.coordinate                       = WalkRequest::Hip;
    getMotionRequest().walkRequest.stepControl.target.translation.x = step_list.at(0).x;
    getMotionRequest().walkRequest.stepControl.target.translation.y = step_list.at(0).y;
    getMotionRequest().walkRequest.stepControl.target.rotation      = step_list.at(0).rotation;
    getMotionRequest().walkRequest.character                        = step_list.at(0).character;

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
        foot_to_be_used = Foot::Right;
        break;
      case MotionStatus::StepControlStatus::NONE:
        foot_to_be_used = Foot::Right;
        break;
    }

    // stepID higher than the last one means, stepControl request with the
    // last_stepcontrol_stepID has been accepted
    // switch the foot_to_be_used
    if (last_stepcontrol_stepID < getMotionStatus().stepControl.stepID)
    {
      pop_step();
      last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
      foot_to_be_used         = foot_to_be_used == Right ? Left : Right;
    }

    // false means right foot
    getMotionRequest().walkRequest.stepControl.moveLeftFoot = foot_to_be_used == Right ? false : true;
  } 
  else 
  {
    getMotionRequest().id = motion::stand;
  }
  
  // Correct last_stepcontrol_stepID if neccessary
  if (getMotionStatus().stepControl.stepID < last_stepcontrol_stepID) {
    last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
  }
}
