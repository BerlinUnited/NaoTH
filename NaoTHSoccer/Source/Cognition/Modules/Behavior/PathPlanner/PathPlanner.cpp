/**
 * @file PathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class PathPlanner
 */

#include "PathPlanner.h"
#include <iostream>

PathPlanner::PathPlanner()
:
step_list({}),
foot_to_be_used(PathPlanner::Foot::Right),
last_stepcontrol_stepID(0)
{
  DEBUG_REQUEST_REGISTER("PathPlanner:walk:walk_to_ball_and_kick_forward",
                         "Walk to the ball with alternating steps.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:walk:execute_steplist",
                         "Experimental", true);
  DEBUG_REQUEST_REGISTER("PathPlanner:walk:add_forward_step",
                         "Experimental", false);
}

void PathPlanner::execute()
{
  DEBUG_REQUEST("PathPlanner:walk:walk_to_ball_and_kick_forward",
                look_at_ball();
                getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_back;

                static char foot;
                std::size_t approximate_steps_to_ball;
                static bool executed_once = false;
                double step_size          = 40.0;
                Vector2d preview          = getBallModel().positionPreview;
                double distance           = preview.abs();
                double ballRad            = 70.0;
                bool ballWasSeen          = getBallModel().valid;

                if (distance > 700) {
                  std::cout << towards_ball('r') << std::endl;
                  add(new_step(step_size,
                               0.0,
                               towards_ball('r')
                               ));
                } else if (distance > 280) {
                  if (!executed_once) {
                    executed_once = true;

                    approximate_steps_to_ball = static_cast<std::size_t>((distance - ballRad - step_size) / 40.0);
                    // check how many steps are possible with the step_size and
                    // position right or left foot in front of the ball
                    // according to the check
                    foot = find_foot_with(approximate_steps_to_ball);
                  }
                  add(new_step(step_size,
                               towards_ball('y', foot),
                               0.0));
                } else if (distance > 220) {
                  executed_once = false;

                  // close the remaining distance to the ball
                  add(new_step(step_size , 0.0, 0.0));
                } else if (step_list.size() == 0) { // <-- when the next step is awaited
                  // do a single step with the foot that is not in front of the ball
                  // if approximate_steps_to_ball was off by one step
                  if (!executed_once) {
                    executed_once = true;

                    if (   (foot == 'r' && foot_to_be_used == Right)
                        || (foot == 'l' && foot_to_be_used == Left)) {
                      std::cout << "correcting!" << std::endl;
                      // 30.0 instead of step_size (40.0) because
                      // sometimes that would cause hitting the ball by
                      // accident
                      add(new_step(30.0, 0.0, 0.0));
                    }
                  }
                  if (ballWasSeen)
                  {
                    // kick the ball forward
                    //add(new_step(500.0, 0.0, 0.0, 1.0));
                  }
                }
                );
  DEBUG_REQUEST("PathPlanner:walk:add_forward_step",
                add(new_step(40.0, 0.0, 0.0));
                );
  DEBUG_REQUEST("PathPlanner:walk:execute_steplist",
                execute_step_list();
                );
}

void PathPlanner::look_at_ball()
{
  if (getBallPercept().ballWasSeen || getBallPerceptTop().ballWasSeen)
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
  }
}


PathPlanner::Step PathPlanner::new_step(double x,
                                                double y,
                                                double rotation) {
  Step newStep = {x, y, rotation, 0.5};
  return newStep;
}
PathPlanner::Step PathPlanner::new_step(double x,
                                                double y,
                                                double rotation,
                                                double character) {
  Step newStep = {x, y, rotation, character};
  return newStep;
}
void PathPlanner::add(PathPlanner::Step step) {
  if (step_list.size() == 0) {
    step_list.push_back(step);
  }
}
void PathPlanner::pop_step() {
  assert(!step_list.empty());
  step_list.erase(step_list.begin());
}

double PathPlanner::towards_ball(const char x_or_y) {
  double ballRad    = getBallPercept().radiusInImage;
  Vector2d preview  = getBallModel().positionPreview;
  switch (x_or_y) {
    case 'x': return 0.7 * (preview.x - std::abs(preview.y) - ballRad);
    case 'y': return 0.7 * (preview.y);
    case 'r': return preview.abs() > 250 ? preview.angle() : 0.0;
  }
  return 0.0;
}
double PathPlanner::towards_ball(const char x_y_or_rot, const char for_left_or_right_foot) {
  double ballRad = getBallPercept().radiusInImage;
  Vector2d preview;
  switch (for_left_or_right_foot) {
    case 'r':
      preview = getBallModel().positionPreviewInRFoot;
      break;
    case 'l':
      preview = getBallModel().positionPreviewInLFoot;
      break;
  }
  switch (x_y_or_rot) {
    case 'x': return 0.7 * (preview.x - std::abs(preview.y) - ballRad);
    case 'y': return 0.7 * (preview.y);
    case 'r': return preview.abs() > 250 ? preview.angle() : 0.0;
  }
  return 0.0;
}
char PathPlanner::find_foot_with(const std::size_t approximate_steps_to_ball) {
  if (approximate_steps_to_ball % 2 == 0) {
    if (foot_to_be_used == Right) {
      return 'r';
    } else {
      return 'l';
    }
  } else {
    if (foot_to_be_used == Right) {
      return 'l';
    } else {
      return 'r';
    }
  }
}

void PathPlanner::execute_step_list() {
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
