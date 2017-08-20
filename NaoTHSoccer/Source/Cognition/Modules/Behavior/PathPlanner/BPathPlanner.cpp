/**
 * @file BPathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class BPathPlanner (BISEC)
 */

#include "BPathPlanner.h"

BPathPlanner::BPathPlanner() {}
BPathPlanner::~BPathPlanner() {}

Vector2d BPathPlanner::get_gait(const Vector2d& goal,
                                const std::vector<Vector3d>& obstacles) const
{
  this->obstacles = transform(obstacles);

  compute_path(Vector2d(0, 0), goal);

  Vector2d direction   = trajectory[0].end - trajectory[0].start;
  double direction_len = length(direction);

  Vector2d gait = Vector2d(0, 0);
  if (direction_len > 0)
  {
    // Dirty: let the FootStepPlanner actually limit the requested step
    gait = direction / direction_len * 100;
  }

  return gait;
}

std::vector<BPathPlanner::Trajectory> BPathPlanner::get_trajectory() const
{
  return trajectory;
}

std::vector<BPathPlanner::Obstacle> BPathPlanner::transform(const std::vector<Vector3d>& obstacles) const
{
  std::vector<BPathPlanner::Obstacle> transformed;

  for (const Vector3d& obstacle : obstacles)
  {
    transformed.push_back(BPathPlanner::Obstacle(obstacle));
  }

  return transformed;
}

double BPathPlanner::length(const Vector2d& vector) const
{
  return vector.abs();
}

double BPathPlanner::length_of_trajectory(const Vector2d& start,
                                          const Vector2d& end) const
{
  return (end - start).abs();
}

BPathPlanner::Obstacle* BPathPlanner::hit_obstacle(const Vector2d& start,
                                                   const Vector2d& end) const
{
  std::vector<BPathPlanner::CollisionElement> hits;

  for (const Obstacle& obstacle : obstacles)
  {
    Vector2d start_to_obst = obstacle.pos - start;
    Vector2d line          = end - start;

    double line_mag       = std::pow(line.x, 2) + std::pow(line.y, 2);
    double start_dot_obst = start_to_obst.x * line.x + start_to_obst.y * line.y;

    double t = start_dot_obst / line_mag;

    Vector2d point;
    if (t < 0)
    {
      point = start;
      continue;
    }
    else if (t > 1)
    {
      point = end;
      continue;
    }
    else
    {
      point = start + line * t;
    }

    if (length_of_trajectory(point, obstacle.pos) <= obstacle.radius + robot_radius - 50)
    {
      hits.push_back(BPathPlanner::CollisionElement(obstacle, t));
    }
  }

  if (hits.size() > 0)
  {
    int index = 0;
    for (int i = 1; i < hits.size(); i++)
    {
      if (hits[i].t < hits[index].t)
      {
        index = i;
      }
    }

    return &hits[index].obstacle;
  }

  return nullptr;
}

bool BPathPlanner::still_colliding(const Vector2d* sub_target) const
{
  for (const BPathPlanner::Obstacle& obstacle : obstacles)
  {
    if (length_of_trajectory(*sub_target, obstacle.pos) <= obstacle.radius + robot_radius)
    {
      return true;
    }
  }

  return false;
}

