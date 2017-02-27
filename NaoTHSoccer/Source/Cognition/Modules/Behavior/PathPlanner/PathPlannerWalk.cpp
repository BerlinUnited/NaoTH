/**
 * @file PathPlannerWalk.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class PathPlannerWalk
 */

#include "PathPlannerWalk.h"
#include <iostream>

PathPlannerWalk::PathPlannerWalk()
:
last_stepID(0),
step_list({}),
last_stepcontrol_stepID(0),
foot_to_be_used(PathPlannerWalk::Foot::Right)
{
  DEBUG_REQUEST_REGISTER("PathPlanner:walk:walk_to_ball",
                         "Walk to the ball with alternating steps.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:walk:execute_steplist",
                         "Experimental", true);
  DEBUG_REQUEST_REGISTER("PathPlanner:walk:add_forward_step",
                         "Experimental", false);
}

void PathPlannerWalk::execute()
{
  DEBUG_REQUEST("PathPlanner:walk:walk_to_ball",
                // TEST: std::cout << getBallModel().positionPreview.abs() << std::endl;
                if (step_list.size() < 4 && getBallModel().positionPreview.abs() > 200) {
                  add(new_step(towards_ball('x'),
                               towards_ball('y'),
                               towards_ball('r')
                               ));
                }
                );
  DEBUG_REQUEST("PathPlanner:walk:add_forward_step",
                add(new_step(40.0, 0.0, 0.0));
                );

  DEBUG_REQUEST("PathPlanner:walk:execute_steplist",
                execute_step_list();
                );
}

void PathPlannerWalk::stand() {
  getMotionRequest().id = motion::stand;
  getMotionRequest().standardStand = true;
  getMotionRequest().walkRequest.stepControl.target.translation.x = 0.0;
  getMotionRequest().walkRequest.stepControl.target.translation.y = 0.0;
  getMotionRequest().walkRequest.stepControl.target.rotation = 0.0;
}

void PathPlannerWalk::add(PathPlannerWalk::Step step) {
  step_list.push_back(step);
}
PathPlannerWalk::Step PathPlannerWalk::new_step(float x, float y, float rotation) {
  Step newStep = {last_stepID, x, y, rotation};
  last_stepID++;
  return newStep;
}
PathPlannerWalk::Step PathPlannerWalk::new_step(std::size_t ID, float x, float y, float rotation) {
  assert(ID > last_stepID);
  Step newStep = {ID, x, y, rotation};
  last_stepID = ID;
  return newStep;
}
void PathPlannerWalk::pop_step() {
  assert(!step_list.empty());
  step_list.erase(step_list.begin());
}

float PathPlannerWalk::towards_ball(const char x_or_y) {
  double ballRad    = getBallPercept().radiusInImage;
  Vector2d preview  = getBallModel().positionPreview;
  switch (x_or_y) {
    case 'x': return 0.7 * (preview.x - std::abs(preview.y) - ballRad);
    case 'y': return 0.7 * (preview.y);
    case 'r': return preview.abs() > 250 ? preview.angle() : 0.0;
  }
  return 0.0;
}

void PathPlannerWalk::execute_step_list() {
  if (step_list.size() > 0) {
    getMotionRequest().walkRequest.stepControl.stepID = getMotionStatus().stepControl.stepID;
    //getMotionRequest().walkRequest.stepControl.stepID = step_list.at(0).ID;

    getMotionRequest().id                                           = motion::walk;
    getMotionRequest().walkRequest.character                        = 0.5; // stable
    getMotionRequest().standardStand                                = false;
    getMotionRequest().walkRequest.stepControl.time                 = 300;
    getMotionRequest().walkRequest.coordinate                       = WalkRequest::Hip;
    getMotionRequest().walkRequest.stepControl.target.translation.x = step_list.at(0).x;
    getMotionRequest().walkRequest.stepControl.target.translation.y = step_list.at(0).y;
    getMotionRequest().walkRequest.stepControl.target.rotation      = step_list.at(0).rotation;

    // stepID higher than the last one means, stepControl request with the
    // last_stepcontrol_stepID has been accepted
    // switch foot_to_be_used
    if (last_stepcontrol_stepID < getMotionStatus().stepControl.stepID)
    {
      pop_step();
      last_stepcontrol_stepID = getMotionStatus().stepControl.stepID;
      foot_to_be_used         = foot_to_be_used == Right ? Left : Right;
    }

    if (foot_to_be_used == Right)
    {
      getMotionRequest().walkRequest.stepControl.moveLeftFoot = false; // false = right foot
    }
    else
    {
      getMotionRequest().walkRequest.stepControl.moveLeftFoot = true; // false = right foot
    }
  } else {
    stand();
  }

  if (getMotionStatus().stepControl.stepID < last_stepcontrol_stepID) {
    last_stepcontrol_stepID = 0;
  }
}
