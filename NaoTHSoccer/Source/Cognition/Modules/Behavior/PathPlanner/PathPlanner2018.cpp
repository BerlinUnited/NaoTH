/**
* @file PathPlanner2018.cpp
*
* @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
* Implementation of class PathPlanner2018
*/

#include "PathPlanner2018.h"
#include "Tools/Math/Polygon.h"
#include "Tools/Math/Line.h"
#include <forward_list>

PathPlanner2018::PathPlanner2018()
  :
  target_reached(false),
  stepBuffer({}),
  footToUse(Foot::RIGHT),
  lastStepRequestID(getMotionStatus().stepControl.stepRequestID + 1),      // WalkRequest stepRequestID starts at 0, we have to start at 1
  kickPlanned(false)
{
  getDebugParameterList().add(&params);
}

PathPlanner2018::~PathPlanner2018()
{
  getDebugParameterList().remove(&params);
}

double comp(Vector2d b, Vector2d a){
  return a * b / a.abs();
}
// determines if a point p is on the left hand side of the line segment defined by s_begin, s_end
bool on_left_hand_side(Vector2d p, Vector2d s_begin, Vector2d s_end){
  Vector2d s_begin_to_p = p - s_begin;
  Vector2d s_begin_to_s_end = s_end - s_begin;
  Vector2d left_normal(-s_begin_to_s_end.y, s_begin_to_s_end.x);
  return comp(s_begin_to_p, left_normal) >= 0;
}

void PathPlanner2018::execute()
{
  getPathModel().kick_executed = false;

  // Always executed first
  manageStepBuffer();

  // The kick has been executed
  // Tells XABSL to jump into next state
  if (kickPlanned && stepBuffer.empty())
  {
    getPathModel().kick_executed = true;
  }

  // HACK: xabsl set a forced motion request => clear everything
  if (getPathModel().path2018_routine == PathModel::PathPlanner2018Routine::NONE && getMotionRequest().forced) {
    stepBuffer.clear();
    return;
  }

  switch (getPathModel().path2018_routine)
  {
  case PathModel::PathPlanner2018Routine::NONE:
    if (kickPlanned)
    {
      kickPlanned = false;
    }

    // TODO: should the stepBuffer just be cleared here no matter what?
    if (stepBuffer.empty())
    {
      return;
    }
    break;
  case PathModel::PathPlanner2018Routine::AVOID:
    avoid_obstacle(getPathModel().target_point);
    break;
  case PathModel::PathPlanner2018Routine::MOVE_AROUND_BALL2:
    //TODO maybe use a parameter to select the actual routine that is executed when move around is set from the behavior???
    moveAroundBall2(getPathModel().direction, getPathModel().radius, getPathModel().stable);
    break;
  case PathModel::PathPlanner2018Routine::FORWARDKICK:
    if (nearApproach_forwardKick(params.forwardKickOffset.x, params.forwardKickOffset.y))
    {
      forwardKick();
    }
    break;
  case PathModel::PathPlanner2018Routine::SIDEKICK_LEFT:
    if (farApproach())
    {
      if (nearApproach_sideKick(Foot::LEFT, 0.0, params.sidekickOffsetY))
      {
        sideKick(Foot::LEFT);
      }
    }
    break;
  case PathModel::PathPlanner2018Routine::SIDEKICK_RIGHT:
    if (farApproach())
    {
      if (nearApproach_sideKick(Foot::RIGHT, 0.0, -1 * params.sidekickOffsetY))
      {
        sideKick(Foot::RIGHT);
      }
    }
    break;
  case PathModel::PathPlanner2018Routine::SIDESTEP:
    sidesteps(Foot::RIGHT, getPathModel().direction);
  }//end switch

  // Always executed last
  executeStepBuffer();

  PLOT("PathPlanner:buffer_size", static_cast<double>(stepBuffer.size()));
}

