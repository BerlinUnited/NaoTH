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
  TeamBallModel() {}

  // the global position of ball
  Vector2d positionOnField;

  // the position of ball in local coordination
  Vector2d position;
  
  // root mean squared error of the estimate [m]
  double rmse = 0.0;

  // the time of message [ms] (last received teamcomm message)
  unsigned int time = 0;

  // validity of the model: true => the teamball-locator was able to determine a teamball
  bool valid = false;

  virtual ~TeamBallModel()
  {}

  virtual void print(std::ostream& stream) const
  {
      stream << "valid: " << (valid ? "true" : "false") << "\n"
             << "global position: " << positionOnField.x << "/" << positionOnField.y << "\n"
             << "local position: " << position.x << "/" << position.y << "\n"
             << "root mean squared error: " << rmse << "\n"
             << "time: " << time << "\n"
             << std::endl;
  }

};

//REPRESENTATION_INTERFACE(TeamBallModel);

#endif  /* _TEAMBALLMODEL_H */
