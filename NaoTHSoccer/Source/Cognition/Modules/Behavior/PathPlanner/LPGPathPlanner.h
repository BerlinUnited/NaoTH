/**
 * @file PathPlanner.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class LPGPathPlanner
 */

#ifndef _LPGPathPlanner_H_
#define _LPGPathPlanner_H_

#include "Tools/Math/Geometry.h"
#include "Tools/Math/Vector3.h"

#include "Tools/AStar/stlastar.h"

// A*
class LPGState
{
public:

  float GoalDistanceEstimate(LPGState &nodeGoal);
  bool IsGoal(LPGState &nodeGoal);
  bool GetSuccessors(AStarSearch<LPGState> *astarsearch,
                     LPGState *parent_node);
  float GetCost(LPGState &successor);
  bool IsSameState(LPGState &rhs);
};


class LPGPathPlanner
{
public:
  LPGPathPlanner();
  ~LPGPathPlanner();

  Vector2d get_gait(Vector2d goal, std::vector<Vector3d> obstacles) const;

private:

  // parameters for LPG
  double base         = 1.1789;
  double minimal_cell = 100.0;
  double angular_part = 16.0;

  // parameters for obstacle function
  double parameter_s  = 0.5;

  struct Cell
  {
    // Constructor
    Cell() : r(0), a(0) {}

    unsigned int r;      // rings
    unsigned int a;      // angular
  };

  struct Vector2cell
  {
    LPGPathPlanner::Cell cell1;
    LPGPathPlanner::Cell cell2;
  };

  AStarSearch<LPGState> astar;

  Cell compute_cell(const Vector2d &coords) const;
  Vector2d cell_middle(const Cell &the_cell) const;
  double distance_between_cells(const Cell &a, const Cell &b) const;

  std::vector<Vector2i> get_waypoints(Vector2d goal, std::vector<Vector3d> obstacles) const;

};

#endif /* _LPGPathPlanner_H_ */
