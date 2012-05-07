/**
* @file PotentialFieldProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class PotentialFieldProvider
*/

#include "PotentialFieldProvider.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"

PotentialFieldProvider::PotentialFieldProvider()
{
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_potential_field:local","draw local potential field", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_potential_field:global","draw gobal potential field", false);

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:goal_field_geometry","...", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:different_colors","...", false);

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_ball_approach_field:local", "...", false);

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:attackDirection", "draw attack direction", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:goal_target", "draw goal target", false);
}


void PotentialFieldProvider::execute()
{
  const Pose2D& robotPose = getRobotPose();
  Vector2<double> ballRelative = getBallModel().positionPreview;

  // choose the goal model
  GoalModel::Goal oppGoalModel = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());

  //FIXME: frameWhenOpponentGoalWasSeen not supported by Model anymore!
  if ( getFrameInfo().getTimeSince(getLocalGoalModel().frameWhenOpponentGoalWasSeen.getTime()) < 10000
    //&& getLocalGoalModel().someGoalWasSeen
    && getLocalGoalModel().opponentGoalIsValid )
  {
    const GoalModel::Goal localOppGoalModel = getLocalGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
    if ( localOppGoalModel.calculateCenter().abs() < 4000 )
    {
      oppGoalModel = localOppGoalModel;
    }
  }

  // begin --- getGoal() ---
  Vector2<double> targetPoint = getGoalTarget(ballRelative, oppGoalModel);
  // preview
  targetPoint = getMotionStatus().plannedMotion.hip / targetPoint;
  // ----------


  DEBUG_REQUEST("PotentialFieldProvider:goal_target",
    FIELD_DRAWING_CONTEXT;


    PEN("0000FF", 10);
    CIRCLE((robotPose * oppGoalModel.leftPost).x, (robotPose * oppGoalModel.leftPost).y, 50);
    CIRCLE((robotPose * oppGoalModel.rightPost).x, (robotPose * oppGoalModel.rightPost).y, 50);

    PEN("FF00FF", 10);
    CIRCLE((robotPose * targetPoint).x, (robotPose * targetPoint).y, 50);
  );

  std::list<Vector2<double> > obstacles = getValidObstacles();

  Vector2<double> p = calculatePotentialField(ballRelative, targetPoint, obstacles);
  getRawAttackDirection().attackDirection = p;

  DEBUG_REQUEST("PotentialFieldProvider:attackDirection",
    FIELD_DRAWING_CONTEXT;

    PEN("FFFFFF", 20);

    Vector2<double> targetDir = getRawAttackDirection().attackDirection;
    targetDir.normalize(200);

    Vector2<double> globalBall
        = getRobotPose()*ballRelative;
    CIRCLE(globalBall.x, globalBall.y, 25);
    Vector2<double> globalTarget = getRobotPose()*targetDir;

    ARROW(
          globalBall.x,
          globalBall.y,
          globalTarget.x,
          globalTarget.y
          );
  );


  DEBUG_REQUEST("PotentialFieldProvider:draw_potential_field:local",
    FIELD_DRAWING_CONTEXT;
    PEN("FFFFFF", 20);

    double step = 500;
    for (double x = getFieldInfo().xPosOwnGroundline; x <= getFieldInfo().xPosOpponentGroundline; x += step)
    {
      for (double y = getFieldInfo().yPosRightSideline; y <= getFieldInfo().yPosLeftSideline; y += step)
      {
        Vector2<double> ball = getRobotPose()/Vector2<double>(x,y);
        Vector2<double> f = calculatePotentialField(ball, targetPoint, obstacles);
        f.normalize(200);
        f += ball;
        ARROW(ball.x, ball.y, f.x, f.y);
      }
    }
  );


  DEBUG_REQUEST("PotentialFieldProvider:goal_field_geometry",
    FIELD_DRAWING_CONTEXT;

    GoalModel::Goal gt = getSelfLocGoalModel().goal;
    Vector2<double> ball;
    Vector2<double> goalTarget = getGoalTarget(ball, gt);

    PEN("0000FF", 10);
    CIRCLE(gt.leftPost.x, gt.leftPost.y, 50);
    CIRCLE(gt.rightPost.x, gt.rightPost.y, 50);

    PEN("FF00FF", 10);
    CIRCLE(goalTarget.x, goalTarget.y, 50);
  );


  DEBUG_REQUEST("PotentialFieldProvider:draw_potential_field:global",
    FIELD_DRAWING_CONTEXT;

    const double stepX = (getFieldInfo().xPosOpponentGroundline - getFieldInfo().xPosOwnGroundline)/50.0;
    const double stepY = (getFieldInfo().yPosLeftSideline - getFieldInfo().yPosRightSideline)/50.0;

    Pose2D robotPose;
    MODIFY("PotentialFieldProvider:global_drawing_direction", robotPose.rotation);

    GoalModel::Goal gt;
    gt.leftPost = robotPose/getFieldInfo().opponentGoalPostLeft;
    gt.rightPost = robotPose/getFieldInfo().opponentGoalPostRight;
    PEN("0000FF", 10);
    CIRCLE(gt.leftPost.x, gt.leftPost.y, 50);
    CIRCLE(gt.rightPost.x, gt.rightPost.y, 50);

    for (robotPose.translation.x = getFieldInfo().xPosOwnGroundline; robotPose.translation.x <= getFieldInfo().xPosOpponentGroundline; robotPose.translation.x += stepX)
    {
      for (robotPose.translation.y = getFieldInfo().yPosRightSideline; robotPose.translation.y <= getFieldInfo().yPosLeftSideline; robotPose.translation.y += stepY)
      {
        PEN("FFFFFF", 5);
        GoalModel::Goal g;
        g.leftPost = robotPose/getFieldInfo().opponentGoalPostLeft;
        g.rightPost = robotPose/getFieldInfo().opponentGoalPostRight;

        Vector2<double> ball;
        Vector2<double> goal = getGoalTarget(ball, g);
        
        Vector2<double> f = calculateGoalPotentialField(goal, ball);

        f.normalize(50);
        f = robotPose*f;
        ARROW(robotPose.translation.x, robotPose.translation.y, f.x, f.y);
      }
    }
  );
}//end execute