bool BPathPlanner::compute_path(const Vector2d& start,
                                const Vector2d& end) const
{
  std::vector<BPathPlanner::Trajectory> trajectory {Trajectory(start, end)};

  std::vector<Vector2d> waypoints {start, end};

  unsigned int depth = 0;

  for (;;)
  {
    if (depth > 10)
    {
      std::cout << "Max. depth reached!" << std::endl;
      //this->trajectory = trajectory;
      return false;
    }

    depth++;

    // Used to save waypoints including new sub_targets temporarily
    // while going through all waypoints of original trajectory
    std::vector<Vector2d> tmp_waypoints = waypoints;

    // Counts the collisions on current trajectory - 0 means we are done
    unsigned int counter = 0;

    // Check for collision and compute waypoints with shortest sub_target
    for (unsigned int i = 0; i < waypoints.size() - 1; i++)
    {
      Vector2d* sub_target1 = compute_sub_target(waypoints[i], waypoints[i+1], +1);
      Vector2d* sub_target2 = compute_sub_target(waypoints[i], waypoints[i+1], -1);

      // Did a collision happen so that sub_targets were computed?
      if (sub_target1 && sub_target2)
      {
        Vector2d sub_target;
        std::vector<Vector2d> tmp_waypoints1;
        std::vector<Vector2d> tmp_waypoints2;

        // Insert new sub_targets to tmp_waypoints
        for (int k = 0; k < waypoints.size(); k++)
        {
          tmp_waypoints1.push_back(waypoints[k]);
          tmp_waypoints2.push_back(waypoints[k]);
          if (k == i)
          {
            tmp_waypoints1.push_back(*sub_target1);
            tmp_waypoints2.push_back(*sub_target2);
          }
        }

        // Compute length of the two competing trajectories
        double dist1 = 0;
        double dist2 = 0;
        for (int k = 0; k < tmp_waypoints1.size() - 1; k++)
        {
          dist1 += length_of_trajectory(tmp_waypoints1[k], tmp_waypoints1[k+1]);
          dist2 += length_of_trajectory(tmp_waypoints2[k], tmp_waypoints2[k+1]);
        }

        // Which trajectory is shorter?
        if (dist1 < dist2)
        {
          sub_target = *sub_target1;
        }
        else
        {
          sub_target = *sub_target2;
        }
        delete sub_target1;
        delete sub_target2;

        auto tmp_waypoints_it = tmp_waypoints.begin() + i + 1;
        tmp_waypoints.insert(tmp_waypoints_it, sub_target);

        // Count up the collision counter
        counter++;
      }
      else
      {
        // No collision between waypoints[i] and waypoints[i+1], continue with next two waypoints
        continue;
      }
    }

    // Produce new trajectory with final waypoints
    trajectory = {};
    waypoints = tmp_waypoints;
    for (int i = 0; i < waypoints.size() - 1; i++)
    {
      trajectory.push_back(Trajectory(waypoints[i], waypoints[i+1]));
    }

    // If there wasn't any collision, return new trajectory
    if (counter == 0)
    {
      std::cout << "Collision free path found!" << std::endl;
      this->trajectory = trajectory;
      return true;
    }
  }
}

Vector2d* BPathPlanner::compute_sub_target(const Vector2d& start,
                                           const Vector2d& end,
                                           const int sign) const
{
  Obstacle* collision = hit_obstacle(start, end);
  if (!collision) {return nullptr;}

  // Used to push out the sub_target more if it still collides
  double offset = 2;

  // Compute unit vector
  Vector2d target_vec = (end - start);
  target_vec          = target_vec / length(target_vec);

  // Compute orthogonal vectors
  Vector2d orth_vec1 = Vector2d(target_vec.y * -1, target_vec.x);
  Vector2d orth_vec2 = Vector2d(target_vec.y, target_vec.x * -1);

  // Compute first sub_target
  Vector2d* sub_target = new Vector2d();
  if (sign == 1)
  {
    *sub_target = orth_vec1 * (offset * robot_radius) + collision->pos;
  }
  else
  {
    *sub_target = orth_vec2 * (offset * robot_radius) + collision->pos;
  }

  // Check if computed sub_target still collides, push out further if it is the case
  while (still_colliding(sub_target))
  {
    offset += 1;
    if (sign == 1)
    {
      *sub_target = orth_vec1 * (offset * robot_radius) + collision->pos;
    }
    else
    {
      *sub_target = orth_vec2 * (offset * robot_radius) + collision->pos;
    }
  }

  return sub_target;
}
