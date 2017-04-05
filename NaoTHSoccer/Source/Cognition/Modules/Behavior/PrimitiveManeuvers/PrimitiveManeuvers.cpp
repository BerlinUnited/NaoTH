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
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:walk_to_point", "Walks to the given point.", false);
  DEBUG_REQUEST_REGISTER("PrimitiveManeuvers:execute_step_list", "Executes the step list", true);
}

void PrimitiveManeuvers::execute()
{
  // Execute step list
  DEBUG_REQUEST("PrimitiveManeuvers:execute_step_list",
                PrimitiveManeuvers::execute_step_list();
                );

  DEBUG_REQUEST("PrimitiveManeuvers:walk_to_point",
                prepare_walk();
                double x = 0.0f;
                double y = 0.0f;
                //if(!PrimitiveManeuvers::MWalk_to_point(x, y)) {}

                static const Vector2d relativeCoord = getMotionStatus().plannedMotion.hip/Vector2d(x, y);
                static const double angle = Geometry::angleTo(getRobotPose(), Vector2d(x, y));

                std::cout << angle << " -- " << relativeCoord.x << ", "
                << relativeCoord.y << std::endl;
                );
}

// Primitive Maneuvers
bool PrimitiveManeuvers::MWalk_to_point(double x, double y)
{
  //Vector2d relativeCoord = getRobotPose()*Vector2d(x, y);

  // Rotate completely first
  if (PrimitiveManeuvers::MRotate_towards_point(x, y))
  {
    /*if (   relativeCoord.x != 0.0f
        || relativeCoord.y != 0.0f)
    {
      if (relativeCoord.x > 40.0f)
      {
        add(new_step(40.0f, 0.0f, 0.0f));
      }
      else
      {
        add(new_step(relativeCoord.x, 0.0f, 0.0f));
      }
    }
    else
    {
      return true;
    }*/
  }

  return false;
}

bool PrimitiveManeuvers::MRotate_towards_point(double x, double y)
{
  static const Vector2d relativeCoord = getRobotPose()*Vector2d(x, y);
  static const double angle = Geometry::angleTo(relativeCoord,
                                                Vector2d(x, y));

  static double angleDelta  = 0.0f;

  std::cout << angle << std::endl;

  if (std::abs(angleDelta) < std::abs(angle))
  {
    if (add(new_step(0.0f, 0.0f, angle/10)))
    {
      std::cout << angle << " --- " << angleDelta << std::endl;
      angleDelta += angle/10.0f;
    }
  }
  else
  {
    return true;
  }

  return false;
}




// Stepcontrol
void PrimitiveManeuvers::prepare_walk()
{
  // Look at the ball
  /*if (getBallPercept().ballWasSeen || getBallPerceptTop().ballWasSeen)
  {
    Vector2d pos = (*getMultiBallPercept().begin()).positionOnField;
    for(MultiBallPercept::ConstABPIterator iter = getMultiBallPercept().begin(); iter != getMultiBallPercept().end(); iter++) {
      if(pos.abs() > (*iter).positionOnField.abs())
      {
        pos = (*iter).positionOnField;
      }
    }
    getHeadMotionRequest().id = HeadMotionRequest::look_at_world_point;
    getHeadMotionRequest().targetPointInTheWorld.x = pos.x;
    getHeadMotionRequest().targetPointInTheWorld.y = pos.y;
    getHeadMotionRequest().targetPointInTheWorld.z = getFieldInfo().ballRadius;
  }*/


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
