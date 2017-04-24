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
   pathType(none),
     goto_distance(0.0),
     goto_yOffset(0.0),
     move_around_ball_direction(0.0),
     move_around_ball_radius(0.0),
     kick_executed(false)
   {}
  ~PathModel() {}

  enum PathType
  {
    none,
    go_to_ball,
    go_to_ball_right,
    go_to_ball_left,
    go_to_ball_dynamic_right,
    go_to_ball_dynamic_left,
    move_around_ball,
    prepare_walk_back_approach,
    fast_forward_right,
    fast_forward_left,
    kick_with_foot_right,
    kick_with_foot_left,
    sidekick_left,
    sidekick_right
  };

  PathType pathType;

  // distance and yOffset parameters (set by XABSL)
  double goto_distance;
  double goto_yOffset;
  double ballRadius;
  
  // Move around ball
  double move_around_ball_direction;
  double move_around_ball_radius;

  bool kick_executed;

  virtual void print(std::ostream& stream) const
  {
    std::string path_type;
    switch (pathType)
    {
    case none:
      path_type = "none";
      break;
    case go_to_ball:
      path_type = "go_to_ball";
      break;
    case go_to_ball_right:
      path_type = "go_to_ball_right";
      break;
    case go_to_ball_left:
      path_type = "go_to_ball_left";
      break;
    case go_to_ball_dynamic_left:
      path_type = "go_to_ball_dynamic_left";
      break;
    case go_to_ball_dynamic_right:
      path_type = "go_to_ball_dynamic_right";
      break;
    case move_around_ball:
      path_type = "move_around_ball";
      break;
    case prepare_walk_back_approach:
      path_type = "prepare_walk_back_approach";
      break;
    case fast_forward_right:
      path_type = "fast_forward_right";
      break;
    case fast_forward_left:
      path_type = "fast_forward_left";
      break;
    case kick_with_foot_right:
      path_type = "kick_with_foot_right";
      break;
    case kick_with_foot_left:
      path_type = "kick_with_foot_left";
      break;
    case sidekick_left:
      path_type = "sidekick_left";
      break;
    case sidekick_right:
      path_type = "sidekick_right";
      break;
    }

    stream << "path_type = " << path_type << std::endl;
    stream << "goto_distance = " << goto_distance << std::endl;
    stream << "goto_yOffset = " << goto_yOffset << std::endl;
    stream << "move_around_ball_direction = " << move_around_ball_direction << std::endl;
    stream << "move_around_ball_radius = " << move_around_ball_radius << std::endl;
  }
};



#endif /* _PathModel_H_ */
