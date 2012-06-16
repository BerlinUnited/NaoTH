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
#include "Tools/Math/Line.h"

using namespace naoth;

PotentialFieldProvider::PotentialFieldProvider()
{
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:goal_target", "draw goal target", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:goal_field_geometry","some debug stuff for how the target is calculated", false);

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:attackDirection:local", "it is what it is", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:attackDirection:global", "it is what it is", false);

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_potential_field:different_colors","...", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_potential_field:local","draw local potential field", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_potential_field:global","draw gobal potential field", false);
  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_potential_field:high_res","draw gobal potential field with high resolution around the opponent goal", false);

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:draw_potential_field:sensitivity", "", false);

  DEBUG_REQUEST_REGISTER("PotentialFieldProvider:getGoalTargetOld", "", false);
}


void PotentialFieldProvider::execute()
{
  const Pose2D& robotPose = getRobotPose();
  Vector2<double> ballRelative = getBallModel().position;


  // choose the goal model:
  // use the selflocator by default
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

  // calculate the target point to play the ball to based on the 
  // goal model and the ball model 
  Vector2<double> targetPoint = getGoalTarget(ballRelative, oppGoalModel);
  
  
  DEBUG_REQUEST("PotentialFieldProvider:goal_target",
    FIELD_DRAWING_CONTEXT;

    PEN("0000FF", 10);
    CIRCLE((robotPose * oppGoalModel.leftPost).x, (robotPose * oppGoalModel.leftPost).y, 50);
    CIRCLE((robotPose * oppGoalModel.rightPost).x, (robotPose * oppGoalModel.rightPost).y, 50);

    PEN("FF00FF", 10);
    CIRCLE((robotPose * targetPoint).x, (robotPose * targetPoint).y, 50);
  );

  // get valide obstacles
  std::list<Vector2<double> > obstacles = getValidObstacles();


  // calculate the potential field at the ball
  Vector2<double> attackDirection = calculatePotentialField(ballRelative, targetPoint, obstacles);
  getRawAttackDirection().attackDirection = attackDirection;


  // ATTENTION: PREVIEW
  // while aplying preview consider following facts:
  // - transformation has to be applied to all points involved in the calculations
  //   (i.e., ball, goal(target point), opponents, myself)
  // - because all our functions are homogeneous with respect to linear transformations
  //   we may aply the preview AFTER all calculations are done
  //   (in particular it is because ||T*x + T*y|| = ||T||*||x-y|| = 1*||x-y||)
  // - in case it is not true in the future, I suggest to apply the preview
  //   in calculatePotentialField(...) to all points uniformly
  // ...
  // so for now it is much easier to calculate the previews in the behavior
  // (check StrategySymbols for this)
  //
  //Vector2<double> attackDirectionPreviewHip = getMotionStatus().plannedMotion.hip / attackDirection;
  //Vector2<double> attackDirectionPreviewLFoot = getMotionStatus().plannedMotion.lFoot / attackDirection;
  //Vector2<double> attackDirectionPreviewRFoot = getMotionStatus().plannedMotion.rFoot / attackDirection;


  DEBUG_REQUEST("PotentialFieldProvider:attackDirection:local",
    FIELD_DRAWING_CONTEXT;

    PEN("FFFFFF", 20);

    Vector2<double> targetDir = getRawAttackDirection().attackDirection;
    targetDir.normalize(200);

    Vector2<double> globalBall
        = getRobotPose()*ballRelative;
    CIRCLE(globalBall.x, globalBall.y, 25);
    Vector2<double> globalTarget = getRobotPose()*(ballRelative+targetDir);

    ARROW(
          globalBall.x,
          globalBall.y,
          globalTarget.x,
          globalTarget.y
          );
  );

  DEBUG_REQUEST("PotentialFieldProvider:attackDirection:global",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    TRANSLATION(getRobotPose().translation.x, getRobotPose().translation.y);
    ROTATION(getRobotPose().rotation);

    Vector2<double> targetDir = getRawAttackDirection().attackDirection;
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


  DEBUG_REQUEST("PotentialFieldProvider:draw_potential_field:global",
    FIELD_DRAWING_CONTEXT;
    PEN("FFFFFF", 5);

    const double stepX = getFieldInfo().xFieldLength/50.0;
    const double stepY = getFieldInfo().yFieldLength/50.0;

    Vector2<double> simulatedGlobalBall;
    for (simulatedGlobalBall.x = -getFieldInfo().xFieldLength/2.0;
         simulatedGlobalBall.x <= getFieldInfo().xFieldLength/2.0;
         simulatedGlobalBall.x += stepX)
    {
      for (simulatedGlobalBall.y = -getFieldInfo().yFieldLength/2.0;
           simulatedGlobalBall.y <= getFieldInfo().yFieldLength/2.0;
           simulatedGlobalBall.y += stepY)
      {
        // claculate the local attack direction for the current 
        // robots position and current obstacles
        Vector2<double> simulatedLocalBall = robotPose/simulatedGlobalBall;
        Vector2<double> target = getGoalTarget(simulatedLocalBall, oppGoalModel);
        Vector2<double> f = calculatePotentialField(simulatedLocalBall, target, obstacles);

        // transform it to global coordinates
        f.normalize(50);
        f = robotPose*(f+simulatedLocalBall);
        
        ARROW(simulatedGlobalBall.x, simulatedGlobalBall.y, f.x, f.y);
      }
    }
  );

  

  DEBUG_REQUEST("PotentialFieldProvider:draw_potential_field:high_res",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);

    const double stepX = 20;
    const double stepY = 20;

    Vector2<double> simulatedGlobalBall;
    for (simulatedGlobalBall.x = getFieldInfo().opponentGoalCenter.x - 1000;
         simulatedGlobalBall.x <= getFieldInfo().opponentGoalCenter.x + 500;
         simulatedGlobalBall.x += stepX)
    {
      for (simulatedGlobalBall.y = getFieldInfo().opponentGoalPostRight.y - 500;
           simulatedGlobalBall.y <= getFieldInfo().opponentGoalPostLeft.y + 500;
           simulatedGlobalBall.y += stepY)
      {
        // claculate the local attack direction for the current 
        // robots position and current obstacles
        Vector2<double> simulatedLocalBall = robotPose/simulatedGlobalBall;
        Vector2<double> target = getGoalTarget(simulatedLocalBall, oppGoalModel);
        Vector2<double> f = calculatePotentialField(simulatedLocalBall, target, obstacles);

        // transform it to global coordinates
        // ATTENTION: since it is a vector and not a point, we apply only the rotation
        f.rotate(robotPose.rotation);

        //ARROW(simulatedGlobalBall.x, simulatedGlobalBall.y, f.x, f.y);
        SIMPLE_PARTICLE(simulatedGlobalBall.x, simulatedGlobalBall.y, f.angle());
      }
    }
  );



  DEBUG_REQUEST("PotentialFieldProvider:draw_potential_field:sensitivity",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 1);

    const double stepX = 20;
    const double stepY = 20;

    const double noise = 25; // mm
    const double trials = 10;
    
    Vector2<double> simulatedGlobalBall;
    for (simulatedGlobalBall.x = getFieldInfo().opponentGoalCenter.x - 1000;
         simulatedGlobalBall.x <= getFieldInfo().opponentGoalCenter.x + 500;
         simulatedGlobalBall.x += stepX)
    {
      for (simulatedGlobalBall.y = getFieldInfo().opponentGoalPostRight.y - 500;
           simulatedGlobalBall.y <= getFieldInfo().opponentGoalPostLeft.y + 500;
           simulatedGlobalBall.y += stepY)
      {

        // claculate the local attack direction for the current 
        // robots position and current obstacles
        Vector2<double> simulatedLocalBall = robotPose/simulatedGlobalBall;
        Vector2<double> target = getGoalTarget(simulatedLocalBall, oppGoalModel);
        Vector2<double> f = calculatePotentialField(simulatedLocalBall, target, obstacles);

        // transform it to global coordinates
        // ATTENTION: since it is a vector and not a point, we apply only the rotation
        f.rotate(robotPose.rotation);
        f.normalize();

        double deviation = 0;

        // apply some random noise
        for(int i = 0; i < trials; i++)
        {
          Vector2<double> simulatedLocalBall_noise = robotPose/simulatedGlobalBall;
          simulatedLocalBall_noise.x += (Math::random()-0.5)*2.0*noise;
          simulatedLocalBall_noise.y += (Math::random()-0.5)*2.0*noise;

          Vector2<double> target_noice = getGoalTarget(simulatedLocalBall_noise, oppGoalModel);
          Vector2<double> f_noice = calculatePotentialField(simulatedLocalBall_noise, target, obstacles);

          // transform it to global coordinates
          // ATTENTION: since it is a vector and not a point, we apply only the rotation
          f_noice.rotate(robotPose.rotation);
          f_noice.normalize();

          deviation = max(deviation, fabs(Math::toDegrees(Math::normalize(f.angle() - f_noice.angle()))));
        }


        //ARROW(simulatedGlobalBall.x, simulatedGlobalBall.y, f.x, f.y);
        //SIMPLE_PARTICLE(simulatedGlobalBall.x, simulatedGlobalBall.y, f.angle());

        FILLOVAL(simulatedGlobalBall.x, simulatedGlobalBall.y, deviation*0.5, deviation*0.5);
      }
    }
  );
  
  
}//end execute

