/**
 * @file PathPlanner.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class LPGPathPlanner
 */

#ifndef _LPGPathPlanner_H_
#define _LPGPathPlanner_H_

#include <memory>
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

#include "Tools/AStar/stlastar.h"


#include <Tools/Debug/DebugDrawings.h>

// Helper Class
class LPGHelper
{
public:

  struct Cell
  {
    // Constructor
    Cell() : r(0), a(0) {}
    Cell(int r, int a) : r(r), a(a) {}

    int r;      // rings
    int a;      // angular
  };

  Cell compute_cell(const Vector2d& coords) const;
  double distance_between_cells(const Cell& a, const Cell& b) const;
  void set_obstacles(const std::vector<Vector3d>& obstacles);
  double obst_func(const Cell& the_cell) const;
  Vector2d cell_middle(const Cell& the_cell) const;
  double distance(const Vector2d& a, const Vector2d& b) const;

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
  LPGState(std::shared_ptr<LPGHelper> helper);
  LPGState(std::shared_ptr<LPGHelper> helper, LPGHelper::Cell the_cell);

  const LPGHelper::Cell& get_cell() const {
    return the_cell;
  }

  float GoalDistanceEstimate(LPGState &nodeGoal);
  bool IsGoal(LPGState &nodeGoal);
  bool GetSuccessors(AStarSearch<LPGState> *astarsearch,
                     LPGState *parent_node);
  float GetCost(LPGState &successor);
  bool IsSameState(LPGState &rhs);

private:
  std::shared_ptr<LPGHelper> helper;
  LPGHelper::Cell the_cell;
 
};

// PathPlanner
class LPGPathPlanner
{
public:
  LPGPathPlanner();
  ~LPGPathPlanner();

  Vector2d get_gait(const Vector2d& goal, const std::vector<Vector3d>& obstacles, DrawingCanvas2D& canvas) const;
  
  std::vector<Vector2d>& get_waypoint_coordinates() const {
    return waypoint_coordinates;
  }

private:
  std::vector<LPGHelper::Cell> compute_waypoints(const Vector2d& goal, DrawingCanvas2D& canvas) const;

// internal state
private:

  mutable std::vector<LPGHelper::Cell> waypoints;
  mutable std::vector<Vector2d> waypoint_coordinates;

  mutable std::shared_ptr<LPGHelper> helper;
  mutable AStarSearch<LPGState> astar;
};

#endif /* _LPGPathPlanner_H_ */
