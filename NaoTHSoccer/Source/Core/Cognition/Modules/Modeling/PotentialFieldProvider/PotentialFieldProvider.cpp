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

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:attackDirection:local", "", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:attackDirection:global", "", false);
}


void PotentialFieldProvider::execute()
{
  Vector2<double> p = calculatePotentialField(getBallModel().positionPreview);
  getSoccerStrategy().attackDirection = p;


  DEBUG_REQUEST("PotentialFieldProvider:attackDirection:local",
    FIELD_DRAWING_CONTEXT;
    PEN("FFFFFF", 20);

    Vector2<double> targetDir = getSoccerStrategy().attackDirection;
    targetDir.normalize(200);

    ARROW(
          getBallModel().positionPreview.x,
          getBallModel().positionPreview.y,
          getBallModel().positionPreview.x + targetDir.x,
          getBallModel().positionPreview.y + targetDir.y
          );
  );

  DEBUG_REQUEST("PotentialFieldProvider:attackDirection:global",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    TRANSLATION(getRobotPose().translation.x, getRobotPose().translation.y);
    ROTATION(getRobotPose().rotation);

    Vector2<double> targetDir = getSoccerStrategy().attackDirection;
    targetDir.normalize(200);

    ARROW(
          getBallModel().positionPreview.x,
          getBallModel().positionPreview.y,
          getBallModel().positionPreview.x + targetDir.x,
          getBallModel().positionPreview.y + targetDir.y
          );

    ROTATION(-getRobotPose().rotation);
    TRANSLATION(-getRobotPose().translation.x, -getRobotPose().translation.y);
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
        Vector2<double> f = calculatePotentialField(ball);
        f.normalize(200);
        f += ball;
        ARROW(ball.x, ball.y, f.x, f.y);
      }
    }
  );


  DEBUG_REQUEST("PotentialFieldProvider:goal_field_geometry",
    FIELD_DRAWING_CONTEXT;
    //const double step = 500;
    Pose2D robotPose;
    robotPose.translation.x = 2000.0;
    MODIFY("PotentialFieldProvider:global_drawing_direction", robotPose.rotation);

    GoalModel::Goal gt;
    gt.leftPost = robotPose/getFieldInfo().opponentGoalPostLeft;
    gt.rightPost = robotPose/getFieldInfo().opponentGoalPostRight;
    Vector2<double> ball;
    Vector2<double> goal = getGoalTarget(ball, gt);

    PEN("0000FF", 10);
    CIRCLE(gt.leftPost.x, gt.leftPost.y, 50);
    CIRCLE(gt.rightPost.x, gt.rightPost.y, 50);

    PEN("FF00FF", 10);
    CIRCLE(goal.x, goal.y, 50);
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



  DEBUG_REQUEST("PotentialFieldProvider:draw_ball_approach_field:local",
    FIELD_DRAWING_CONTEXT;
    PEN("666600", 2);

    double yMax = fabs(getBallModel().position.y) + 500;
    double xMax = fabs(getBallModel().position.x) + 500;

    double step = 20;
    for (double x = -100; x <= xMax; x += step)
    {
      for (double y = -yMax; y <= yMax; y += step)
      {
        Vector2<double> pos = Vector2<double>(x,y);
        Vector2<double> target(1000, 1000);

        Vector2<double> f = approachBall(pos, target);
        f.normalize(10);
        f += pos;
        ARROW(pos.x, pos.y, f.x, f.y);
      }
    }
  );
}//end execute

/*
  depends on:
  getSelfLocGoalModel()
  getLocalGoalModel()
  
*/
Vector2<double> PotentialFieldProvider::calculatePotentialField(const Vector2<double>& point)
{
  // choose the goal model
//  Vector2<double> ownGoal = theInstance->goalModel.getTeamGoal(ownColor).calculateCenter();
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
  Vector2<double> oppGoal = getGoalTarget(point, oppGoalModel);

  // preview
  oppGoal = getMotionStatus().plannedMotion.hip / oppGoal;
  // ----------

  /*
  double leftOffset = oppGoalModel->leftPost.y - oppGoalModel->leftPost.abs() * Math::fromDegrees(15);
  double rightOffset = oppGoalModel->rightPost.y + oppGoalModel->rightPost.abs() * Math::fromDegrees(15);

  Vector2<double> oppGoal = oppGoalModel->calculateCenter();
  oppGoal.x += 100;
  if (leftOffset > rightOffset)
  {
    oppGoal.y = Math::clamp(point.y, rightOffset, leftOffset);
  }
  else if ( Math::normalize((oppGoalModel->leftPost-point).angle() - (oppGoalModel->rightPost-point).angle()) < Math::fromDegrees(30) )
  {
    oppGoal = oppGoalModel->calculateCenter();
    Vector2<double> c2r = oppGoalModel->rightPost-oppGoal;
    c2r.rotate(Math::fromDegrees(-90));
    double distance = ( getFieldInfo().xPosOpponentGroundline - getFieldInfo().xPosOpponentPenaltyArea ) * 0.5;
    oppGoal = oppGoal + c2r.normalize(distance);
  }*/
  // end --- getGoal() ---

  Vector2<double> fieldF = calculateGoalPotentialField(oppGoal, point);
  //  fieldF -= calculateGoalPotentialField(ownGoal, ball);

  Vector2<double> playerF;
  bool isAvoiding = false;
  for (vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().opponents.begin(); iter != getPlayersModel().opponents.end(); ++iter)
  {
    if (getFrameInfo().getTimeSince(iter->frameInfoWhenWasSeen.getTime()) < 1000)
    {
      playerF -= calculatePlayerPotentialField(iter->pose.translation, point);
      isAvoiding = true;
    }
  }
  for (vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().teammates.begin(); iter != getPlayersModel().teammates.end(); ++iter)
  {
    if (iter->number != getPlayerInfo().gameData.playerNumber &&
      getFrameInfo().getTimeSince(iter->frameInfoWhenWasSeen.getTime()) < 1000)
    {
      playerF -= calculatePlayerPotentialField(iter->pose.translation, point);
      isAvoiding = true;
    }
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
}//end updateTheFootState


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

  // preview
  Vector2d p = getMotionStatus().plannedMotion.hip / player;

  Vector2<double> v = p-ball;
  double t = v.abs();
  if ( t >= d-100 ) return Vector2<double>();

  return v.normalize() * exp(a / d - a / (d - t));
}//end calculatePlayerPotentialField


