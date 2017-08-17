/**
 * @file LPGPathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class LPGPathPlanner
 */

#include "LPGPathPlanner.h"

static LPGHelper helper;

// --- Helper ---
LPGHelper::Cell LPGHelper::compute_cell(const Vector2d &coords) const
{
  LPGHelper::Cell the_cell;
  the_cell.r = std::floor(std::log(((std::sqrt(std::pow(coords.x, 2) + std::pow(coords.y, 2)) * (base - 1)) / minimal_cell) + 1) / log(base));
  the_cell.a = std::floor((angular_part / (2*Math::pi)) * (std::atan2(coords.y, coords.x == 0 ? 1 : coords.x)) + 0.5);

  return the_cell;
}

double LPGHelper::distance_between_cells(const LPGHelper::Cell &a, const LPGHelper::Cell &b) const
{
  return distance(cell_middle(a), cell_middle(b));
}

void LPGHelper::set_obstacles(const std::vector<Vector3d> obstacles)
{
  this->obstacles = obstacles;
}

double LPGHelper::obst_func(const LPGHelper::Cell &the_cell) const
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

    a = (r_f - r_d) * 10;   // cost of constant part
    r = r_f + r_d;           // radius of constant part
    s = parameter_s * r;     // radius of linear decreasing part

    sum += std::max(std::min(1 - ((dist_to_obst_mid - r) / s), 1.0), 0.0) * a;
  }

  //std::cout << sum << std::endl;

  return sum;
}

Vector2d LPGHelper::cell_middle(const LPGHelper::Cell &the_cell) const
{
  double prd = (((std::pow(base, the_cell.r+0.5) - 1) * minimal_cell) / (base - 1));
  return Vector2d(std::cos(the_cell.a * (2*Math::pi/16)) * prd,
                  std::sin(the_cell.a * (2*Math::pi/16)) * prd);
}

double LPGHelper::distance(const Vector2d &a, const Vector2d &b) const
{
  return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

double LPGHelper::length(const Vector2d &coords) const
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

float LPGState::GoalDistanceEstimate(LPGState &nodeGoal)
{
  return helper.distance_between_cells(the_cell, nodeGoal.the_cell);
}

bool LPGState::IsGoal(LPGState &nodeGoal)
{
  return IsSameState(nodeGoal);
}

bool LPGState::GetSuccessors(AStarSearch<LPGState> *astarsearch,
                             LPGState *parent_node)
{
  LPGState new_suc;
  if (!parent_node)
  {
    new_suc.the_cell = LPGHelper::Cell(0, 0);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(0, 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(0, -1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(1, 0);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(1, 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(1, -1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(-1, 0);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(-1, 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(-1, -1);
    astarsearch->AddSuccessor(new_suc);
  }
  else
  {
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r + 0, parent_node->the_cell.a + 0);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r + 0, parent_node->the_cell.a + 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r + 0, parent_node->the_cell.a - 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r + 1, parent_node->the_cell.a + 0);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r + 1, parent_node->the_cell.a + 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r + 1, parent_node->the_cell.a - 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r - 1, parent_node->the_cell.a + 0);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r - 1, parent_node->the_cell.a + 1);
    astarsearch->AddSuccessor(new_suc);
    new_suc.the_cell = LPGHelper::Cell(parent_node->the_cell.r - 1, parent_node->the_cell.a - 1);
    astarsearch->AddSuccessor(new_suc);
  }

  /*for (int i = -1; i <= 1; i++)
  {
    for (int k = -1; k <= 1; k++)
    {
      LPGState new_suc;
      if (!parent_node)
      {
        new_suc.the_cell.r = i;
        new_suc.the_cell.a = k;
      }
      else
      {
        new_suc.the_cell.r = parent_node->the_cell.r + i;
        new_suc.the_cell.a = parent_node->the_cell.a + k;
      }

      //std::cout << new_suc.the_cell.r << " --- " << new_suc.the_cell.a << std::endl;

      astarsearch->AddSuccessor(new_suc);
    }
  }*/
  return true;  // TODO: add range check of grid
}

float LPGState::GetCost(LPGState &successor)
{
  return helper.distance_between_cells(the_cell, successor.the_cell) + helper.obst_func(successor.the_cell);
}

bool LPGState::IsSameState(LPGState &rhs)
{
  return (the_cell.r == rhs.the_cell.r && the_cell.a == rhs.the_cell.a);
}
// --- A* ---

// --- LPG PathPlanner ---
LPGPathPlanner::LPGPathPlanner()
{

}

LPGPathPlanner::~LPGPathPlanner()
{

}

Vector2d LPGPathPlanner::get_gait(Vector2d goal,
                                  std::vector<Vector3d> obstacles)
{
  // Pass the obstacles to LPGHelper before computing waypoints
  helper.set_obstacles(obstacles);
  std::vector<LPGHelper::Cell> waypoints = get_waypoints(goal);

  if (waypoints.empty())
  {
    std::cout << "empty" << std::endl;
    return Vector2d(0, 0);
  }
  else
  {
    for (LPGHelper::Cell cell : waypoints)
    {
      std::cout << cell.r << " -- " << cell.a << " -- " << helper.cell_middle(cell) << std::endl;
    }
    std::cout << std::endl;
  }

  // Compute the gait
  Vector2d gait         = helper.cell_middle(waypoints[1]);
  double distance       = helper.distance(Vector2d(0, 0), gait);
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

std::vector<LPGHelper::Cell> LPGPathPlanner::get_waypoints(Vector2d goal)
{
  std::vector<LPGHelper::Cell> waypoints;

  LPGState startNode((LPGHelper::Cell()));
  LPGState endNode(helper.compute_cell(goal));

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
      astar.CancelSearch();
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
      if ((waypoints.back().r == node->get_cell().r && waypoints.back().a == node->get_cell().a)) {continue;}
      waypoints.push_back(node->get_cell());
    }

    // Reset A*
    astar.FreeSolutionNodes();
  }
  // This is called when search fails or is canceled or out of memory
  // All nodes will be freed implicitly
  else if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_FAILED)
  {
    return waypoints; // empty vector
  }
  else if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_OUT_OF_MEMORY)
  {
    return waypoints; //empty vector
  }

  return waypoints;
}
// --- LPG PathPlanner ---
