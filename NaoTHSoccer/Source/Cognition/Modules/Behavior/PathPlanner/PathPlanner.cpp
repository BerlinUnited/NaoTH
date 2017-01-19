/**
 * @file PathPlanner.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
 * Implementation of class PathPlanner
 */

#include "PathPlanner.h"
//#include <iostream>

PathPlanner::PathPlanner() 
:
  isWalking(false)
{
  // Stand
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:stand",
                         "Stand still", true);

  // Walk
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:walk_forward",
                         "Walk forward", false);
  // Go to ball (XABSL: go_to_ball)
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:go_to_ball",
                         "Go to the ball", false);
  // Search for the ball
  DEBUG_REQUEST_REGISTER("PathPlanner:head:Search",
                         "Search for the ball", false);
  // Look at the ball
  DEBUG_REQUEST_REGISTER("PathPlanner:head:LookAtMultiBallPercept",
                "Look at the ball", false);
}

void PathPlanner::execute() 
{
  // reset some stuff by default
  getMotionRequest().forced = false;
  getMotionRequest().standHeight = -1; // sit in a stable position

  getMotionRequest().walkRequest.target = Pose2D();
  getMotionRequest().forced = false;

  // Stand still
  DEBUG_REQUEST("PathPlanner:motion:stand",
                getMotionRequest().id = motion::stand;
                );

  // Walk forward
  DEBUG_REQUEST("PathPlanner:motion:walk_forward",
                getMotionRequest().id = motion::walk;
                getMotionRequest().walkRequest.target.translation.x = 500;
                );

  // Walk to the ball (XABSL: go_to_ball)
  DEBUG_REQUEST(
    "PathPlanner:motion:go_to_ball",
    if (getMultiBallPercept().wasSeen())
    {
      Vector2d preview = getBallModel().positionPreview;
      Vector2d pos     = getBallModel().position;
      double ballRad   = getMultiBallPercept().getPercepts().at(0).radiusInImage;
      getMotionRequest().id                               = motion::walk;
      getMotionRequest().walkRequest.coordinate           = WalkRequest::Hip;
      getMotionRequest().walkRequest.character            = 0.5;
      getMotionRequest().walkRequest.target.translation.x = std::max(0.7 * (pos.x - ballRad), 0.0);
      getMotionRequest().walkRequest.target.translation.y = preview.y;
      getMotionRequest().walkRequest.target.rotation      = preview.abs() > 250
                                                          ? preview.angle()
                                                          : 0;
      /*std::cout << "preview.x: "   << preview.x                           << std::endl
                << "pos.x: "       << pos.x                               << std::endl
                << "ballRad: "     << ballRad                             << std::endl
                << "translate x: " << 0.7 * (preview.x - pos.x - ballRad) << std::endl;*/
    }
  );

  // Search for the ball
  DEBUG_REQUEST("PathPlanner:head:Search",
                getHeadMotionRequest().id = HeadMotionRequest::search;
                getHeadMotionRequest().searchCenter = Vector3<double>(2000, 0, 0);
                getHeadMotionRequest().searchSize = Vector3<double>(1500, 2000, 0);
                );
  // Look at the ball
  DEBUG_REQUEST("PathPlanner:head:LookAtMultiBallPercept",
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
                );
}//end execute