list<Vector2<double> > PotentialFieldProvider::getValidObstacles() const
{
  std::list<Vector2<double> > result;

  // opponents
  for (vector<PlayersModel::Player>::const_iterator iter =
       getPlayersModel().opponents.begin();
       iter != getPlayersModel().opponents.end(); ++iter)
  {
    if (iter->frameInfoWhenWasSeen.getFrameNumber() != 0 &&
      getFrameInfo().getTimeSince(iter->frameInfoWhenWasSeen.getTime()) < 1000)
    {
      result.push_back(iter->pose.translation);
    }
  }

  // teammates
  for (vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().teammates.begin(); iter != getPlayersModel().teammates.end(); ++iter)
  {
    if (iter->number != getPlayerInfo().gameData.playerNumber &&
      iter->frameInfoWhenWasSeen.getFrameNumber() != 0 &&
      getFrameInfo().getTimeSince(iter->frameInfoWhenWasSeen.getTime()) < 1000)
    {
      result.push_back(iter->pose.translation);
    }
  }
  return result;
} //end getValidOpponents


Vector2<double> PotentialFieldProvider::calculatePotentialField(
    const Vector2<double>& point, 
    const Vector2<double>& targetPoint,
    const list<Vector2<double> > &obstacles) const
{
  // we are attracted to the target point
  Vector2<double> fieldF = globalExponentialAttractor(targetPoint, point);

  // we are repelled by the opponents
  Vector2<double> playerF;
  for (list<Vector2<double> >::const_iterator iter =
       obstacles.begin(); iter != obstacles.end(); ++iter)
  {
    playerF -= compactExponentialRepeller(*iter, point);
  }

  if (!obstacles.empty())
  {
    // my self
    playerF -= compactExponentialRepeller(Vector2<double>(0, 0), point);
  }

  // TODO: remove magic normalization
  double ff = fieldF.abs() * 0.8;
  if ( playerF.abs() > ff)
  {
    playerF.normalize(ff);
  }

  return fieldF + playerF;
}//end calculatePotentialField


