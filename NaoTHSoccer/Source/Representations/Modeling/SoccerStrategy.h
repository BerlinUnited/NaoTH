/**
* @file SoccerStrategy.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* soccer strategy
*/

#ifndef SOCCERSTRATEGY_H
#define SOCCERSTRATEGY_H

#include "Tools/Math/Vector2.h"

class SoccerStrategy: public naoth::Printable
{
public:
  SoccerStrategy():timeToBall(std::numeric_limits<double>::max())
  {
  }

  virtual void print(std::ostream& stream) const
  {
    stream<<"formation = "<<formation<<"\n";
    stream<<"attackDirection = "<<attackDirection<<"\n";
    stream<<"timeToBall = "<<timeToBall<<"\n";
    stream<<"   timeForWalkstartTowardBall = "<<timeForWalkstartTowardBall<<"\n";
    stream<<"   distanceToPoint = "<<distanceToPoint<<"\n";
    stream<<"   timeForDistance = "<<timeForDistance<<"\n";
    stream<<"   timeForTurning = "<<timeForTurning<<"\n";
    stream<<"   timeForStandingUp = "<<timeForStandingUp<<"\n";
    stream<<"   timeForObstacle = "<<timeForObstacle<<"\n";
    stream<<"timeToTurn_afterBallAcquisition = "<<timeToTurn_afterBallAcquisition<<"\n";
    stream<<"==> totalTime = "<<totalTime<<"\n";
  }

  /** formation position */
  Vector2<double> formation;

  /** direction of the attack (based on potential field, see: PotentialFieldProvider) */
  Vector2<double> attackDirection; 

  /** the shorest time, in which the robot can reach the ball [ms] */
  double timeToBall;
  /** Explanation for the time **/
  double distanceToPoint;
  double timeForDistance;
  double timeForTurning;
  double timeForStandingUp;
  double timeForObstacle;
  double timeToTurn_afterBallAcquisition;
  double timeForWalkstartTowardBall;
  double totalTime;
};


#endif  /* SOCCERSTRATEGY_H */

