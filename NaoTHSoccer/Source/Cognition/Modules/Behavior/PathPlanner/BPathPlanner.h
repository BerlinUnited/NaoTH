/**
 * @file BPathPlanner.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class BPathPlanner (BISEC)
 */

#ifndef _BPathPlanner_H_
#define _BPathPlanner_H_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

class BPathPlanner
{
public:
  BPathPlanner();
  ~BPathPlanner();

  Vector2d get_gait(const Vector2d& goal,
                    const std::vector<Vector3d>& obstacles) const;

private:

  struct Trajectory
  {
    Trajectory(const Vector2d& start, const Vector2d& end) : start(start), end(end) {}

    Vector2d start;
    Vector2d end;
  };

  struct Obstacle
  {
    Obstacle(const Vector3d& obstacle) : pos(Vector2d(obstacle.x, obstacle.y)), radius(obstacle.z) {}

    Vector2d pos;
    double radius;
  };

  std::vector<Obstacle>& obstacles;

  // Only used in hit_obstacle
  struct Vector4
  {
    Vector4(const Obstacle& obstacle, const double t) : obstacle(obstacle), t(t) {}

    Obstacle obstacle;
    double t;
  };

  double robot_radius = 300;

  std::vector<Obstacle> transform(const std::vector<Vector3d>& obstacles) const;
  double length(const Vector2d& vector) const;
  double length_of_trajectory(const Vector2d& start,
                              const Vector2d& end) const;
  int hit_obstacle(const Vector2d& start,
                   const Vector2d& end) const;
  bool still_colliding(const Vector2d* sub_target) const;
  std::vector<Trajectory> compute_path(const Vector2d& start,
                                       const Vector2d& end,
                                       unsigned int depth) const;
  Vector2d* compute_sub_target(const Vector2d& start, const Vector2d& end, const int sign) const;
  
};

#endif /* _BPathPlanner_H_ */
