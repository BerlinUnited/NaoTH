/**
 * @file PathModel.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#ifndef _PathModel_H_
#define _PathModel_H_

#include <Tools/DataStructures/Printable.h>

class PathModel : public naoth::Printable
{
public:
   PathModel() :
     path_routine(NONE),
     distance(0.0),
     yOffset(0.0),
     direction(0.0),
     radius(0.0),
     kick_executed(false)
   {}
  ~PathModel() {}

  enum PathRoutine
  {
    NONE,
    GO_TO_BALL,
    GO_TO_BALL_RIGHT,
    GO_TO_BALL_LEFT,
    APPROACH_BALL_RIGHT,
    APPROACH_BALL_LEFT,
    MOVE_AROUND_BALL,
    SHORT_KICK_RIGHT,
    SHORT_KICK_LEFT,
    LONG_KICK_RIGHT,
    LONG_KICK_LEFT,
    SIDEKICK_RIGHT,
    SIDEKICK_LEFT
  };

  PathRoutine path_routine;

  // distance and yOffset parameters (set by XABSL)
  double distance;
  double yOffset;
  
  // Move around ball
  double direction;
  double radius;

  bool kick_executed;

  virtual void print(std::ostream& stream) const
  {
    std::string path_type;
    switch (path_routine)
    {
    case NONE:
      path_type = "none";
      break;
    case GO_TO_BALL:
      path_type = "go_to_ball";
      break;
    case GO_TO_BALL_LEFT:
      path_type = "go_to_ball_left";
      break;
    case GO_TO_BALL_RIGHT:
      path_type = "go_to_ball_right";
      break;
    case APPROACH_BALL_LEFT:
      path_type = "approach_ball_left";
      break;
    case APPROACH_BALL_RIGHT:
      path_type = "approach_ball_right";
      break;
    case MOVE_AROUND_BALL:
      path_type = "move_around_ball";
      break;
    case SHORT_KICK_LEFT:
      path_type = "short_kick_left";
      break;
    case SHORT_KICK_RIGHT:
      path_type = "short_kick_right";
      break;
    case LONG_KICK_LEFT:
      path_type = "long_kick_left";
      break;
    case LONG_KICK_RIGHT:
      path_type = "long_kick_right";
      break;
    case SIDEKICK_LEFT:
      path_type = "sidekick_left";
      break;
    case SIDEKICK_RIGHT:
      path_type = "sidekick_right";
      break;
    }

    stream << "path_type = " << path_type << std::endl;
    stream << "distance = " << distance << std::endl;
    stream << "yOffset = " << yOffset << std::endl;
    stream << "direction = " << direction << std::endl;
    stream << "radius = " << radius << std::endl;
  }
};



#endif /* _PathModel_H_ */
