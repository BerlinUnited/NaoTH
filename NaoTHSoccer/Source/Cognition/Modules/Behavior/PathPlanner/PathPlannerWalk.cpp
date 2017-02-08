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
lastStepID(0),
executionFoot(PathPlannerWalk::Foot::Right)
{
  DEBUG_REQUEST_REGISTER("PathPlanner:walk:walk_forward",
                         "Walk forward with alternating steps.", false);
}

void PathPlannerWalk::execute()
{
  DEBUG_REQUEST("PathPlanner:walk:walk_forward",
                getMotionRequest().walkRequest.stepControl.stepID = getMotionStatus().stepControl.stepID;

                // stepID higher than the last one means, lastStepID has been accepted
                // switch executionFoot
                if (lastStepID < getMotionStatus().stepControl.stepID)
                {
                  lastStepID    = getMotionStatus().stepControl.stepID;
                  executionFoot = executionFoot == Right ? Left : Right;
                }

                getMotionRequest().id = motion::walk;
                getMotionRequest().walkRequest.character = 0.5; // stable
                getMotionRequest().standardStand = false;
                getMotionRequest().walkRequest.stepControl.target.translation.x = 40.0;
                getMotionRequest().walkRequest.stepControl.target.translation.y = 0.0;
                getMotionRequest().walkRequest.stepControl.target.rotation      = 0.0;
                getMotionRequest().walkRequest.stepControl.time     = 300;
                getMotionRequest().walkRequest.coordinate           = WalkRequest::Hip;

                if (executionFoot == Right)
                {
                  getMotionRequest().walkRequest.stepControl.moveLeftFoot = false; // false = right foot
                }
                else
                {
                  getMotionRequest().walkRequest.stepControl.moveLeftFoot = true; // false = right foot
                }
                );
}