void PathPlanner2018::moveAroundBall2(const double direction, const double radius, const bool stable) {
 if (stepBuffer.empty())
 {
    double step_radius = 100;
    double ball_distance = getBallModel().positionPreview.abs();
    Pose2D target_pose;

    Vector2d target_point = getBallModel().positionPreview - Vector2d(cos(direction), sin(direction)) * radius;

    // reset target_reached flag if we moved too much away from target position
    if(target_point.abs() > 0.5 * step_radius
       || fabs(getBallModel().positionPreview.angle()) > Math::fromDegrees(8)) {
        target_reached = false;
    }

    if (target_reached) {
        target_pose = Pose2D();
    } else if(target_point.abs() < step_radius) { // we can reach the target_point directly
        Vector2d tmp_target_point = target_point;
        tmp_target_point.rotate(-getBallModel().positionPreview.angle());
        double angle = std::asin(tmp_target_point.y/radius);
        target_pose = {getBallModel().positionPreview.angle() - angle, target_point};
        target_reached = true;
    } else if(ball_distance >= step_radius + radius) {
        // TODO: maybe make this the "go to ball" ?!
        // we are completely outside of the radius of the ball
        // make step in direction of the target point if it isn't behind the ball
        Vector2d tmp_target_point = target_point;
        tmp_target_point.rotate(-getBallModel().positionPreview.angle());
        if(tmp_target_point.x > ball_distance) {
            tmp_target_point.x = ball_distance;
            tmp_target_point.y = tmp_target_point.y > 0 ? radius : -radius;
        }
        tmp_target_point.rotate(getBallModel().positionPreview.angle());
        target_pose = {getBallModel().positionPreview.angle(), tmp_target_point};
    } else if(ball_distance <= std::max(radius - step_radius, step_radius - radius)){
        // we are completely in the radius of ball
        // make step away from ball in direction of the target point, if possible
        Vector2d tmp_target_point = target_point;
        tmp_target_point.rotate(-getBallModel().positionPreview.angle());
        double angle;
        if(tmp_target_point.x > ball_distance) { // might cross ball so just make a side step
            tmp_target_point = {0, (tmp_target_point.y > 0) ? step_radius : -step_radius};
            angle = std::atan2(tmp_target_point.y, ball_distance);
        } else {
            if(tmp_target_point.abs2() > step_radius * step_radius) {
                tmp_target_point.normalize(step_radius);
            }
            angle = std::atan2(tmp_target_point.y, ball_distance - tmp_target_point.x);
        }

        tmp_target_point.rotate(getBallModel().positionPreview.angle());
        target_pose = {getBallModel().positionPreview.angle() - angle, tmp_target_point};
    } else {
        // step 1: coordinate transformation, the ball has to lie on the x axis
        // so we would rotate about -getBallModel().positionPreview.angle()
        // happens implicitly by using ball_distance

        // step 2: caluclate possible intersection points is1 and is2
        double step_radius2 = step_radius * step_radius;
        double ball_distance2 = getBallModel().positionPreview.abs2();
        double radius2 = radius * radius;
        double x = (step_radius2 - radius2 + ball_distance2) / (2*ball_distance);
        double yy = step_radius2 - x * x;

        assert(yy >= 0.0);

        Vector2d is1(x,  sqrt(yy));
        Vector2d is2(x, -sqrt(yy));

        // need to remember angle for target rotation
        double angle = std::asin(is1.y/radius);

        // step 3: reverse (hidden) coordinate transformation
        is1.rotate(getBallModel().positionPreview.angle());
        is2.rotate(getBallModel().positionPreview.angle());

        // step 4: choose intersection point which is closer to the target point
        if( (is2 - target_point).abs2() < (is1 - target_point).abs2()) {
            target_pose.rotation = getBallModel().positionPreview.angle() + angle;
            target_pose.translation = is2;
        } else {
            target_pose.rotation = getBallModel().positionPreview.angle() - angle;
            target_pose.translation = is1;
        }
    }

    StepBufferElement move_around_step;
    move_around_step.debug_name = "move_around_step2";
    move_around_step.setPose(target_pose);
    move_around_step.setStepType(StepType::WALKSTEP);

    if (stable) {
      move_around_step.setCharacter(params.moveAroundBallCharacterStable);
    } else{
      move_around_step.setCharacter(params.moveAroundBallCharacter);
    }

    move_around_step.setScale(1.0);
    move_around_step.setCoordinate(Coordinate::Hip);
    move_around_step.setFoot(Foot::NONE);
    move_around_step.setSpeedDirection(Math::fromDegrees(0.0));
    move_around_step.setRestriction(RestrictionMode::SOFT);
    move_around_step.setProtected(false);
    move_around_step.setTime(250);

    addStep(move_around_step);

    PLOT("PathPlanner:move_around_ball_2:target:x", target_point.x);
    PLOT("PathPlanner:move_around_ball_2:target:y", target_point.y);
    PLOT("PathPlanner:move_around_ball_2:target:reached", target_reached);
  }
}

