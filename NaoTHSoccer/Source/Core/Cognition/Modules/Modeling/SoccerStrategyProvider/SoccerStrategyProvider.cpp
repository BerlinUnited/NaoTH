/**
* @file SoccerStrategyProvider.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* provides soccer strategy
*/

#include "SoccerStrategyProvider.h"

#include  <Tools/DataConversion.h>

SoccerStrategyProvider::FormationParameters::FormationParameters()
: ParameterList("SoccerStrategyProvider::FormationParameters")
{
  PARAMETER_REGISTER(num) = 0;
  PARAMETER_REGISTER(home.x) = 0;
  PARAMETER_REGISTER(home.y) = 0;
  PARAMETER_REGISTER(attr.x) = 0;
  PARAMETER_REGISTER(attr.y) = 0;
  PARAMETER_REGISTER(attr.z) = 0;
  PARAMETER_REGISTER(min.x) = 0;
  PARAMETER_REGISTER(min.y) = 0;
  PARAMETER_REGISTER(max.x) = 0;
  PARAMETER_REGISTER(max.y) = 0;
  PARAMETER_REGISTER(behindBall) = true;
  PARAMETER_REGISTER(penaltyAreaAllowed) = true;
}

SoccerStrategyProvider::SoccerStrategyProvider()
{
}

void SoccerStrategyProvider::execute()
{
  if (getPlayerInfo().gameData.playerNumber != theFormationParameters.num)
  {
    // TODO: make it configurable
    string path = "Config/formation/" + DataConversion::toStr(static_cast<unsigned int> (getPlayerInfo().gameData.playerNumber)) + ".cfg";
    ASSERT(false); // todo
    //VERIFY(theFormationParameters.loadFromConfigFile(path));
  }
  
  getSoccerStrategy().formation = calculateForamtion();

  getSoccerStrategy().timeToBall = estimateTimeToBall();
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
  // the simplest case: this speed should get from motion
  const double stepTime = 140; //ms
  const double speed = 95.0/stepTime; // mm/ms
  const double turnSpeed = Math::fromDegrees(45) / stepTime;


  double t = getBallModel().positionPreview.abs() / speed;
  t += abs(getBallModel().positionPreview.angle()) / turnSpeed;

  if ( getBodyState().fall_down_state != BodyState::upright )
    t += 3000; // stand up time

  return t;
}