Vector2<double> PotentialFieldProvider::getGoal(const Vector2<double>& ball, const GoalModel::Goal& oppGoalModel)
{
  double angle_offset = 15;
  double angle_threshold = 30;
  double dist_threshold = 0;
  MODIFY("potentialfield:angle_offset",angle_offset);
  MODIFY("potentialfield:angle_threshold",angle_threshold);
  MODIFY("potentialfield:dist_threshold",dist_threshold);


  double leftOffset = oppGoalModel.leftPost.y - oppGoalModel.leftPost.abs() * Math::fromDegrees(angle_offset);
  double rightOffset = oppGoalModel.rightPost.y + oppGoalModel.rightPost.abs() * Math::fromDegrees(angle_offset);

  Vector2<double> oppGoal = oppGoalModel.calculateCenter();
  oppGoal.x += 100;
  if (leftOffset > rightOffset)
  {
    oppGoal.y = Math::clamp(ball.y, rightOffset, leftOffset);
    PEN("FF0000", 20);
  }
  else if ( Math::normalize((oppGoalModel.leftPost-ball).angle() - (oppGoalModel.rightPost-ball).angle()) < Math::fromDegrees(angle_threshold) )
  {
    oppGoal = oppGoalModel.calculateCenter();
    Vector2<double> c2r = oppGoalModel.rightPost-oppGoal;
    c2r.rotate(Math::fromDegrees(-90));
    double distance = ( getFieldInfo().xPosOpponentGroundline - getFieldInfo().xPosOpponentPenaltyArea ) * 0.5 + dist_threshold;
    oppGoal = oppGoal + c2r.normalize(distance);
    PEN("0000FF", 20);
  }
  return oppGoal; 
}//end getGoal

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


Vector2<double> PotentialFieldProvider::approachBall(const Vector2<double>& point, const Vector2<double>& target)
{
  double approachDistance = 200;
  MODIFY("potentialfield:approach_ball:distance",approachDistance);

  double repulsor_strength = 2;
  MODIFY("potentialfield:approach_ball:repulsor:strength",repulsor_strength);
  
  // correct the field by target direction
  Vector2<double> dirTarget = (target - getBallModel().position).normalize();

  // ball rupulsor
  Vector2<double> ballRepulsor = calculateBallPotentialField(point, getBallModel().position, dirTarget);



  // adjust rotation around ball
  //double relAngleToBall = Math::pi + (ballRepulsor-dirTarget).angle();
  //double relAngleToBall = -(ballRepulsor-dirTarget).angle();
  //ballRepulsor.rotate(relAngleToBall);

  // targep point attractor
  Vector2<double> approachPos = getBallModel().position - dirTarget*approachDistance;
  Vector2<double> dirApproachPos = (approachPos - point).normalize();

//  if(dirTarget.abs() < 1.0) return ballRepulsor;

  //
  Vector2<double> dirApproachBall = ballRepulsor*repulsor_strength + dirApproachPos; 

  return dirApproachBall;
}//end approachBall

Vector2<double> PotentialFieldProvider::calculateBallPotentialField(const Vector2<double>& point, const Vector2<double>& ball, const Vector2<double>& dir)
{
  double a = 1000;
  MODIFY("potentialfield:approach_ball:repulsor:a",a);
  double d = 600;
  MODIFY("potentialfield:approach_ball:repulsor:d",d);

  Vector2<double> v = point-ball;
  double t = v.abs();
  if ( t >= d-100 ) return Vector2<double>();

  // direction of the repulsion
  // cos_weight == 1.0: full repulsion
  // cos_weight == 0.0: low repulsion
  double cos_weight = (dir*v.normalize() + 1.0)*0.5;

  return v * exp(a / d - a / (d - t))*cos_weight;
}//end calculateBallPotentialField