void PathPlanner2018::avoid_obstacle(Pose2D target_point){
  using namespace std;
  using namespace Math;

  if (stepBuffer.empty())
  {
    // HACK: limit path length to avoid endless loop
    // TODO: better limit number of iterations
    // TODO: does the algorithm terminate in every case?
    //       What if no valid path exists (reachability)
    int path_length = 0;
    int number_of_retries = 0;
    int max_path_length = 20;
    int max_retries = 20;

    target_point.translation = Vector2d(2000, 0);//getBallModel().position;
    forward_list<Vector2d> path({Vector2d(), target_point.translation});

    auto vertex = path.begin();
    do {
      bool collision = false;
      LineSegment path_segment = LineSegment(*vertex, *next(vertex));

      // handle all obstacles
      for (const Obstacle& obs : getObstacleModel().obstacleList) {
        const auto& obstacle_points = obs.shape_points.getPoints();
        vector<Vector2d> intersection_points;
        vector<Vector2d> obs_vertices;

        // check if the current path segment intersects the current obstacle
        for (auto obs_vertex = obstacle_points.begin(); next(obs_vertex) != obstacle_points.end(); ++obs_vertex) {
          LineSegment polygon_segment = LineSegment(*obs_vertex, *next(obs_vertex));

          //add intersection point with convex polygon to list
          if (path_segment.intersect(polygon_segment)
              && polygon_segment.intersect(path_segment)){
            collision = true;
            double t = path_segment.intersection(polygon_segment);
            intersection_points.push_back(path_segment.point(t));
            obs_vertices.push_back(*obs_vertex);
            obs_vertices.push_back(*next(obs_vertex));
          }

          // ASSUMPTION: convex polygon
          if(intersection_points.size() == 2) break;
        }

        // determine and add new point to the path
        bool replace_next_vertex = false;
        if (collision) {
          // start or end endpoint of the path segment lies inside the polygon
          if(intersection_points.size() == 1) {
            if(on_left_hand_side(*vertex, obs_vertices[0], obs_vertices[1])) { // start point is inside the obstacle
              // TODO: maybe improve how leaving an obstacle is handled
              //       currently ignore that there was a collision
              collision = false;
              continue;
            } else { // end point is inside the obstacle
              if (next(vertex, 2) == path.end()) { // the end point is the target point
                collision = false;
                continue;
              } else {
                replace_next_vertex = true;
                intersection_points.push_back(*next(vertex));
              }
            }
          }

          Vector2d ab = intersection_points[1] - intersection_points[0];
          if (on_left_hand_side(obs.center, intersection_points[0], intersection_points[1])) {
            ab.x = -ab.x;
          } else {
            ab.y = -ab.y;
          }
          swap(ab.x, ab.y);

          // TODO: maybe the new point might be choosen a little bit more intelligently
          //   	   e.g. use a intersection point with other edges of the polygon in the direction of ab
          Vector2d debug_mean = (intersection_points[0] + intersection_points[1]) * 0.5;
          Vector2d new_point = (intersection_points[0] + intersection_points[1]) * 0.5 + ab;
          // debug
          PEN("555555", 1);
          LINE(debug_mean.x, debug_mean.y, new_point.x, new_point.y);
          if(replace_next_vertex) { // the endpoint of the path segment lies inside the polygon
            *next(vertex) = new_point; // so replace it by a new point
            ++number_of_retries;
          } else {
            // add new vertex after current one to the path
            path.insert_after(vertex, new_point);
            ++path_length;
          }

          // debug
          PEN("00FF00", 1);
          for(auto v = path.begin(); next(v) != path.end(); ++v){
              LINE(v->x, v->y, next(v)->x, next(v)->y);
          }

          // the path was changed after the current vertex
          // so we need to check for all obstacles again
          // if the new part is valid
          break;
        }
      } // end obstacle loop

      // there was no collision so the path is valid until the next vertex
      if(!collision) {
          ++vertex;
          number_of_retries = 0;
      }

    } while(next(vertex) != path.end()
            && path_length < max_path_length
            && number_of_retries < max_retries);

    FIELD_DRAWING_CONTEXT;
    if (path_length < max_path_length) {
        PEN("000000", 1);
    } else {
        PEN("FF0000", 1);
    }

    for(auto vertex = path.begin(); next(vertex) != path.end(); ++vertex){
        LINE(vertex->x, vertex->y, next(vertex)->x, next(vertex)->y);
    }


    //StepBufferElement avoid_step;
    //avoid_step.debug_name = "avoid_step";
    //avoid_step.setPose(Pose2D(0.0, path[1]));
    //avoid_step.setStepType(StepType::WALKSTEP);
    //avoid_step.setCharacter(params.moveAroundBallCharacterStable);
    //avoid_step.setScale(1.0);
    //avoid_step.setCoordinate(Coordinate::Hip);
    //avoid_step.setFoot(Foot::NONE);
    //avoid_step.setSpeedDirection(fromDegrees(0.0));
    //avoid_step.setRestriction(RestrictionMode::SOFT);
    //avoid_step.setProtected(false);
    //avoid_step.setTime(250);

    //addStep(avoid_step);
  }
}