Vector2<double> PotentialFieldProvider::globalExponentialAttractor(const Vector2<double>& p, const Vector2<double>& x) const
{
  const double alpha = theParameters.goal_attractor_strength; //-0.001;

  Vector2<double> v = p - x;
  double d = v.abs();
  
  return v.normalize() * exp(alpha * d);
}//end globalExponentialAttractor

Vector2<double> PotentialFieldProvider::compactExponentialRepeller(const Vector2<double>& p, const Vector2<double>& x) const
{
  const double a = theParameters.player_repeller_strenth; //1500; 
  const double d = theParameters.player_repeller_radius; //2000;

  Vector2<double> v = p - x;
  double t = v.abs();
  if ( t >= d-100 ) return Vector2<double>();

  return v.normalize() * exp(a / d - a / (d - t));
}//end compactExponentialRepeller


Vector2<double> PotentialFieldProvider::getGoalTargetOld(const Vector2<double>& point, const GoalModel::Goal& oppGoalModel) const
{
  double postOffset = 100.0;
  double goalLineOffset = 100.0;
  MODIFY("potentialfield:postOffset", postOffset);
  MODIFY("potentialfield:goalLineOffset", goalLineOffset);


  // relative vector from one post to another
  Vector2<double> leftPost2RightPost = oppGoalModel.rightPost - oppGoalModel.leftPost;
  Vector2<double> rightPost2LeftPost = oppGoalModel.leftPost - oppGoalModel.rightPost;

  // the endpoints of our line are a shortened version of the goal line
  Vector2<double> leftEndpoint = oppGoalModel.leftPost
      + leftPost2RightPost.normalize(postOffset);
  Vector2<double> rightEndpoint = oppGoalModel.rightPost
      + rightPost2LeftPost.normalize(postOffset);

  DEBUG_REQUEST("PotentialFieldProvider:goal_target",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 10);
    CIRCLE((getRobotPose()*leftEndpoint).x, (getRobotPose()*leftEndpoint).y, 100);
    CIRCLE((getRobotPose()*rightEndpoint).x, (getRobotPose()*rightEndpoint).y, 100);
  );

  // this is the goalline we are shooting for
  Math::LineSegment goalLine(leftEndpoint, rightEndpoint);

  // project the point on the goal line
  Vector2<double> target = goalLine.projection(point);

  // a normal vector ponting from the goal towards the field
  Vector2<double> goalNormal = leftPost2RightPost.rotateRight();

  // get distance between point and target point
  double dist = (target-point).abs();

  if(dist <= goalLineOffset)
  {
    // the point is quite near to the goal line, put the target point behind the
    // goal line
    target = target - goalNormal.normalize(goalLineOffset);
  }
  else
  {
    // the point is far away from the goal line, put the target point before the
    // goal line
    target = target + goalNormal.normalize(goalLineOffset);
  }

  return target;
}//end getGoalTarget

