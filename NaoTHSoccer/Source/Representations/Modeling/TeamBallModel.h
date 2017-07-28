/**
 * @file TeamBallModel.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _TEAMBALLMODEL_H
#define  _TEAMBALLMODEL_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"

class TeamBallModel  : public naoth::Printable
{
public:
  TeamBallModel()
      : time(0)
  {
  }

  // the global position of ball
  Vector2d positionOnField;

  // the position of ball in local coordination
  Vector2d position;
  
  // root mean squared error of the estimate [m]
  double rmse;

  // the time of message [ms]
  unsigned int time;

  virtual ~TeamBallModel()
  {}

  virtual void print(std::ostream& stream) const
  {
      stream << "global position: " << positionOnField.x << "/" << positionOnField.y << "\n"
             << "local position: " << position.x << "/" << position.y << "\n"
             << "root mean squared error: " << rmse << "\n"
             << "time: " << time << "\n"
             << std::endl;
  }

};

//REPRESENTATION_INTERFACE(TeamBallModel);

#endif  /* _TEAMBALLMODEL_H */
