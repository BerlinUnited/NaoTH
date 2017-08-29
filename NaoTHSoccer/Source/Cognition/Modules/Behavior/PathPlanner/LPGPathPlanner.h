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


class LPGGrid
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

public:
  Cell coords_to_cell(const Vector2d& coords) const;
  Vector2d cell_to_coords(const Cell& the_cell) const;
  double distance_between_cells(const Cell& a, const Cell& b) const;

private:
  // parameters for LPG
  double base         = 1.1789;
  double minimal_cell = 100.0;
  double angular_part = 16.0;


public:
  void set_obstacles(const std::vector<Vector3d>& obstacles) {
    this->obstacles = obstacles;
  }
  double obst_func(const Cell& the_cell) const;

private:
  // parameters for obstacle function
  double parameter_s  = 0.5;
  std::vector<Vector3d> obstacles;
};

// A* State Class
class LPGState
{
public:

  LPGState() : helper(nullptr), the_cell(LPGGrid::Cell()), is_origin(false) {}
  LPGState(std::shared_ptr<LPGGrid> helper) : helper(helper), the_cell(LPGGrid::Cell()), is_origin(false) {}
  LPGState(std::shared_ptr<LPGGrid> helper, LPGGrid::Cell the_cell) : helper(helper), the_cell(the_cell), is_origin(false) {}
  LPGState(std::shared_ptr<LPGGrid> helper, bool is_origin) : helper(helper), the_cell(LPGGrid::Cell()), is_origin(is_origin) {}

  const LPGGrid::Cell& get_cell() const {
    return the_cell;
  }

  float GoalDistanceEstimate(LPGState &nodeGoal);
  bool IsGoal(LPGState &nodeGoal);
  bool GetSuccessors(AStarSearch<LPGState> *astarsearch,
                     LPGState *parent_node);
  float GetCost(LPGState &successor);
  bool IsSameState(LPGState &rhs);

  bool isOrigin() { return is_origin; }

private:
  std::shared_ptr<LPGGrid> helper;
  LPGGrid::Cell the_cell;
 
  // HACK?
  // indicates the empty virtual cell at the center of the grid
  bool is_origin;
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
  std::vector<LPGGrid::Cell> compute_waypoints(const Vector2d& goal, DrawingCanvas2D& canvas) const;

// internal state
private:

  mutable std::vector<LPGGrid::Cell> waypoints;
  mutable std::vector<Vector2d> waypoint_coordinates;

  mutable std::shared_ptr<LPGGrid> helper;
  mutable AStarSearch<LPGState> astar;
};

#endif /* _LPGPathPlanner_H_ */