Vector2<double> PotentialFieldProvider::getGoalTarget(const Vector2<double>& point, const GoalModel::Goal& oppGoalModel) const
{
  // for debug reasons
  DEBUG_REQUEST("PotentialFieldProvider:getGoalTargetOld",
    return getGoalTargetOld(point, oppGoalModel);
  );

  double postOffset = 200.0;
  double goalLineOffsetFront = 100.0;
  double goalLineOffsetBack = 100.0;
  MODIFY("potentialfield:postOffset", postOffset);
  MODIFY("potentialfield:goalLineOffset", goalLineOffsetFront);
  MODIFY("potentialfield:goalLineOffset", goalLineOffsetBack);

  // normalized vector from left post to the right
  const Vector2<double> leftToRight((oppGoalModel.rightPost - oppGoalModel.leftPost).normalize());

  // a normal vector ponting from the goal towards the field
  Vector2<double> goalNormal(leftToRight);
  goalNormal.rotateRight();


  // the endpoints of our line are a shortened version of the goal line
  Vector2<double> leftEndpoint = oppGoalModel.leftPost + leftToRight * postOffset;
  Vector2<double> rightEndpoint = oppGoalModel.rightPost - leftToRight * postOffset;

  // this is the goalline we are shooting for
  Math::LineSegment goalLine(leftEndpoint, rightEndpoint);

  // project the point on the goal line
  Vector2<double> target = goalLine.projection(point);

  // this is the cos of the angle between the vectors (leftEndpoint-point) and (rightEndpoint-point)
  // simple linear algebra: <l-p,r-p>/(||l-p||*||r-p||)
  double goalAngleCos = (oppGoalModel.leftPost-point).normalize()*(oppGoalModel.rightPost-point).normalize();

  // assymetric quadratic scale
  // goalAngleCos = -1 => t = -goalLineOffsetBack
  // goalAngleCos =  1 => t =  goalLineOffsetFront;
  double c = (goalLineOffsetFront + goalLineOffsetBack)*0.5;
  double v = (goalLineOffsetFront - goalLineOffsetBack)*0.5;
  double t = goalAngleCos*(goalAngleCos*c + v);

  // move the target depending on the goal opening angle
  target = target + goalNormal.normalize(t);


  DEBUG_REQUEST("PotentialFieldProvider:goal_target",
    FIELD_DRAWING_CONTEXT;
    PEN("0000FF", 10);
    CIRCLE((getRobotPose()*leftEndpoint).x, (getRobotPose()*leftEndpoint).y, 100);
    CIRCLE((getRobotPose()*rightEndpoint).x, (getRobotPose()*rightEndpoint).y, 100);
  );
 
  return target;
}//end getGoalTargetCool
