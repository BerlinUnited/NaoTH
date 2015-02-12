/**
* @file SoccerStrategyProvider.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* provides soccer strategy
*/

#include "SoccerStrategyProvider.h"
#include <PlatformInterface/Platform.h>
#include  <Tools/DataConversion.h>

using namespace std;

SoccerStrategyProvider::FormationParameters::FormationParameters()
  : num(0), behindBall(true), penaltyAreaAllowed(true)
{

}

SoccerStrategyProvider::FormationParameters::FormationParameters(unsigned int playerNumber)
{
  const naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  string group = "Formation";
  ASSERT( config.hasGroup(group) );

  num = playerNumber;
  string p = "player"+DataConversion::toStr(playerNumber)+".";
  home.x = config.getDouble(group, p+"home.x");
  home.y = config.getDouble(group, p+"home.y");
  attr.x = config.getDouble(group, p+"attr.x");
  attr.y = config.getDouble(group, p+"attr.y");
  attr.z = config.getDouble(group, p+"attr.z");
  min.x = config.getDouble(group, p+"min.x");
  min.y = config.getDouble(group, p+"min.y");
  max.x = config.getDouble(group, p+"max.x");
  max.y = config.getDouble(group, p+"max.y");
  behindBall = config.getBool(group, p+"behindBall");
  penaltyAreaAllowed = config.getBool(group, p+"penaltyAreaAllowed");
}

SoccerStrategyProvider::SoccerStrategyProvider()
{
}

void SoccerStrategyProvider::execute()
{
  /*
	if (getPlayerInfo().gameData.playerNumber != theFormationParameters.num)
  {
    theFormationParameters = FormationParameters(getPlayerInfo().gameData.playerNumber);
  }
  */
  //
  //getSoccerStrategy().formation = calculateForamtion();


  double timeToBallEstimation = estimateTimeToBall();

  getSoccerStrategy().timeToBall = timeToBallEstimation;
  PLOT("SoccerStrategyProvider:timeToBall", getSoccerStrategy().timeToBall);


  // Heinrich: this is actually a hack, if the attack direction jumps, 
  //           then the reason is somwhere else
  // smooth the attack direction
  attackDirectionBuffer.add(getRawAttackDirection().attackDirection);
  getSoccerStrategy().attackDirection = attackDirectionBuffer.getAverage();

  //NEW:
  //getSoccerStrategy().timeToTurn_afterBallAcquisition = estimateTimeFromBallTo_GoalDir(getSoccerStrategy().attackDirection, 
  //                                                                              getBallModel().futurePosition[BALLMODEL_MAX_FUTURE_SECONDS]);
  //getSoccerStrategy().totalTime = getSoccerStrategy().timeToBall + getSoccerStrategy().timeToTurn_afterBallAcquisition;
}//end execute


Vector2<double> SoccerStrategyProvider::calculateForamtion() const
{
  Vector2<double> p;

  // calculate the formation according to ball position on the field
  Vector2 <double> ballPositionField = getRobotPose() * getBallModel().position;
  p.x = theFormationParameters.home.x + theFormationParameters.attr.x * ballPositionField.x;
  p.y = theFormationParameters.home.y + theFormationParameters.attr.y * ballPositionField.y + theFormationParameters.attr.z * ballPositionField.x;

  if ( theFormationParameters.behindBall )
  {
    p.x = min(p.x, ballPositionField.x);
  }

  double distToBall = (p-ballPositionField).abs();
  const double minDistToBall = 1000;
  if ( distToBall < minDistToBall )
  {
    p = ballPositionField + (p-ballPositionField).normalize(minDistToBall);
  }

  p.x = Math::clamp(p.x, theFormationParameters.min.x, theFormationParameters.max.x);
  p.y = Math::clamp(p.y, theFormationParameters.min.y, theFormationParameters.max.y);

  const double border = 500;
  const double penaltyFront = getFieldInfo().xPosOwnPenaltyArea + border;
  const double penaltyLeft = getFieldInfo().yPosLeftPenaltyArea + border;
  const double penaltyRight = getFieldInfo().yPosRightPenaltyArea - border;
  if (!theFormationParameters.penaltyAreaAllowed)
  {
    // move the position outside of penalty area
    if ( p.x < penaltyFront
      && p.y < penaltyLeft
      && p.y > penaltyRight )
    {
      double dFront = fabs(p.x - penaltyFront);
      double dLeft = fabs(p.y - penaltyLeft);
      double dRight = fabs(p.y - penaltyRight);

      if ( dLeft < dRight && dLeft < dFront )
      {
        p.y = penaltyLeft;
      }
      else if ( dRight < dLeft && dRight < dFront )
      {
        p.y = penaltyRight;
      }
      else
      {
        p.x = penaltyFront;
      }
    }
  }

  return p;
}//end calculateForamtion

