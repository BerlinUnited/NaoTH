/**
 * @file PathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class PathPlanner
 */

#include "PathPlanner.h"

PathPlanner::PathPlanner()
{
  // Go to ball with right or left foot (when close to ball, approach for attack)
  //XABSL: go_to_ball_foot_dynamic
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:go_to_ball_foot_dynamic",
                         "Go to the ball with the right or left foot, close to the ball, before attacking.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:test:go_to_ball_foot_dynamic_test_right",
                         "TEST: Go to the ball with the right foot, close to the ball, before attacking.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:test:go_to_ball_foot_dynamic_test_left",
                         "TEST: Go to the ball with the left foot, close to the ball, before attacking.", false);
  // Move around the ball
  // XABSL: move_around_ball
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:move_around_ball",
                         "Move around the ball.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:test:move_around_ball_test",
                         "TEST: Move around the ball.", false);
  // Fast forward kick
  // XABSL: fast_forward_kick
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:fast_forward_kick",
                         "Fast forward and kick with right or left foot.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:test:fast_forward_kick_test_right",
                         "TEST: Fast forward and kick with right foot.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:test:fast_forward_kick_test_left",
                         "TEST: Fast forward and kick with left foot.", false);

  // ------ Helpers ------
  // Stand
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:stand",
                         "Stand still", false);

  // Walk
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:walk_forward",
                         "Walk forward", false);
  // Search for the ball
  DEBUG_REQUEST_REGISTER("PathPlanner:head:search",
                         "Search for the ball", false);
  // Turn right or left
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:turn_left", "Turn left.", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:motion:turn_right", "Turn right.", false);
  // Look straight ahead
  DEBUG_REQUEST_REGISTER("PathPlanner:head:look_straight_ahead", "Look straight ahead.", false);
  // ------ Helpers ------
}

void PathPlanner::execute() 
{

  // Go to ball with right or left foot (when close to ball, approach for attack)
  // XABSL: go_to_ball_foot_dynamic
  DEBUG_REQUEST("PathPlanner:motion:go_to_ball_foot_dynamic",
                if (getPathModel().pathType == PathModel::PathType::go_to_ball_right)
                {
                  PathPlanner::goToBallRight(getPathModel().goto_distance,
                                             getPathModel().goto_yOffset);
                }
                else if (getPathModel().pathType == PathModel::PathType::go_to_ball_left)
                {
                  PathPlanner::goToBallLeft(getPathModel().goto_distance,
                                            getPathModel().goto_yOffset);
                }

                // reset parameters deciding execution
                getPathModel().pathType = PathModel::PathType::none;
                );
  // TEST
  DEBUG_REQUEST("PathPlanner:test:go_to_ball_foot_dynamic_test_right",
                double distance = 160.0;
                MODIFY("PathPlanner:test:goto_distance", distance);
                double yOffset  = 0.0;
                MODIFY("PathPlanner:test:yOffset", yOffset);
                PathPlanner::goToBallRight(distance, yOffset);
                );
  DEBUG_REQUEST("PathPlanner:test:go_to_ball_foot_dynamic_test_left",
                double distance = 160.0;
                MODIFY("PathPlanner:test:goto_distance", distance);
                double yOffset  = 0.0;
                MODIFY("PathPlanner:test:goto_yOffset", yOffset);
                PathPlanner::goToBallLeft(distance, yOffset);
                );
  // Move around the ball
  // XABSL: move_around_ball
  DEBUG_REQUEST("PathPlanner:motion:move_around_ball",
                if (getPathModel().pathType == PathModel::PathType::move_around_ball)
                {
                  PathPlanner::moveAroundBall(getPathModel().move_around_ball_direction,
                                              getPathModel().move_around_ball_radius);
                }

                // reset parameters deciding execution
                getPathModel().pathType = PathModel::PathType::none;
                );
  // TEST
  DEBUG_REQUEST("PathPlanner:test:move_around_ball_test",
                double direction = Math::fromDegrees(30.0);
                MODIFY("PathPlanner:test:move_around_ball_direction", direction);
                double radius    = 210.0;
                MODIFY("PathPlanner:test:move_around_ball_radius", radius);

                PathPlanner::moveAroundBall(direction, radius);
                );
  // Fast forward kick
  // XABSL: fast_forward_kick
  DEBUG_REQUEST("PathPlanner:motion:fast_forward_kick",
                if (getPathModel().pathType == PathModel::PathType::fast_forward_right)
                {
                  PathPlanner::fastForwardRight();
                }
                else if (getPathModel().pathType == PathModel::PathType::fast_forward_left)
                {
                  PathPlanner::fastForwardLeft();
                }

                // reset parameters deciding execution
                getPathModel().pathType = PathModel::PathType::none;
                );
  // TEST
  DEBUG_REQUEST("PathPlanner:test:fast_forward_kick_test_right",
                PathPlanner::fastForwardRight();
                );
  DEBUG_REQUEST("PathPlanner:test:fast_forward_kick_test_left",
                PathPlanner::fastForwardLeft();
                );


  // ------ Helpers ------
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
  // ------ Helpers ------


  // ------ HEAD ------

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
  // ------ HEAD ------
}//end execute


