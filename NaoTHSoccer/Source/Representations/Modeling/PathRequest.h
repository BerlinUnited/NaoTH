/**
 * @file PathRequest.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#ifndef PATH_REQUEST_H
#define PATH_REQUEST_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/Math/Pose2D.h>
#include <Tools/Debug/NaoTHAssert.h>

class PathRequest : public naoth::Printable
{
public:
  PathRequest() :
     path2018_routine(PathID::NONE),
     direction(0.0),
     radius(0.0),
     stable(false)
  {}

  ~PathRequest() {}

  enum class PathID
  {
    NONE,
    AVOID,
    MOVE_AROUND_BALL_OLD,
    MOVE_AROUND_BALL2,
    FORWARDKICK,
    SIDEKICK_LEFT,
    SIDEKICK_RIGHT,
    SIDESTEP,
    APPROACH_DRIBBLE
  };

  inline static std::string getName(PathID id)
  {
    switch(id)
    {
      case PathID::NONE:                  return "none";
      case PathID::AVOID:                 return "avoid";
      case PathID::MOVE_AROUND_BALL_OLD:  return "move_around_ball_old";
      case PathID::MOVE_AROUND_BALL2:     return "move_around_ball";
      case PathID::FORWARDKICK:           return "forwardkick";
      case PathID::SIDEKICK_LEFT:         return "sidekick_left";
      case PathID::SIDEKICK_RIGHT:        return "sidekick_right";
      case PathID::SIDESTEP:              return "sidestep";
      case PathID::APPROACH_DRIBBLE:      return "approach_dribble";
      default: ASSERT(false);             return "unknown_path_id";
    }
  }

  PathID path2018_routine;

  // move around ball
  double direction;
  double radius;
  bool stable;

  Pose2D target_point;

  virtual void print(std::ostream& stream) const
  {  
    stream << "path_type2018 = " << getName(path2018_routine) << std::endl;
    stream << "direction = " << direction << std::endl;
    stream << "radius = " << radius << std::endl;

    stream << "target_point = [" 
      << target_point.translation.x << ", " 
      << target_point.translation.y << ", "
      << target_point.rotation << "] " << std::endl;
  }
};



#endif /* PATH_REQUEST_H */