double SoccerStrategyProvider::estimateTimeToBall() const
{
  for( int i=0; i<BALLMODEL_MAX_FUTURE_SECONDS; i++)
  {
    double t = estimateTimeToPoint(getBallModel().futurePosition[i]);
    if ( t < i )
    {
      return t; // we can catch the ball
    }
  }
  return estimateTimeToPoint(getBallModel().futurePosition[BALLMODEL_MAX_FUTURE_SECONDS]);
}//end estimateTimeToBall


double SoccerStrategyProvider::estimateTimeToPoint(const Vector2d& p) const
{
  // the simplest case: this speed should get from motion
  const double stepTime = 140; //ms
  const double speed = 90.0/stepTime; // mm/ms
  const double turnSpeed = Math::fromDegrees(30) / stepTime;


  double t = p.abs() / speed;
  t += fabs(p.angle()) / turnSpeed;

  if ( getBodyState().fall_down_state != BodyState::upright )
    t += 3000; // stand up time

  if ( isSomeoneBetweenMeAndPoint(p) )
    t += 3000;

  return t;
}//end estimateTimeToPoint

bool SoccerStrategyProvider::isSomeoneBetweenMeAndPoint(const Vector2d& p) const
{
  const double dist = p.abs();
  const double dir = p.angle();

  unsigned int myNum = getPlayerInfo().gameData.playerNumber;
  const PlayersModel& players = getPlayersModel();
  unsigned int currentFrameNumber = getFrameInfo().getFrameNumber();

  const double radiusOfRobot = 550;
  // teammates
  for( vector<PlayersModel::Player>::const_iterator iter=players.teammates.begin();
    iter!=players.teammates.end(); ++iter)
  {
    if ( iter->number == myNum ) continue;

    if ( iter->frameInfoWhenWasSeen.getFrameNumber() != currentFrameNumber ) continue;

    double d = iter->pose.translation.abs();
    if ( d < dist )
    {
      double ang = Math::normalizeAngle(iter->pose.translation.angle() - dir);
      if (fabs(ang) < Math::pi_2)
      {
        double blockRadius = fabs(d * tan(ang));
        if (blockRadius < radiusOfRobot)
        {
          return true;
        }
      }
    }
  }

  // opponents
  for( vector<PlayersModel::Player>::const_iterator iter=players.opponents.begin();
    iter!=players.opponents.end(); ++iter)
  {
    if ( iter->frameInfoWhenWasSeen.getFrameNumber() != currentFrameNumber ) continue;

    double d = iter->pose.translation.abs();
    if (d < dist)
    {
      double ang = Math::normalizeAngle(iter->pose.translation.angle() - dir);
      if (fabs(ang) < Math::pi_2)
      {
        double blockRadius = fabs(d * tan(ang));
        if (blockRadius < radiusOfRobot)
        {
          return true;
        }
      }
    }
  }

  // todo: consider goals
  return false;
}//end isSomeoneBetweenMeAndPoint

