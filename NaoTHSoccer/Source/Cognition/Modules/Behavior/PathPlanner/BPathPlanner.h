/**
 * @file BPathPlanner.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class BPathPlanner (BISEC)
 */

#ifndef _BPathPlanner_H_
#define _BPathPlanner_H_

#include <iostream>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

class BPathPlanner
{
public:
  BPathPlanner();
  ~BPathPlanner();

  struct Trajectory
  {
    Trajectory(const Vector2d& start, const Vector2d& end) : start(start), end(end) {}

    Vector2d start;
    Vector2d end;
  };

  Vector2d get_gait(const Vector2d& goal,
                    const std::vector<Vector3d>& obstacles) const;
  std::vector<Trajectory> get_trajectory() const;

private:

  mutable std::vector<Trajectory> trajectory;

  struct Obstacle
  {
    Obstacle(const Vector3d& obstacle) : pos(Vector2d(obstacle.x, obstacle.y)), radius(obstacle.z) {}

    Vector2d pos;
    double radius;
  };

  mutable std::vector<Obstacle> obstacles;

  // Only used in hit_obstacle
  struct CollisionElement
  {
    CollisionElement(const Obstacle& obstacle, const double t) : obstacle(obstacle), t(t) {}

    Obstacle obstacle;
    double t;
  };

  double robot_radius = 300;

  std::vector<Obstacle> transform(const std::vector<Vector3d>& obstacles) const;
  double length(const Vector2d& vector) const;
  double length_of_trajectory(const Vector2d& start,
                              const Vector2d& end) const;
  Obstacle* hit_obstacle(const Vector2d& start,
                         const Vector2d& end) const;
  bool still_colliding(const Vector2d* sub_target) const;
  bool compute_path(const Vector2d& start,
                    const Vector2d& end) const;
  std::vector<Trajectory> compute_path_alt(const Vector2d& start,
                                           const Vector2d& end,
                                           const int sign) const;
  std::vector<Trajectory> compare_paths(const std::vector<Trajectory>& trajectory1,
                                        const std::vector<Trajectory>& trajectory2) const;
  Vector2d* compute_sub_target(const Vector2d& start, const Vector2d& end, const int sign) const;
  
};

#endif /* _BPathPlanner_H_ */
