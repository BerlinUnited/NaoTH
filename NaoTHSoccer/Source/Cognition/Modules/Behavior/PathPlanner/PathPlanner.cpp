/**
 * @file PathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class PathPlanner
 */

#include "PathPlanner.h"

PathPlanner::PathPlanner()
{
  // Stand
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:stand",
                         "Stand still", false);

  // Walk
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:walk_forward",
                         "Walk forward", false);
  // Go to ball with right or left foot (when close to ball, approach for attack)
  //XABSL: go_to_ball_foot_dynamic
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:go_to_ball_foot_dynamic",
                         "Go to the ball with the right or left foot, close to the ball, before attacking.", true);
  // Search for the ball
  DEBUG_REQUEST_REGISTER("PathPlanner:head:search",
                         "Search for the ball", false);
  // Turn right or left
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:turn_left", "Turn left.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:turn_right", "Turn right.", false);
  // Look straight ahead
  DEBUG_REQUEST_REGISTER("PathPlanner:head:look_straight_ahead", "Look straight ahead.", false);
}

void PathPlanner::execute() 
{
  // Stand still
  DEBUG_REQUEST("PathPlanner:motion:stand",
                getMotionRequest().id = motion::stand;
                );

  // Walk forward
  DEBUG_REQUEST("PathPlanner:motion:walk_forward",
                // reset some stuff by default
                getMotionRequest().forced = false;
                getMotionRequest().standHeight = -1; // sit in a stable position
                getMotionRequest().walkRequest.target = Pose2D();
                getMotionRequest().forced = false;

                // walk forward
                getMotionRequest().id = motion::walk;
                getMotionRequest().walkRequest.target.translation.x = 500;
                );

  // Go to ball with right or left foot (when close to ball, approach for attack)
  // XABSL: go_to_ball_foot_dynamic
  DEBUG_REQUEST("PathPlanner:motion:go_to_ball_foot_dynamic",
                if (    getPathModel().goto_ball_right
                    && !getPathModel().goto_ball_left)
                {
                  PathPlanner::goToBallRight(getPathModel().goto_distance,
                                             getPathModel().goto_yOffset);
                }
                else if (  !getPathModel().goto_ball_right
                         && getPathModel().goto_ball_left)
                {
                  PathPlanner::goToBallLeft(getPathModel().goto_distance,
                                            getPathModel().goto_yOffset);
                }
                );


  DEBUG_REQUEST("PathPlanner:motion:turn_left",
                getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;
                getMotionRequest().id = motion::walk;
                getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(60);
                );
  DEBUG_REQUEST("PathPlanner:motion:turn_right",
                getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;
                getMotionRequest().id = motion::walk;
                getMotionRequest().walkRequest.target.rotation = Math::fromDegrees(-60);
                );
  // --- HEAD ---

  // Search for the ball
  DEBUG_REQUEST("PathPlanner:head:search",
    // Ball hasn't been seen before
    if (   !getBallPercept().ballWasSeen
        && !getBallPerceptTop().ballWasSeen
        && !getMultiBallPercept().wasSeen())
    {
      getHeadMotionRequest().id           = HeadMotionRequest::search;
      getHeadMotionRequest().searchCenter = Vector3<double>(2000, 0, 0);
      getHeadMotionRequest().searchSize   = Vector3<double>(1500, 2000, 0);
    }
    // Ball was seen before, but isn't anymore
    else if (   !getBallPercept().ballWasSeen
             && !getBallPerceptTop().ballWasSeen
             &&  getMultiBallPercept().wasSeen())
    {
      getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;
      if (getBallModel().positionPreview.angle() > 0)
      { // Rotate left
        getMotionRequest().id                               = motion::walk;
        getMotionRequest().walkRequest.coordinate           = WalkRequest::Hip;
        getMotionRequest().walkRequest.target.translation.x = 0;
        getMotionRequest().walkRequest.target.translation.y = 0;
        getMotionRequest().walkRequest.target.rotation      = 200;
      }
      else
      { // Rotate right
        getMotionRequest().id                               = motion::walk;
        getMotionRequest().walkRequest.coordinate           = WalkRequest::Hip;
        getMotionRequest().walkRequest.target.translation.x =  0.0;
        getMotionRequest().walkRequest.target.translation.y =  0.0;
        getMotionRequest().walkRequest.target.rotation      = -200;
      }
    }
    // Ball is being seen - look at it !
    else if (getBallPercept().ballWasSeen || getBallPerceptTop().ballWasSeen)
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
  );
  DEBUG_REQUEST("PathPlanner:head:look_straight_ahead",
                getHeadMotionRequest().id = HeadMotionRequest::look_straight_ahead;
                );
}//end execute

void PathPlanner::goToBallRight(int distance, int yOffset)
{
  Vector2d preview = getBallModel().positionPreviewInRFoot;
  double ballRad   = getBallPercept().radiusInImage;

  getMotionRequest().id                     = motion::walk;
  getMotionRequest().walkRequest.coordinate = WalkRequest::RFoot;
  getMotionRequest().walkRequest.character  = 0.5; // stable

  getMotionRequest().walkRequest.target.translation.x =
    0.7 * (preview.x - std::abs(preview.y + yOffset) - distance - ballRad);
  getMotionRequest().walkRequest.target.translation.y =
    0.7 * (preview.y + yOffset);
  getMotionRequest().walkRequest.target.rotation =
    preview.abs() > 250 ? preview.angle() : 0.0;
}
void PathPlanner::goToBallLeft(int distance, int yOffset)
{
  Vector2d preview = getBallModel().positionPreviewInLFoot;
  double ballRad   = getBallPercept().radiusInImage;

  getMotionRequest().id                     = motion::walk;
  getMotionRequest().walkRequest.coordinate = WalkRequest::LFoot;
  getMotionRequest().walkRequest.character  = 0.5; // stable

  getMotionRequest().walkRequest.target.translation.x =
  0.7 * (preview.x - std::abs(preview.y - yOffset) - distance - ballRad);
  getMotionRequest().walkRequest.target.translation.y =
  0.7 * (preview.y - yOffset);
  getMotionRequest().walkRequest.target.rotation =
  preview.abs() > 250 ? preview.angle() : 0.0;
}
