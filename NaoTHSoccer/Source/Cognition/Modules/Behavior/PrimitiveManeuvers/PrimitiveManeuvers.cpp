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
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:walk_to_ball", "Walks to the ball.", true);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:execute_step_list", "Executes the step list", true);
}

void PrimitiveManeuvers::execute()
{
  // Execute step list
  DEBUG_REQUEST("PrimitiveManeuvers:execute_step_list",
                PrimitiveManeuvers::execute_step_list();
                );

  DEBUG_REQUEST("PrimitiveManeuvers:walk_to_ball",
                prepare_walk();

                if(PrimitiveManeuvers::MWalk_to_ball(Foot::Right))
                {

                }
                );
}

// Primitive Maneuvers
bool PrimitiveManeuvers::MWalk_to_ball(Foot foot)
{
  Vector2d pos;
  switch (foot) {
    case Foot::Left:  pos = getBallModel().positionPreviewInLFoot; break;
    case Foot::Right: pos = getBallModel().positionPreviewInRFoot; break;
    case Foot::NONE:  pos = getBallModel().positionPreview; break;
  }
  double rotation = pos.angle();

  Vector2d step = limit_step(Vector2d(pos.x, rotation));

  if (pos.abs() > 220)
  {
    add(new_step(step.x, 0.0f, step.y));
  }
  else if (foot_to_be_used != foot)
  {
    add(new_step(step.x, 0.0f, step.y));
  }
  else
  {
    return true;
  }
  return false;
}

Vector2d PrimitiveManeuvers::limit_step(Vector2d step)
{
  // taken out of the stepplanner
  // 0.75 because 0.5 * character(usually 0.5) + 0.5 (also out of stepplanner)
  double maxStepTurn     = Math::fromDegrees(30) * 0.75;
  double maxStep         = 40.0f;
  double stepTurn        = Math::clamp(step.y, -maxStepTurn, maxStepTurn);
  double stepX           = Math::clamp(step.x, -maxStep, maxStep) * cos(stepTurn/maxStepTurn * Math::pi/2);

  return Vector2d(stepX, stepTurn);
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
  assert(!step_list.empty());
  step_list.erase(step_list.begin());
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
  } else {
    getMotionRequest().id = motion::stand;
  }

  if (getMotionStatus().stepControl.stepID < last_stepcontrol_stepID) {
    last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
  }
}
