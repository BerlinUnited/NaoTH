/**
 * @file LPGPathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class LPGPathPlanner
 */

#include "LPGPathPlanner.h"

// --- A* ---
float LPGState::GoalDistanceEstimate(LPGState &nodeGoal)
{

  return 0;
}

bool LPGState::IsGoal(LPGState &nodeGoal)
{
  return IsSameState(nodeGoal);
}

bool LPGState::GetSuccessors(AStarSearch<LPGState> *astarsearch,
                             LPGState *parent_node)
{

  return false;
}

float LPGState::GetCost(LPGState &successor)
{

  return 0;
}

bool LPGState::IsSameState(LPGState &rhs)
{

  return false;
}
// --- A* ---


// --- LPG PathPlanner ---
LPGPathPlanner::LPGPathPlanner()
{

}

LPGPathPlanner::~LPGPathPlanner()
{

}

LPGPathPlanner::Cell LPGPathPlanner::compute_cell(const Vector2d &coords) const
{
  LPGPathPlanner::Cell the_cell;
  the_cell.r = std::floor(std::log(((std::sqrt(std::pow(coords.x, 2) + std::pow(coords.y, 2)) * (base - 1)) / minimal_cell) + 1) / log(base));
  the_cell.a = std::floor((angular_part / (2*Math::pi)) * (std::atan2(coords.y, coords.x == 0 ? 1 : coords.x)) + 0.5);

  return the_cell;
}

Vector2d LPGPathPlanner::cell_middle(const Cell &the_cell) const
{
  double prd = (((std::pow(base, the_cell.r+0.5) - 1) * minimal_cell) / (base - 1));
  return Vector2d(std::cos(the_cell.a * (2*Math::pi/16)) * prd,
                  std::sin(the_cell.a * (2*Math::pi/16)) * prd);
}

double LPGPathPlanner::distance_between_cells(const LPGPathPlanner::Cell &a,
                                              const LPGPathPlanner::Cell &b) const
{
  Vector2d first  = cell_middle(a);
  Vector2d second = cell_middle(b);

  return std::sqrt(std::pow(first.x - second.x, 2)) + std::pow(first.y - second.y, 2);
}

Vector2d LPGPathPlanner::get_gait(Vector2d goal,
                                  std::vector<Vector3d> obstacles) const
{
  return Vector2d(0, 0);
}

std::vector<Vector2i> LPGPathPlanner::get_waypoints(Vector2d goal,
                                                    std::vector<Vector3d> obstacles) const
{
  std::vector<Vector2i> waypoints;

  LPGPathPlanner::Cell start = compute_cell(goal);
  LPGPathPlanner::Cell end   = LPGPathPlanner::Cell();


  return waypoints;
}
// --- LPG PathPlanner ---
