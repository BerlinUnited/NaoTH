/**
* @file SoccerStrategy.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* soccer strategy
*/

#ifndef SOCCERSTRATEGY_H
#define  SOCCERSTRATEGY_H

#include "Tools/Math/Vector2.h"

class SoccerStrategy: public Printable
{
public:
  SoccerStrategy():timeToBall(std::numeric_limits<double>::max())
  {
  }

  virtual void print(ostream& stream) const
  {
    stream<<"formation = "<<formation<<"\n";
    stream<<"attackDirection = "<<attackDirection<<"\n";
  }

  Vector2<double> formation; // formation position


  Vector2<double> attackDirection; // direction of the attack (based on potential field, see: PotentialFieldProvider)

  double timeToBall; // the shorest time that the robot reach the ball
};


#endif  /* SOCCERSTRATEGY_H */