bool PathPlanner2018::farApproach()
{
  if (stepBuffer.empty())
  {
    Vector2d ballPos = getBallModel().positionPreview;
    double numPossibleSteps = ballPos.abs() / params.stepLength;

    if (numPossibleSteps > params.farToNearApproachThreshold)
    {
      double translation_xy = params.stepLength;

      StepBufferElement far_approach_step;
      far_approach_step.debug_name = "far_approach_step";
      far_approach_step.setPose({ ballPos.angle(), translation_xy, std::min(translation_xy, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1) });
      far_approach_step.setStepType(StepType::WALKSTEP);
      far_approach_step.setCharacter(0.7);
      far_approach_step.setScale(1.0);
      far_approach_step.setCoordinate(Coordinate::Hip);
      far_approach_step.setFoot(Foot::NONE);
      far_approach_step.setSpeedDirection(Math::fromDegrees(0.0));
      far_approach_step.setRestriction(RestrictionMode::HARD);
      far_approach_step.setProtected(false);
      far_approach_step.setTime(250);

      addStep(far_approach_step);
    }
    else
    {
      return true;
    }
  }

  return false;
}

bool PathPlanner2018::sidesteps(const Foot& foot, const double direction){
  // Always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Coordinate coordinate = Coordinate::Hip;

    if (foot == Foot::RIGHT)
    {
      coordinate = Coordinate::RFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::LFoot;
    }
    else
    {
      ASSERT(false);
    }

    StepBufferElement side_step;
    side_step.setPose({ 0.0, 0.0, direction > 0.0 ? 100.0 : -100.0});
    side_step.setStepType(StepType::WALKSTEP);
    side_step.setCharacter(0.3);
    side_step.setScale(1.0);
    side_step.setCoordinate(coordinate);
    side_step.setFoot(Foot::NONE);
    side_step.setSpeedDirection(Math::fromDegrees(0.0));
    side_step.setRestriction(RestrictionMode::SOFT);
    side_step.setProtected(false);
    side_step.setTime(400);

    addStep(side_step);
    return true;
  }
  else{
    return false;
  }

}

