/**
 * @file PathModel.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#ifndef PathModel_H
#define PathModel_H

#include <Tools/DataStructures/Printable.h>
#include "Tools/Math/Pose2D.h"

class PathModel : public naoth::Printable
{
public:
   PathModel() :
     path2018_routine(PathPlanner2018Routine::NONE),
     direction(0.0),
     radius(0.0),
     stable(false),
     kick_executed(false)
   {}
  ~PathModel() {}

  enum class PathPlanner2018Routine
  {
    NONE,
    AVOID,
    MOVE_AROUND_BALL_OLD,
    MOVE_AROUND_BALL2,
    FORWARDKICK,
    SIDEKICK_LEFT,
    SIDEKICK_RIGHT,
    SIDESTEP
  };
  PathPlanner2018Routine path2018_routine;

  // move around ball
  double direction;
  double radius;
  bool stable;

  bool kick_executed;
  Pose2D target_point;

  virtual void print(std::ostream& stream) const
  {  
    std::string path_type2018;
    switch (path2018_routine)
    {
    case PathPlanner2018Routine::NONE:
        path_type2018 = "none";
        break;
    case PathPlanner2018Routine::AVOID:
      path_type2018 = "avoid";
      break;
    case PathPlanner2018Routine::MOVE_AROUND_BALL_OLD:
      path_type2018 = "move_around_ball_old";
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

    stream << "path_type2018 = " << path_type2018 << std::endl;
    stream << "direction = " << direction << std::endl;
    stream << "radius = " << radius << std::endl;
  }
};



#endif /* PathModel_H */
