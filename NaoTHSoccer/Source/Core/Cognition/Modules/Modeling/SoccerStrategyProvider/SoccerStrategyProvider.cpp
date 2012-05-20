/**
* @file SoccerStrategyProvider.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* provides soccer strategy
*/

#include "SoccerStrategyProvider.h"
#include <PlatformInterface/Platform.h>
#include  <Tools/DataConversion.h>

SoccerStrategyProvider::FormationParameters::FormationParameters()
:num(0), behindBall(true), penaltyAreaAllowed(true)
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
  if (getPlayerInfo().gameData.playerNumber != theFormationParameters.num)
  {
    theFormationParameters = FormationParameters(getPlayerInfo().gameData.playerNumber);
  }
  
  getSoccerStrategy().formation = calculateForamtion();

  getSoccerStrategy().timeToBall = estimateTimeToBall();

  attackDirectionBuffer.add(getRawAttackDirection().attackDirection);
  double sumX = 0.0;
  double sumY = 0.0;
  for(int i=0; i < attackDirectionBuffer.getNumberOfEntries(); i++)
  {
    sumX += attackDirectionBuffer[i].x;
    sumY += attackDirectionBuffer[i].y;
  }
  getSoccerStrategy().attackDirection.x = sumX / (double) attackDirectionBuffer.getNumberOfEntries();
  getSoccerStrategy().attackDirection.y = sumY / (double) attackDirectionBuffer.getNumberOfEntries();
}

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
      double dFront = abs(p.x - penaltyFront);
      double dLeft = abs(p.y - penaltyLeft);
      double dRight = abs(p.y - penaltyRight);

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
}

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
}

double SoccerStrategyProvider::estimateTimeToPoint(const Vector2d& p) const
{
  // the simplest case: this speed should get from motion
  const double stepTime = 140; //ms
  const double speed = 90.0/stepTime; // mm/ms
  const double turnSpeed = Math::fromDegrees(30) / stepTime;


  double t = p.abs() / speed;
  t += abs(p.angle()) / turnSpeed;

  if ( getBodyState().fall_down_state != BodyState::upright )
    t += 3000; // stand up time

  if ( isSomeoneBetweenMeAndPoint(p) )
    t += 3000;

  return t;
}

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
      if (abs(ang) < Math::pi_2)
      {
        double blockRadius = abs(d * tan(ang));
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
      if (abs(ang) < Math::pi_2)
      {
        double blockRadius = abs(d * tan(ang));
        if (blockRadius < radiusOfRobot)
        {
          return true;
        }
      }
    }
  }

  // todo: consider goals
  return false;
}