bool PathPlanner2018::nearApproach_forwardKick(const double offsetX, const double offsetY)
{
  // Always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Vector2d ballPos;
    Vector2d targetPos;
    Coordinate coordinate = Coordinate::Hip;

    //if (foot == Foot::RIGHT)
    if (getBallModel().positionPreview.y < 0)
    {
      ballPos    = getBallModel().positionPreviewInRFoot;
      coordinate = Coordinate::RFoot;
    }
    //else if (foot == Foot::LEFT)
    else if (getBallModel().positionPreview.y >= 0)
    {
      coordinate = Coordinate::LFoot;
      ballPos    = getBallModel().positionPreviewInLFoot;
    }
    else
    {
      ASSERT(false);
    }
    // add the desired offset
    targetPos.x = ballPos.x - getFieldInfo().ballRadius - offsetX;
    targetPos.y = ballPos.y - offsetY;

    // Am I ready for a kick or still walking to the ball?
    // Approach further if we are too far away, or foot not aligned to ball or foot to close - We use different thresholds for too far and too close
    if (targetPos.x  > params.forwardKickThreshold_far.x || std::abs(targetPos.y) > params.forwardKickThreshold_far.y || targetPos.x < params.forwardKickThreshold_near.x)
    {
      // generate a correction step
      double translation_xy = params.stepLength;  //TODO kann man nicht die steplength aus den motion nehmen?

      // std::abs(targetPos.y) => das heisst doch wenn der ball in der y richtung springt wird ein schritt zurück geplant und ausgeführt
      // das ist dafür das das er an den ball anlaufen kann ohne zu rotieren. Wenn man nah am ball ist wird angenommen das die Rotation
      // stimmt und dann soll diese auch nicht korrigiert werden
      // TODO: Falls targetPos.x perfekt ist (=0) und abs(targetPos.y) > params.forwardKickThreshold_far.y wird trotzdem ein Schritt zurück gemacht,
      //       obwohl ein einfacher Side-Step ausreichen könnte.
      //       Die obige Erklärung(?) scheint nicht nachvollziehbar.
      double translation_x = std::min(translation_xy, targetPos.x - std::abs(targetPos.y));
      double translation_y = std::min(translation_xy, std::abs(targetPos.y)) * (targetPos.y < 0 ? -1 : 1);

      StepBufferElement near_approach_forward_step;
      near_approach_forward_step.debug_name = "near_approach_forward_step";
      near_approach_forward_step.setPose({ 0.0, translation_x, translation_y });
      near_approach_forward_step.setStepType(StepType::WALKSTEP);
      near_approach_forward_step.setCharacter(params.nearApproach_step_character);
      near_approach_forward_step.setScale(1.0);
      near_approach_forward_step.setCoordinate(coordinate);
      near_approach_forward_step.setFoot(Foot::NONE);
      near_approach_forward_step.setSpeedDirection(Math::fromDegrees(0.0));
      near_approach_forward_step.setRestriction(RestrictionMode::HARD);
      near_approach_forward_step.setProtected(false);
      near_approach_forward_step.setTime(250);

      addStep(near_approach_forward_step);
    }
    else
    {
      return true;
    }
  }

  return false;
}

