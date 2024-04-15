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
     path2018_routine(NONE),
     direction(0.0),
     radius(0.0),
     stable(false),
     kick_executed(false)
  {}
  ~PathModel() {}

  enum PathPlanner2018Routine
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

  inline static std::string getName(PathPlanner2018Routine id)
  {
    switch(id)
    {
      case NONE:                  return "none";
      case AVOID:                 return "avoid";
      case MOVE_AROUND_BALL_OLD:  return "move_around_ball_old";
      case MOVE_AROUND_BALL2:     return "move_around_ball";
      case FORWARDKICK:           return "forwardkick";
      case SIDEKICK_LEFT:         return "sidekick_left";
      case SIDEKICK_RIGHT:        return "sidekick_right";
      case SIDESTEP:              return "sidestep";
      default: ASSERT(false);
    }
  }

  PathPlanner2018Routine path2018_routine;

  // move around ball
  double direction;
  double radius;
  bool stable;

  bool kick_executed;
  Pose2D target_point;

  virtual void print(std::ostream& stream) const
  {  
    stream << "path_type2018 = " << getName(path2018_routine) << std::endl;
    stream << "direction = " << direction << std::endl;
    stream << "radius = " << radius << std::endl;
  }
};



#endif /* PathModel_H */