list<Vector2<double> > PotentialFieldProvider::getValidObstacles()
{
  std::list<Vector2<double> > result;

  for (vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().opponents.begin(); iter != getPlayersModel().opponents.end(); ++iter)
  {
    if (getFrameInfo().getTimeSince(iter->frameInfoWhenWasSeen.getTime()) < 1000)
    {
      // preview
      Vector2d p = getMotionStatus().plannedMotion.hip / iter->pose.translation;

      result.push_back(p);
    }
  }
  for (vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().teammates.begin(); iter != getPlayersModel().teammates.end(); ++iter)
  {
    if (iter->number != getPlayerInfo().gameData.playerNumber &&
      getFrameInfo().getTimeSince(iter->frameInfoWhenWasSeen.getTime()) < 1000)
    {
      // preview
      Vector2d p = getMotionStatus().plannedMotion.hip / iter->pose.translation;
      result.push_back(p);
    }
  }
  return result;
} // end getValidOpponents()


Vector2<double> PotentialFieldProvider::calculatePotentialField(
    const Vector2<double>& point, const Vector2<double>& targetPoint,
    const list<Vector2<double> > &obstacles)
{

  Vector2<double> fieldF = calculateGoalPotentialField(targetPoint, point);

  Vector2<double> playerF;
  bool isAvoiding = false;
  for (list<Vector2<double> >::const_iterator iter =
       obstacles.begin(); iter != obstacles.end(); ++iter)
  {
    playerF -= calculatePlayerPotentialField(*iter, point);
    isAvoiding = true;
  }

  if (isAvoiding)
  {
    // my self
    playerF -= calculatePlayerPotentialField(Vector2<double>(0, 0), point);
  }

  double ff = fieldF.abs() * 0.8;
  if ( playerF.abs() > ff)
  {
    playerF.normalize(ff);
  }
  return fieldF + playerF;
}//end calculatePotentialField


Vector2<double> PotentialFieldProvider::calculateGoalPotentialField(const Vector2<double>& goal, const Vector2<double>& point)
{
  const double alpha = -0.001;

  Vector2<double> v = goal - point;
  double d = v.abs();
  
  return v.normalize() * exp(alpha * d);
}//end calculateGoalPotentialField

Vector2<double> PotentialFieldProvider::calculatePlayerPotentialField(const Vector2<double>& player, const Vector2<double>& ball)
{
  const double a = 1500;
  const double d = 2000;

  Vector2<double> v = player-ball;
  double t = v.abs();
  if ( t >= d-100 ) return Vector2<double>();

  return v.normalize() * exp(a / d - a / (d - t));
}//end calculatePlayerPotentialField

Vector2<double> PotentialFieldProvider::getGoalTarget(const Vector2<double>& point, const GoalModel::Goal& oppGoalModel)
{
  double angle_inner_threshold = 15;
  double angle_outer_threshold = 30;
  double dist_threshold = 0;
  MODIFY("potentialfield:angle_inner_threshold",angle_inner_threshold);
  MODIFY("potentialfield:angle_outer_threshold",angle_outer_threshold);
  MODIFY("potentialfield:dist_threshold",dist_threshold);

  double leftOffset = oppGoalModel.leftPost.abs() * Math::fromDegrees(angle_inner_threshold);
  double rightOffset = oppGoalModel.rightPost.abs() * Math::fromDegrees(angle_inner_threshold);
  
  Vector2<double> goalCenter = oppGoalModel.calculateCenter();
  Vector2<double> goalNormal = oppGoalModel.rightPost - goalCenter;
  goalNormal.rotateLeft().normalize();
  Vector2<double> goalTangential = oppGoalModel.leftPost - goalCenter;
  goalTangential.normalize();


  Vector2<double> leftPost = oppGoalModel.leftPost - goalTangential*leftOffset;
  Vector2<double> rightPost = oppGoalModel.rightPost + goalTangential*rightOffset;

  double goalAngle = fabs(Math::normalize((leftPost-point).angle() - (rightPost-point).angle()));
  Math::LineSegment goalLine(leftPost, rightPost);

  if (goalAngle > Math::fromDegrees(angle_outer_threshold))
  {
    double t = goalLine.project(point);
    goalCenter = goalLine.point(t);
    
    // 10cm behind the actual goal line
    goalCenter += goalNormal*100;
    DEBUG_REQUEST("PotentialFieldProvider:different_colors", PEN("FF0000", 5); );
  }
  else if ( goalAngle < Math::fromDegrees(angle_inner_threshold) )
  {
    double distance = ( getFieldInfo().xPosOpponentGroundline - getFieldInfo().xPosOpponentPenaltyArea ) * 0.5 + dist_threshold;
    goalCenter -= goalNormal*distance;

    Math::LineSegment penaltyLine(goalCenter + goalTangential*distance, goalCenter - goalTangential*distance);
    goalCenter = penaltyLine.point(penaltyLine.project(point));

    DEBUG_REQUEST("PotentialFieldProvider:different_colors", PEN("0000FF", 5); );
  }else
  {
    DEBUG_REQUEST("PotentialFieldProvider:different_colors", PEN("000000", 5); );
    // 10cm behind the actual goal line
    goalCenter += goalNormal*100;
  }

  return goalCenter; 
}//end getGoalTarget