bool PathPlanner2018::nearApproach_sideKick(const Foot& foot, const double offsetX, const double offsetY)
{
  // TODO: Has to work without rotation (like nearApproach_forwardKick)
  // Always execute the steps that were planned before planning new steps
  if (stepBuffer.empty())
  {
    Vector2d ballPos;
    Coordinate coordinate = Coordinate::Hip;

    if (foot == Foot::RIGHT)
    {
      ballPos = getBallModel().positionPreviewInRFoot;
      coordinate = Coordinate::RFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::LFoot;
      ballPos = getBallModel().positionPreviewInLFoot;
    }
    else
    {
      ASSERT(false);
    }
    // add the desired offset
    ballPos.x += offsetX;
    ballPos.y += offsetY;

    // TODO: Are there better ways to calculate this?
    double numPossibleStepsX = std::abs(ballPos.x) / params.stepLength;
    double numPossibleStepsY = std::abs(ballPos.y) / params.stepLength;

    // Am I ready for a kick ?
    if (numPossibleStepsX > params.readyForSideKickThresholdX
      || numPossibleStepsY > params.readyForSideKickThresholdY)
    {
      double translation_x = std::min(params.stepLength, ballPos.x - getFieldInfo().ballRadius - params.nearApproachSideKickBallPosOffsetX - std::abs(ballPos.y));
      double translation_y = std::min(params.stepLength, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1);

      StepBufferElement new_step;
      new_step.setPose({ 0.0, translation_x, translation_y });
      new_step.setStepType(StepType::WALKSTEP);
      new_step.setCharacter(0.7);
      new_step.setScale(1.0);
      new_step.setCoordinate(coordinate);
      new_step.setFoot(Foot::NONE);
      new_step.setSpeedDirection(Math::fromDegrees(0.0));
      new_step.setRestriction(RestrictionMode::HARD);
      new_step.setProtected(false);
      new_step.setTime(250);

      addStep(new_step);
    }
    else
    {
      MotionStatus::StepControlStatus::MoveableFoot movableFoot = getMotionStatus().stepControl.moveableFoot;

      if (movableFoot != (foot == Foot::RIGHT ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT)
        && movableFoot != MotionStatus::StepControlStatus::BOTH)
      {
        double translation_x = std::min(params.stepLength, ballPos.x - getFieldInfo().ballRadius - params.nearApproachSideKickBallPosOffsetX - std::abs(ballPos.y));
        double translation_y = std::min(params.stepLength, std::abs(ballPos.y)) * (ballPos.y < 0 ? -1 : 1);

        StepBufferElement correction_step;
        correction_step.setPose({ 0.0, translation_x, translation_y });
        correction_step.setStepType(StepType::WALKSTEP);
        correction_step.setCharacter(0.7);
        correction_step.setScale(1.0);
        correction_step.setCoordinate(coordinate);
        correction_step.setFoot(Foot::NONE);
        correction_step.setSpeedDirection(Math::fromDegrees(0.0));
        correction_step.setRestriction(RestrictionMode::HARD);
        correction_step.setProtected(false);
        correction_step.setTime(250);

        addStep(correction_step);
      }
      else if (getMotionStatus().stepControl.moveableFoot == (foot == Foot::RIGHT ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT)
        && movableFoot != MotionStatus::StepControlStatus::BOTH)
      {
        // HACKY
        Vector2d ballPosLeftFoot  = getBallModel().positionPreviewInLFoot;
        Vector2d ballPosRightFoot = getBallModel().positionPreviewInRFoot;
        if (foot == Foot::RIGHT
          && ballPosLeftFoot.abs() < ballPosRightFoot.abs())
        {
          StepBufferElement correction_step;
          correction_step.setPose({ 0.0, 0.0, 0.0 });
          correction_step.setStepType(StepType::WALKSTEP);
          correction_step.setCharacter(0.7);
          correction_step.setScale(1.0);
          correction_step.setCoordinate(Coordinate::LFoot);
          correction_step.setFoot(Foot::NONE);
          correction_step.setSpeedDirection(Math::fromDegrees(0.0));
          correction_step.setRestriction(RestrictionMode::HARD);
          correction_step.setProtected(false);
          correction_step.setTime(250);

          addStep(correction_step);

          correction_step.setCoordinate(Coordinate::RFoot);

          addStep(correction_step);
        }
        else if (foot == Foot::LEFT
          && ballPosRightFoot.abs() < ballPosLeftFoot.abs())
        {
          StepBufferElement correction_step;
          correction_step.setPose({ 0.0, 0.0, 0.0 });
          correction_step.setStepType(StepType::WALKSTEP);
          correction_step.setCharacter(0.7);
          correction_step.setScale(1.0);
          correction_step.setCoordinate(Coordinate::RFoot);
          correction_step.setFoot(Foot::NONE);
          correction_step.setSpeedDirection(Math::fromDegrees(0.0));
          correction_step.setRestriction(RestrictionMode::HARD);
          correction_step.setProtected(false);
          correction_step.setTime(250);

          addStep(correction_step);

          correction_step.setCoordinate(Coordinate::LFoot);

          addStep(correction_step);
        }
      }
      return true;
    }
  }

  return false;
}

