/**
* @file SoccerStrategy.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* soccer strategy
*/

#ifndef SOCCERSTRATEGY_H
#define SOCCERSTRATEGY_H

#include "Tools/Math/Vector2.h"

class SoccerStrategy: public Printable
{
public:
  SoccerStrategy():timeToBall(std::numeric_limits<double>::max())
  {
  }

  virtual void print(std::ostream& stream) const
  {
    stream<<"formation = "<<formation<<"\n";
    stream<<"attackDirection = "<<attackDirection<<"\n";
  }

  /** formation position */
  Vector2<double> formation;

  /** direction of the attack (based on potential field, see: PotentialFieldProvider) */
  Vector2<double> attackDirection; 

  /** the shorest time that the robot reach the ball */
  double timeToBall;
};


#endif  /* SOCCERSTRATEGY_H */