// ------ Helping Functions ------
double clip(double n, double lower, double upper) {
  return std::max(lower, std::min(n, upper));
}
void PathPlanner::lookAtBall()
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
// ------ Helping Functions ------


void PathPlanner::goToBallRight(double distance, double yOffset)
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
void PathPlanner::goToBallLeft(double distance, double yOffset)
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
void PathPlanner::moveAroundBall(double direction, double radius)
{
  lookAtBall();

  Vector2d ballPrev        = getBallModel().positionPreview;
  if (getBallModel().positionPreview.angle() > 30)
  {
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
    getMotionRequest().walkRequest.character  = 0.5; // stable

    getMotionRequest().walkRequest.target.translation.x = 0.0;
    getMotionRequest().walkRequest.target.translation.y = 0.0;
    getMotionRequest().walkRequest.target.rotation      = ballPrev.angle();
  }
  else
  {
    getMotionRequest().id = motion::walk;
    getMotionRequest().walkRequest.coordinate = WalkRequest::Hip;
    getMotionRequest().walkRequest.character  = 0.5; // stable

    getMotionRequest().walkRequest.target.rotation      = ballPrev.angle();
    getMotionRequest().walkRequest.target.translation.x = (ballPrev.abs() - radius) * std::cos(ballPrev.angle());
    // turn right
    if (direction > 0)
    {
      getMotionRequest().walkRequest.target.translation.y = clip(radius * std::tan(clip(-direction, -Math::pi_4, 0)), -100, 0) * std::cos(ballPrev.angle());
    }
    // turn left
    else
    {
      getMotionRequest().walkRequest.target.translation.y = clip(radius * std::tan(clip(-direction, 0, Math::pi_4)), 0, 100) * std::cos(ballPrev.angle());
    }
  }
}
void PathPlanner::fastForwardRight()
{
  Vector2d ballPrev        = getBallModel().positionPreviewInRFoot;

  lookAtBall();

  getMotionRequest().id                     = motion::walk;
  getMotionRequest().walkRequest.coordinate = WalkRequest::RFoot;
  getMotionRequest().walkRequest.character  = 1.0; // fast!

  getMotionRequest().walkRequest.stepControl.moveLeftFoot         = false; // false = right foot
  getMotionRequest().walkRequest.stepControl.target.translation.x = ballPrev.x + 500.0;
  getMotionRequest().walkRequest.stepControl.target.translation.y = 0.0;
  getMotionRequest().walkRequest.stepControl.target.rotation      = 0.0;
  getMotionRequest().walkRequest.stepControl.speedDirection       = 0.0;
  getMotionRequest().walkRequest.stepControl.time                 = 300;
  getMotionRequest().walkRequest.stepControl.scale                = 0.7;
}
void PathPlanner::fastForwardLeft()
{
  Vector2d ballPrev        = getBallModel().positionPreviewInLFoot;

  lookAtBall();

  getMotionRequest().id                     = motion::walk;
  getMotionRequest().walkRequest.coordinate = WalkRequest::LFoot;
  getMotionRequest().walkRequest.character  = 1.0; // fast!

  getMotionRequest().walkRequest.stepControl.moveLeftFoot         = false; // false = right foot
  getMotionRequest().walkRequest.stepControl.target.translation.x = ballPrev.x + 500.0;
  getMotionRequest().walkRequest.stepControl.target.translation.y = 0.0;
  getMotionRequest().walkRequest.stepControl.target.rotation      = 0.0;
  getMotionRequest().walkRequest.stepControl.speedDirection       = 0.0;
  getMotionRequest().walkRequest.stepControl.time                 = 300;
  getMotionRequest().walkRequest.stepControl.scale                = 0.7;
}
