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
     path_routine(PathRoutine::NONE),
     path2018_routine(PathPlanner2018Routine::NONE),
     distance(0.0),
     yOffset(0.0), //TODO delete me
     direction(0.0),
     radius(0.0),
     stable(false),
     kick_executed(false)
   {}
  ~PathModel() {}

  enum class PathRoutine
  {
    NONE,
    GO_TO_BALL_FAST,
    GO_TO_BALL_SLOW,
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

  enum class PathPlanner2018Routine
  {
    NONE,
    MOVE_AROUND_BALL2,
    FORWARDKICK,
    SIDEKICK_LEFT,
    SIDEKICK_RIGHT,
    SIDESTEP
  };

  PathRoutine path_routine;
  PathPlanner2018Routine path2018_routine;

  // distance and yOffset parameters (set by XABSL)
  double distance;
  double yOffset; //TODO delete me

  // Move around ball
  double direction;
  double radius;
  bool stable;

  bool kick_executed;

  virtual void print(std::ostream& stream) const
  {
    std::string path_type;
    switch (path_routine)
    {
    case PathRoutine::NONE:
      path_type = "none";
      break;
    case PathRoutine::GO_TO_BALL_FAST:
      path_type = "go_to_ball_fast";
      break;
    case PathRoutine::GO_TO_BALL_SLOW:
      path_type = "go_to_ball_slow";
      break;
    case PathRoutine::APPROACH_BALL_LEFT:
      path_type = "approach_ball_left";
      break;
    case PathRoutine::APPROACH_BALL_RIGHT:
      path_type = "approach_ball_right";
      break;
    case PathRoutine::MOVE_AROUND_BALL:
      path_type = "move_around_ball";
      break;
    case PathRoutine::SHORT_KICK_LEFT:
      path_type = "short_kick_left";
      break;
    case PathRoutine::SHORT_KICK_RIGHT:
      path_type = "short_kick_right";
      break;
    case PathRoutine::LONG_KICK_LEFT:
      path_type = "long_kick_left";
      break;
    case PathRoutine::LONG_KICK_RIGHT:
      path_type = "long_kick_right";
      break;
    case PathRoutine::SIDEKICK_LEFT:
      path_type = "sidekick_left";
      break;
    case PathRoutine::SIDEKICK_RIGHT:
      path_type = "sidekick_right";
      break;
    }

    std::string path_type2018;
    switch (path2018_routine)
    {
    case PathPlanner2018Routine::NONE:
        path_type2018 = "none";
        break;
    case PathPlanner2018Routine::MOVE_AROUND_BALL2:
      path_type2018 = "move_around_ball";
      break;
    case PathPlanner2018Routine::SIDEKICK_LEFT:
      path_type2018 = "sidekick_left";
      break;
    case PathPlanner2018Routine::SIDEKICK_RIGHT:
      path_type2018 = "sidekick_right";
      break;
    case PathPlanner2018Routine::FORWARDKICK:
      path_type2018 = "forwardkick";
      break;
    case PathPlanner2018Routine::SIDESTEP:
      path_type2018 = "sidestep";
      break;
    }

    stream << "path_type = " << path_type << std::endl;
    stream << "path_type2018 = " << path_type2018 << std::endl;
    stream << "distance = " << distance << std::endl;
    stream << "yOffset = " << yOffset << std::endl;  //TODO delete me
    stream << "direction = " << direction << std::endl;
    stream << "radius = " << radius << std::endl;
  }
};



#endif /* _PathModel_H_ */