void PathPlanner2018::forwardKick()
{
  if (!kickPlanned)
  {
    stepBuffer.clear();

    // 2019 version - makes sure to kick with the foot that is behind the ball
    Vector2d ballPos;
    Foot actual_foot;
    Coordinate coordinate = Coordinate::Hip;
    if (getBallModel().positionPreview.y < 0)
    {
      coordinate = Coordinate::LFoot;
      actual_foot = Foot::RIGHT;
      ballPos    = getBallModel().positionPreviewInRFoot;
    }
    else
    {
      coordinate = Coordinate::RFoot;
      actual_foot = Foot::LEFT;
      ballPos    = getBallModel().positionPreviewInLFoot;
    }

    /*
    // this was used in 2018
    if (foot == Foot::RIGHT)
    {
      coordinate = Coordinate::LFoot;
    }
    else if (foot == Foot::LEFT)
    {
      coordinate = Coordinate::RFoot;
    }
    else
    {
      ASSERT(false);
    }
    */

    // Correction step if the movable foot is different from the foot that is supposed to kick
    if (getMotionStatus().stepControl.moveableFoot != (getBallModel().positionPreview.y < 0 ? MotionStatus::StepControlStatus::RIGHT : MotionStatus::StepControlStatus::LEFT))
    {
      StepBufferElement forward_correction_step("forward_correction_step");
      forward_correction_step
        .setPose({ 0.0, 100.0, 0.0 })
        .setStepType(StepType::WALKSTEP)
        .setCharacter(1.0)
        .setScale(1.0)
        .setCoordinate(coordinate)
        .setFoot(Foot::NONE)
        .setSpeedDirection(Math::fromDegrees(0.0))
        .setRestriction(RestrictionMode::HARD)
        .setProtected(false)
        .setTime(250);

      addStep(forward_correction_step);
    }

    // The kick
    StepBufferElement forward_kick_step;
    forward_kick_step
      .setPose({ 0.0, 500.0, 0.0 }) // kick straight forward
      .setStepType(StepType::KICKSTEP)
      .setCharacter(1.0)
      .setScale(0.7)
      .setCoordinate(coordinate)
      .setFoot(actual_foot)
      .setSpeedDirection(Math::fromDegrees(0.0))
      .setRestriction(RestrictionMode::SOFT)
      .setProtected(true)
      .setTime(params.forwardKickTime);

    // NOTE: change the kick pose if the parameter is set
    if(params.forwardKickAdaptive) {
      forward_kick_step.setPose({ 0.0, ballPos.x, ballPos.y }); // kick towards the ball
    }

    addStep(forward_kick_step);

    // The zero step
    forward_kick_step.setStepType(StepType::ZEROSTEP);
    addStep(forward_kick_step);

    // The retracting walk step
    forward_kick_step.setPose({ 0.0, 0.0, 0.0 });
    forward_kick_step.setStepType(StepType::WALKSTEP);
    addStep(forward_kick_step);

    kickPlanned = true;
  }
}

void PathPlanner2018::sideKick(const Foot& foot) // Foot == RIGHT means that we want to kick with the right foot to the left side
{
  if (stepBuffer.empty() && !kickPlanned)
  {
    Coordinate coordinate = Coordinate::Hip;
    double stepY          = 0.0;
    double speedDirection = 0.0;

    if (foot == Foot::RIGHT)
    {
      coordinate     = Coordinate::LFoot;
      stepY          = 100.0;
      speedDirection = Math::fromDegrees(90);
    }
    else if (foot == Foot::LEFT)
    {
      coordinate     = Coordinate::RFoot;
      stepY          = -100.0;
      speedDirection = Math::fromDegrees(-90);
    }
    else
    {
      ASSERT(false);
    }

    // The kick
    StepBufferElement new_step;
    new_step.setPose({ 0.0, 500.0, stepY });
    new_step.setStepType(StepType::KICKSTEP);
    new_step.setCharacter(1.0);
    new_step.setScale(1.0);
    new_step.setCoordinate(coordinate);
    new_step.setFoot(foot);
    new_step.setSpeedDirection(speedDirection);
    new_step.setRestriction(RestrictionMode::SOFT);
    new_step.setProtected(true);
    new_step.setTime(params.sideKickTime);

    addStep(new_step);

    // The zero step
    new_step.setStepType(StepType::ZEROSTEP);
    addStep(new_step);

    // The retracting walk step
    new_step.setPose({ 0.0, 0.0, 0.0 });
    new_step.setStepType(StepType::WALKSTEP);
    addStep(new_step);

    kickPlanned = true;
  }
}


