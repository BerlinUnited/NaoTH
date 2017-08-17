/**
 * @file PathPlanner.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class LPGPathPlanner
 */

#ifndef _LPGPathPlanner_H_
#define _LPGPathPlanner_H_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

#include "Tools/AStar/stlastar.h"

// Helper Class
class LPGHelper
{
public:

  struct Cell
  {
    // Constructor
    Cell() : r(0), a(0) {}

    unsigned int r;      // rings
    unsigned int a;      // angular
  };

  Cell compute_cell(const Vector2d &coords) const;
  double distance_between_cells(const Cell &a, const Cell &b) const;
  void set_obstacles(std::vector<Vector3d> obstacles);
  double obst_func(const Cell &the_cell) const;
  Vector2d cell_middle(const Cell &the_cell) const;
  double distance(const Vector2d &a, const Vector2d &b) const;
  double length(const Vector2d &coords) const;

private:

  std::vector<Vector3d> obstacles;

  // parameters for LPG
  double base         = 1.1789;
  double minimal_cell = 100.0;
  double angular_part = 16.0;

  // parameters for obstacle function
  double parameter_s  = 0.5;
};

// A* State Class
class LPGState
{
public:
  LPGState();
  LPGState(LPGHelper::Cell the_cell);

  LPGHelper::Cell get_cell() const;

  float GoalDistanceEstimate(LPGState &nodeGoal);
  bool IsGoal(LPGState &nodeGoal);
  bool GetSuccessors(AStarSearch<LPGState> *astarsearch,
                     LPGState *parent_node);
  float GetCost(LPGState &successor);
  bool IsSameState(LPGState &rhs);

private:

  LPGHelper::Cell the_cell;
};

// PathPlanner
class LPGPathPlanner
{
public:
  LPGPathPlanner();
  ~LPGPathPlanner();

  Vector2d get_gait(Vector2d goal, std::vector<Vector3d> obstacles);

private:

  AStarSearch<LPGState> astar;

  std::vector<LPGHelper::Cell> get_waypoints(Vector2d goal);

};

#endif /* _LPGPathPlanner_H_ */
