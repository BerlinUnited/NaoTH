/**
 * @file PathPlanner.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
 * Implementation of class PathPlanner
 */

#include "PathPlanner.h"

PathPlanner::PathPlanner() 
:
  isWalking(false)
{
  // walk
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:walk_forward",
                         "Walk forward", false);

}

void PathPlanner::execute() 
{
  // reset some stuff by default
  getMotionRequest().forced = false;
  getMotionRequest().standHeight = -1; // sit in a stable position

  getMotionRequest().walkRequest.target = Pose2D();
  getMotionRequest().forced = false;

  // walk forward
  DEBUG_REQUEST("PathPlanner:motion:walk_forward",

                getMotionRequest().id = motion::walk;
                getMotionRequest().walkRequest.target.translation.x = 500;

                );

}//end execute
