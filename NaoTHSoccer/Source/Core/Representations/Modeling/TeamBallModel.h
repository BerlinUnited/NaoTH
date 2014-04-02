/**
 * @file TeamBallModel.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef _TEAMBALLMODEL_H
#define  _TEAMBALLMODEL_H

class TeamBallModel
{
public:
  TeamBallModel()
      : time(0),
        goalieTime(0),
        strikerTime(0)
  {
  }

  // the global position of ball
  Vector2<double> positionOnField;

  // the position of ball in local coordination
  Vector2<double> position;

  // the time of message
  unsigned int time;

  Vector2<double> goaliePositionOnField;
  Vector2<double> goaliePosition;
  unsigned int goalieTime;

  Vector2<double> strikerPositionOnField;
  Vector2<double> strikerPosition;
  unsigned int strikerTime;

  virtual ~TeamBallModel()
  {

  }

};

//REPRESENTATION_INTERFACE(TeamBallModel);

#endif  /* _TEAMBALLMODEL_H */
