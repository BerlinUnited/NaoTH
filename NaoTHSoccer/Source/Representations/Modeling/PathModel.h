/**
 * @file PathModel.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 */

#ifndef _PathModel_H_
#define _PathModel_H_

class PathModel
{
public:
   PathModel() {}
  ~PathModel() {}

  enum PathType
  {
    none,
    go_to_ball_right,
    go_to_ball_left,
    move_around_ball,
    fast_forward_right,
    fast_forward_left,
    kick_with_foot_right,
    kick_with_foot_left,
    sidekick_left,
    sidekick_right
  };

  PathType pathType = PathType::none;

  // distance and yOffset parameters (set by XABSL)
  double goto_distance;
  double goto_yOffset;
  
  // Move around ball
  double move_around_ball_direction;
  double move_around_ball_radius;
};

#endif /* _PathModel_H_ */
