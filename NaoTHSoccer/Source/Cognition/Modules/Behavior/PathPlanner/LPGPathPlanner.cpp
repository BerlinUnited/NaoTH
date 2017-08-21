/**
 * @file LPGPathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class LPGPathPlanner
 */

#include "LPGPathPlanner.h"

static LPGHelper helper;

// --- Helper ---
LPGHelper::Cell LPGHelper::compute_cell(const Vector2d& coords) const
{
  LPGHelper::Cell the_cell;
  the_cell.r = static_cast<int>(std::floor(std::log(((std::sqrt(std::pow(coords.x, 2) + std::pow(coords.y, 2)) * (base - 1)) / minimal_cell) + 1) / log(base)));
  the_cell.a = static_cast<int>(std::floor((angular_part / (2*Math::pi)) * (std::atan2(coords.y, coords.x == 0 ? 1 : coords.x)) + 0.5));

  return the_cell;
}

double LPGHelper::distance_between_cells(const LPGHelper::Cell& a, const LPGHelper::Cell& b) const
{
  return distance(cell_middle(a), cell_middle(b));
}

void LPGHelper::set_obstacles(const std::vector<Vector3d>& obstacles)
{
  this->obstacles = obstacles;
}

double LPGHelper::obst_func(const LPGHelper::Cell& the_cell) const
{
  Vector2d obst;
  double sum = 0;
  double r_f, dist_to_obst_mid, obst_dist_to_mid, r_d, a, r, s;

  for (Vector3d obstacle : obstacles)
  {
    obst             = Vector2d(obstacle.x, obstacle.y);
    r_f              = obstacle.z;
    dist_to_obst_mid = distance(cell_middle(the_cell), obst);
    obst_dist_to_mid = distance(obst, Vector2d(0, 0));
    r_d              = obst_dist_to_mid / 10;

    a = r_f - r_d;       // cost of constant part
    r = r_f + r_d;       // radius of constant part
    s = parameter_s * r; // radius of linear decreasing part

    sum += std::max(std::min(1 - ((dist_to_obst_mid - r) / s), 1.0), 0.0) * a;
  }

  return sum;
}

Vector2d LPGHelper::cell_middle(const LPGHelper::Cell& the_cell) const
{
  double prd = (((std::pow(base, the_cell.r+0.5) - 1) * minimal_cell) / (base - 1));
  return Vector2d(std::cos(the_cell.a * (2*Math::pi/16)) * prd,
                  std::sin(the_cell.a * (2*Math::pi/16)) * prd);
}