//Everything beyond this point is NEW:
/*
double SoccerStrategyProvider::estimateTimeToBall() const {
  return estimateTimeToPoint(getBallModel().positionPreview);
}//end estimateTimeToBall

double SoccerStrategyProvider::estimateTimeToPoint(const Vector2d& p, const double& offensiveness) const
{
  const double t_step = 30; // Time needed for one step [s]
  const double dist_step = 30; // Distance covered by a step [mm]
  const double rot_step = 30; // Rotation covered by step [rad]

  double dist_ball = p.abs();
  double rot_ball = p.angle();

  const double time_for_distance = t_step * dist_ball/dist_step;
  const double time_for_rotation = t_step * rot_ball/rot_step;
  const double time_to_destination = time_for_distance + time_for_rotation;

  double attackDir = getSoccerStrategy().attackDirection.angle();
  const double rot_around_ball_after_havingObtainedBall = (rot_ball + attackDir)/(Math::pi2) - floor((rot_ball + attackDir)/(Math::pi2)); //Wrong - Should be revised
  const double time_to_moveAroundBall = t_step * rot_around_ball_after_havingObtainedBall/rot_step;
  
  const double t_total = time_to_destination + offensiveness * time_to_moveAroundBall;

  // Fill in the values in the representation
  getSoccerStrategy().timeForDistance = time_for_distance;
  getSoccerStrategy().timeForTurning = time_for_rotation;
  getSoccerStrategy().timeToTurn_afterBallAcquisition = time_to_moveAroundBall;
  getSoccerStrategy().totalTime = t_total;

  return t_total;
}

/* ALTERNATIVE estimateTimeToPoint
double SoccerStrategyProvider::estimateTimeToPoint(const Vector2d& p) const
{
  FrameInfo fi = getFrameInfo();
  MotionStatus mtst = getMotionStatus();  

  if (robotIsStuck && (fi.getTime() - timeToBall_reviewIntervall < lastTimeEstimation)) {
    double t;
    if (mtst.currentMotion == motion::walk) {
      //Reduce the estimation by 10ms at each step, if robot is walking
      t = last_timeToBallEstimation - (fi.getTime() - lastTimeEstimation); 
    }
    else {
      //If robot is not walking, don't change the estimation
      t = last_timeToBallEstimation; 
    }
    return t;
  }

  //Walking-Parameters (Should be received from Motion):
  const double stepTime = 250.00; // ms
  const double stepSize = 45.00; // mm
  const double turn_stepAngle = Math::fromDegrees(15.00); // radian
  const double standUpTime = 3000; // ms

  const int obstacle_updateInterval = 1000; //How often should the time for passing obstacles be updated

  //Calculate Speed
  const double speed = stepSize/stepTime; // mm/ms
  const double turnSpeed = turn_stepAngle/stepTime; // radian/ms

  double time_for_distance = p.abs() / speed;
  double time_for_turning = fabs(p.angle()) / turnSpeed;
  double time_for_walkStart_toward_ball = 500; //Time needed to start walking forward
 
  double time_for_standing_up = 0;
  double time_for_obstacle = 0;

  double t = time_for_distance + time_for_turning;

  if ( getBodyState().fall_down_state != BodyState::upright ) {
    t += standUpTime; // ms 
    time_for_standing_up = standUpTime;
  }
  
  if (fi.getTime() - lastObstacleUpdate >= min((int)lastTimeToGetPastObstacle, obstacle_updateInterval)) { //If enough time has passed since the last obstacle update
    time_for_obstacle = timeToGetPastObstacle(p); //Estimated time to avoid obstacle (if there are any obstacles), updateInterval = 1000ms
    lastObstacleUpdate = fi.getTime();
    timeToGetPastObstacle = time_for_obstacle;
  }
  else { //If an obstacle update has been performed recently
    time_for_obstacle = lastObstacleUpdate + lastTimeToGetPastObstacle - fi.getTime();
  }
  t += time_for_obstacle;   

  if (mtst.currentMotion == motion::walk) { //If the robot is currently walking
    if (-10 < Math::toDegrees(fabs(p.angle())) && Math::toDegrees(fabs(p.angle())) < 10) { //If the robot is currently facing the target
      time_for_walkStart_toward_ball = 0;
    }
    else { //If robot has to turn around to face the target
      time_for_walkStart_toward_ball += 500; //Turning around involves a walk-stop and a walk-start
    }
  }
  t += time_for_walkStart_toward_ball;
  
  getSoccerStrategy().distanceToPoint = p.abs();
  getSoccerStrategy().timeForDistance = time_for_distance;
  getSoccerStrategy().timeForTurning = time_for_turning;
  getSoccerStrategy().timeForStandingUp = time_for_standing_up;
  getSoccerStrategy().timeForObstacle = time_for_obstacle;
  getSoccerStrategy().timeForWalkstartTowardBall = time_for_walkStart_toward_ball;

  if (fi.getTime() - timeToBall_reviewIntervall > lastTimeEstimation) { //If enough time after last review has passed
    if (((last_timeToBallEstimation - t) > 0.4*last_timeToBallEstimation)) { 
      // If the error of the last estimation is larger than 40% (not considering negative errors)
      robotIsStuck = true;
      // Increase the estimated time for this robot, so other robots have a chance at becoming the striker
      t *= 2;
    }
    last_timeToBallEstimation = t;
    lastTimeEstimation = getFrameInfo().getTime();
  }
  return t;
}//end estimateTimeToPoint
*/
/*
double SoccerStrategyProvider::estimateTimeFromBallTo_GoalDir(const Vector2d& attackDir, const Vector2d& p) const
{
  //Walking-Parameters (Should be received from Motion):
  const double stepTime = 250.00; // ms
  const double turn_stepAngle = Math::fromDegrees(15.00); // radian

  //Calculate Speed
  const double turnSpeed = turn_stepAngle/stepTime; // radian/ms

  double ballAngle = p.angle();
  double attackAngle = fabs(ballAngle - attackDir.angle());
  double t = attackAngle/turnSpeed;

  return t;
}

int SoccerStrategyProvider::timeToGetPastObstacle(const Vector2d& pos) const
{
  int t = 0;
  ObstacleModel om = getObstacleModel();

  double left = 0;
  double right = 0;
  if (om.blockedTime > 100) {
    if (om.leftDistance-100 < pos.x) { //If left sensor is blocked
      left = 1;
      if (om.rightDistance-100 < pos.x) { //If right sensor is blocked
        right = 1;
        t = 3500; //Assumption:  Both sensors are being blocked by the SAME object
      }
      else { //If right sensor is NOT blocked
        if (pos.y > 100) { //If obstacle is on the LEFT side + ball is on the LEFT side
          t = 4500;
        }
        else {
          if (pos.y < -100) { //If obstacle is on the LEFT side + ball is on the RIGHT side
            t = 1500;
          }
          else { //If obstacle is on the LEFT side + ball is IN FRONT of player
            t = 2500;
          }
        }
      }
    }
    else { //If left sensor is NOT blocked
      if (om.rightDistance < pos.x) { //If right sensor is blocked
        right = 1;
        if (pos.y < -100) { //If obstacle is on the RIGHT side + ball is on the RIGHT side
          t = 4500;
        } else if (pos.y > 100) { //If obstacle is on the RIGHT side + ball is on the LEFT side
          t = 1500;
        } else { //If obstacle is on the RIGHT side + ball is IN FRONT of player
          t = 2500;
        }
      }
    }
  }

  return t;
}


bool SoccerStrategyProvider::isSomeoneBetweenMeAndPoint(const Vector2d& p) const
{
  Vector2d selfPos(0, 0);
  Vector2d destinationPos(p);

  return isSomethingBetweenPoints(selfPos, destinationPos);
}//end isSomeoneBetweenMeAndPoint


bool SoccerStrategyProvider::isSomethingBetweenPoints(const Vector2d& p1, Vector2d& p2) const
{
  const Vector2d p = p2 - p1;
  const double dist = p.abs();
  const double dir = p.angle();

  unsigned int myNum = getPlayerInfo().gameData.playerNumber;
  const PlayersModel& players = getPlayersModel();
  unsigned int currentFrameNumber = getFrameInfo().getFrameNumber();

  const double radiusOfRobot = 550;
  // teammates
  for( vector<PlayersModel::Player>::const_iterator iter=players.teammates.begin();
    iter!=players.teammates.end(); ++iter)
  {
    if ( iter->number == myNum ) continue;

    if ( iter->frameInfoWhenWasSeen.getFrameNumber() != currentFrameNumber ) continue;

    double d = iter->pose.translation.abs();
    if ( d < dist )
    {
      double ang = Math::normalizeAngle(iter->pose.translation.angle() - dir);
      if (fabs(ang) < Math::pi_2)
      {
        double blockRadius = fabs(d * tan(ang));
        if (blockRadius < radiusOfRobot)
        {
          return true;
        }
      }
    }
  }

  // opponents
  for( vector<PlayersModel::Player>::const_iterator iter=players.opponents.begin();
    iter!=players.opponents.end(); ++iter)
  {
    if ( iter->frameInfoWhenWasSeen.getFrameNumber() != currentFrameNumber ) continue;

    double d = iter->pose.translation.abs();
    if (d < dist)
    {
      double ang = Math::normalizeAngle(iter->pose.translation.angle() - dir);
      if (fabs(ang) < Math::pi_2)
      {
        double blockRadius = fabs(d * tan(ang));
        if (blockRadius < radiusOfRobot)
        {
          return true;
        }
      }
    }
  }

  // todo: consider goals
  return false;
}//end isSomethingBetweenPoints
*/