void PathPlanner2018::addStep(const StepBufferElement& new_step) {
  stepBuffer.push_back(new_step);
}

void PathPlanner2018::updateSpecificStep(const unsigned int index, StepBufferElement& step)
{
  ASSERT(stepBuffer.size() > 0);
  ASSERT(stepBuffer.size() >= index);

  stepBuffer[index] = step;
}

void PathPlanner2018::manageStepBuffer()
{
  if (stepBuffer.empty())
  {
    return;
  }

  // requested step has been accepted
  if (lastStepRequestID == getMotionStatus().stepControl.stepRequestID)
  {
    /*std::string lastStepType = "";
    if (stepBuffer[0].type == StepType::KICKSTEP)
    {
      lastStepType = "KICKSTEP";
    }
    else if (stepBuffer[0].type == StepType::WALKSTEP)
    {
      lastStepType = "WALKSTEP";
    }
    else if (stepBuffer[0].type == StepType::ZEROSTEP)
    {
      lastStepType = "ZEROSTEP";
    }

    std::cout << "Last executed step: " << lastStepType << " -- " << numPossibleSteps << " > " << params.readyForKickThreshold << " or " << numRotationStepsNecessary << " > " << numPossibleSteps << std::endl;
    */

    stepBuffer.erase(stepBuffer.begin());
    lastStepRequestID = getMotionStatus().stepControl.stepRequestID + 1;
  }
}

void PathPlanner2018::executeStepBuffer()
{
  STOPWATCH_START("PathPlanner2018:execute_steplist");

  if (stepBuffer.empty()) {
    return;
  }

  // normal walking WALKSTEPs use Foot::NONE, for KICKSTEPs the foot to use has to be specified
  if (stepBuffer.front().foot == Foot::NONE)
  {
    switch (getMotionStatus().stepControl.moveableFoot)
    {
    case MotionStatus::StepControlStatus::LEFT:
      footToUse = Foot::LEFT;
      break;
    case MotionStatus::StepControlStatus::RIGHT:
      footToUse = Foot::RIGHT;
      break;
    case MotionStatus::StepControlStatus::BOTH:
      if (stepBuffer.front().pose.translation.y > 0.0f || stepBuffer.front().pose.rotation > 0.0f) {
        footToUse = Foot::LEFT;
      } else {
        footToUse = Foot::RIGHT;
      }
      break;
    case MotionStatus::StepControlStatus::NONE:
      footToUse = Foot::RIGHT;
      break;
    }
  }
  else
  {
    footToUse = stepBuffer.front().foot;
  }

  //set motion request
  getMotionRequest().id                                     = motion::walk;
  getMotionRequest().walkRequest.stepControl.stepID         = getMotionStatus().stepControl.stepID;

  getMotionRequest().walkRequest.coordinate                 = stepBuffer.front().coordinate;
  getMotionRequest().walkRequest.character                  = stepBuffer.front().character;

  getMotionRequest().walkRequest.stepControl.scale          = stepBuffer.front().scale;
  getMotionRequest().walkRequest.stepControl.type           = stepBuffer.front().type;
  getMotionRequest().walkRequest.stepControl.time           = stepBuffer.front().time;
  getMotionRequest().walkRequest.stepControl.speedDirection = stepBuffer.front().speedDirection;
  getMotionRequest().walkRequest.stepControl.target         = stepBuffer.front().pose;
  getMotionRequest().walkRequest.stepControl.restriction    = stepBuffer.front().restriction;
  getMotionRequest().walkRequest.stepControl.isProtected    = stepBuffer.front().isProtected;
  getMotionRequest().walkRequest.stepControl.stepRequestID  = lastStepRequestID;
  getMotionRequest().walkRequest.stepControl.moveLeftFoot   = (footToUse != Foot::RIGHT); // false means right foot

  //std::cout << stepBuffer.front().debug_name << " - " << getMotionRequest().walkRequest.stepControl.moveLeftFoot  << std::endl;
  STOPWATCH_STOP("PathPlanner2018:execute_steplist");
}