double LPGHelper::distance(const Vector2d& a, const Vector2d& b) const
{
  return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

double LPGHelper::length(const Vector2d& coords) const
{
  return std::sqrt(std::pow(coords.x, 2) + std::pow(coords.y, 2));
}
// --- Helper ---

// --- A* State ---
LPGState::LPGState() : the_cell(LPGHelper::Cell()) {}
LPGState::LPGState(LPGHelper::Cell the_cell) : the_cell(the_cell) {}

LPGHelper::Cell LPGState::get_cell() const
{
  return the_cell;
}

float LPGState::GoalDistanceEstimate(LPGState& nodeGoal)
{
  return static_cast<float>(helper.distance_between_cells(the_cell, nodeGoal.the_cell));
}

bool LPGState::IsGoal(LPGState& nodeGoal)
{
  return IsSameState(nodeGoal);
}

bool LPGState::GetSuccessors(AStarSearch<LPGState> *astarsearch,
                             LPGState* parent_node)
{
  for (int i = -1; i <= 1; i++)
  {
    for (int k = -1; k <= 1; k++)
    {
      LPGState new_suc;
      if (parent_node)
      {
        // Continue when same node
        if (i == 0 && k == 0)
        {
          continue;
        }

        new_suc.the_cell.r = the_cell.r + i;
        new_suc.the_cell.a = the_cell.a + k;
      }
      else
      {
        new_suc.the_cell.r = i;
        new_suc.the_cell.a = k;
      }

      // Only add if new_suc isn't the same as its parent node
      // or if there isn't a parent node (first nodes that are generated)
      if ((!parent_node || !(parent_node->IsSameState(new_suc)))
          && std::abs(new_suc.the_cell.r) < 17
          && std::abs(new_suc.the_cell.a) < 17)
      {
        astarsearch->AddSuccessor(new_suc);
      }
    }
  }
  return true;
}

float LPGState::GetCost(LPGState& successor)
{
  return static_cast<float>(helper.distance_between_cells(the_cell, successor.the_cell) + helper.obst_func(successor.the_cell));
}

bool LPGState::IsSameState(LPGState& rhs)
{
  return (the_cell.r == rhs.the_cell.r && the_cell.a == rhs.the_cell.a);
}
// --- A* ---

// --- LPG PathPlanner ---
LPGPathPlanner::LPGPathPlanner() {}

LPGPathPlanner::~LPGPathPlanner() {}

Vector2d LPGPathPlanner::get_gait(const Vector2d& goal,
                                  const std::vector<Vector3d>& obstacles) const
{
  // Pass the obstacles to LPGHelper before computing waypoints
  helper.set_obstacles(obstacles);
  waypoints = compute_waypoints(goal);

  waypoint_coordinates = {};
  for (LPGHelper::Cell& cell : waypoints)
  {
    waypoint_coordinates.push_back(helper.cell_middle(cell));
  }

  if (waypoints.empty())
  {
    return Vector2d(0, 0);
  }

  // Compute the gait
  Vector2d gait;
  double distance      = 0;
  unsigned int counter = 0;
  do
  {
    if (counter == waypoints.size()) {break;}
    gait     = helper.cell_middle(waypoints[counter]);
    distance = helper.distance(Vector2d(0, 0), gait);
    counter++;
  } while (distance < 60);

  // Dirty: Let the FootStepPlanner actually limit the step to the maximum
  double max_steplength = 100;

  gait.x = gait.x / distance * max_steplength;
  gait.y = gait.y / distance * max_steplength;

  // If the maximized step would overstep the goal
  if (helper.length(gait) > helper.length(goal))
  {
    gait = goal;
  }

  return gait;
}

std::vector<Vector2d>& LPGPathPlanner::get_waypoint_coordinates() const
{
  return waypoint_coordinates;
}

std::vector<LPGHelper::Cell> LPGPathPlanner::compute_waypoints(const Vector2d& goal) const
{
  std::vector<LPGHelper::Cell> waypoints;

  LPGState startNode((LPGHelper::Cell()));
  LPGState endNode(helper.compute_cell(goal));

  AStarSearch<LPGState> astar;
  astar.SetStartAndGoalStates(startNode, endNode);

  unsigned int SearchState;

  do
  {
    SearchState = astar.SearchStep();

    // TODO: add debug request activating debugging

    // Don't search for too long
    // TODO: FIND A GOOD BREAK NUMBER
    // 100 seems to work good, 0.05 to 0.2 ms stopwatch for whole PathPlanner module (on my laptop!!! NEED ROBOT)
    int StepCount = astar.GetStepCount();
    if (StepCount == 100)
    {
      //astar.CancelSearch();
    }
  } while (SearchState == AStarSearch<LPGState>::SEARCH_STATE_SEARCHING);

  // Searching is done, check for the result
  if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_SUCCEEDED)
  {
    LPGState *node = astar.GetSolutionStart();
    waypoints.push_back(node->get_cell());

    for (;;)
    {
      node = astar.GetSolutionNext();
      if (!node) {break;}
      if (waypoints.back().r == node->get_cell().r && waypoints.back().a == node->get_cell().a) {continue;}
      waypoints.push_back(node->get_cell());
    }

    // Reset A*
    astar.FreeSolutionNodes();
  }
  // This is called when search fails or is canceled or out of memory
  // All nodes will be freed implicitly
  else if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_FAILED)
  {
    std::cout << "SEARCH FAILED" << std::endl;
    return waypoints; // empty vector
  }
  else if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_OUT_OF_MEMORY)
  {
    std::cout << "OUT OF MEMORY" << std::endl;
    return waypoints; //empty vector
  }

  return waypoints;
}
// --- LPG PathPlanner ---
