/**
 * @file LPGPathPlanner.cpp
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Implementation of class LPGPathPlanner
 */

#include "LPGPathPlanner.h"
#include <Tools/Debug/NaoTHAssert.h>

// --- Helper ---
LPGGrid::Cell LPGGrid::coords_to_cell(const Vector2d& coords) const
{
  LPGGrid::Cell the_cell;
  // NOTE: the_cell.r is allways >= 0
  the_cell.r = static_cast<int>(std::log(((coords.abs() * (base - 1.0)) / minimal_cell) + 1.0) / log(base));
  the_cell.a = static_cast<int>( angular_part * (coords.angle() + Math::pi) / Math::pi2 + 0.5);

  // normalize rotation
  if(the_cell.a < 0) {
    the_cell.a = 16 + the_cell.a;
  } else if(the_cell.a > 15) {
    the_cell.a = the_cell.a - 16;
  }

  return the_cell;
}

// TODO: rename to cell_coordinates?
Vector2d LPGGrid::cell_to_coords(const LPGGrid::Cell& the_cell) const
{
  double prd = (((std::pow(base, the_cell.r+0.5) - 1) * minimal_cell) / (base - 1));
  return Vector2d(std::cos(the_cell.a * (Math::pi2/angular_part)) * prd,
                  std::sin(the_cell.a * (Math::pi2/angular_part)) * prd);
}

double LPGGrid::distance_between_cells(const LPGGrid::Cell& a, const LPGGrid::Cell& b) const
{
  return (cell_to_coords(a) - cell_to_coords(b)).abs();
}

double LPGGrid::obst_func(const LPGGrid::Cell& the_cell) const
{
  Vector2d obst;
  double sum = 0;
  double r_f, dist_to_obst_mid, obst_dist_to_mid, r_d, a, r, s;

  for (const Vector3d& obstacle : obstacles)
  {
    obst             = Vector2d(obstacle.x, obstacle.y);
    r_f              = obstacle.z;
    dist_to_obst_mid = (cell_to_coords(the_cell) - obst).abs(); // distance(cell_middle(the_cell), obst);
    obst_dist_to_mid = obst.abs(); // distance(obst, Vector2d(0, 0)); // obst.abs()
    r_d              = obst_dist_to_mid / 10;

    a = r_f - r_d;       // cost of constant part
    r = r_f + r_d;       // radius of constant part
    s = parameter_s * r; // radius of linear decreasing part

    sum += std::max(std::min(1 - ((dist_to_obst_mid - r) / s), 1.0), 0.0) * a;
  }

  return sum;
}


// --- Helper ---

// --- A* State ---

float LPGState::GoalDistanceEstimate(LPGState& nodeGoal)
{
  return static_cast<float>(helper->distance_between_cells(the_cell, nodeGoal.the_cell));
}

bool LPGState::IsGoal(LPGState& nodeGoal)
{
  return IsSameState(nodeGoal);
}

bool LPGState::GetSuccessors(AStarSearch<LPGState> *astarsearch,
                             LPGState* /*parent_node*/)
{

  // add initial nodes
  if(isOrigin()) {

    // TODO: remove magic
    for (int a = 0; a < 16; a++) {
      LPGState new_suc(helper, LPGGrid::Cell(0,a));
      if(!astarsearch->AddSuccessor(new_suc)) {
        return false; 
      }
    }

    return true;
  }

  // calculate neighbors of a regular cell
  for (int r_step = -1; r_step <= 1; r_step++)
  {
    for (int a_step = -1; a_step <= 1; a_step++)
    {
      LPGState new_suc(helper);
      // Continue when same node
      new_suc.the_cell.r = the_cell.r + r_step;
      new_suc.the_cell.a = the_cell.a + a_step;

      if(r_step == 0 && a_step == 0) { 
        continue; 
      }

      if(new_suc.the_cell.r < 0) { 
        continue; 
      }

      // TODO: remove the magic 16
      if(new_suc.the_cell.a == -1) {
        new_suc.the_cell.a = 15;
      } else if(new_suc.the_cell.a == 16) {
        new_suc.the_cell.a = 0;
      }

      // TODO: remove the magic 16, 17
      ASSERT(new_suc.the_cell.a < 16 &&  new_suc.the_cell.a >= 0);

      //new_suc.the_cell.a = new_suc.the_cell.a % 16;

      // Only add if new_suc isn't the same as its parent node
      // or if there isn't a parent node (first nodes that are generated)
      //if (!parent_node || !(parent_node->IsSameState(new_suc)))
      //    && std::abs(new_suc.the_cell.r) < 17
      //    && std::abs(new_suc.the_cell.a) < 17)
      {
        if(!astarsearch->AddSuccessor(new_suc)) { 
          return false; 
        }
      }
    }
  }
  return true;
}

float LPGState::GetCost(LPGState& successor)
{
  // TODO: replace obst_func by a functor
  ASSERT(helper != nullptr);
  return static_cast<float>(helper->distance_between_cells(the_cell, successor.the_cell) + helper->obst_func(successor.the_cell));
}

bool LPGState::IsSameState(LPGState& rhs)
{
  return (the_cell.r == rhs.the_cell.r && the_cell.a == (rhs.the_cell.a % 16));
}
// --- A* ---

// --- LPG PathPlanner ---
LPGPathPlanner::LPGPathPlanner() {
  helper = std::make_shared<LPGGrid>();
}

LPGPathPlanner::~LPGPathPlanner() {}

Vector2d LPGPathPlanner::get_gait(const Vector2d& goal,
                                  const std::vector<Vector3d>& obstacles, DrawingCanvas2D& canvas) const
{
  // Pass the obstacles to LPGGrid before computing waypoints
  helper->set_obstacles(obstacles);
  waypoints = compute_waypoints(goal, canvas);

  waypoint_coordinates = {};
  for (const LPGGrid::Cell& cell : waypoints)
  {
    waypoint_coordinates.push_back(helper->cell_to_coords(cell));
  }

  Vector2d gait;
  if (waypoints.empty())
  {
    return Vector2d(0, 0);
  }
  else if (waypoints.size() > 1)
  {
    gait = helper->cell_to_coords(waypoints[1]);
  }
  else
  {
    gait = helper->cell_to_coords(waypoints[0]);
  }

  // Compute the gait
  /*
  Vector2d gait;
  double distance      = 0;
  unsigned int counter = 0;
  // select the closest wayoint which is at least 6cm away
  do
  {
    if (counter == waypoints.size()) {break;}
    gait     = helper->cell_to_coords(waypoints[counter]);
    distance = gait.abs(); // helper->distance(Vector2d(0, 0), gait); // 
    counter++;
  } while (distance < 60);
  */

  // Dirty: Let the FootStepPlanner actually limit the step to the maximum
  const double max_steplength = 100;
  gait.normalize(max_steplength);

  // If the maximized step would overstep the goal
  if (gait.abs() > goal.abs()) {
    gait = goal;
  }

  return gait;
}

std::vector<LPGGrid::Cell> LPGPathPlanner::compute_waypoints(const Vector2d& goal, DrawingCanvas2D& /*canvas*/) const
{
  std::vector<LPGGrid::Cell> waypoints;

  LPGState startNode(helper, true);
  LPGState endNode(helper, helper->coords_to_cell(goal));

  // TODO: remove the magic 16, 17
  ASSERT(endNode.get_cell().r >= 0 && endNode.get_cell().a >= 0 && endNode.get_cell().a < 16 );

  astar.SetStartAndGoalStates(startNode, endNode);

  unsigned int SearchState;

  int i = 0;
  do
  {
    SearchState = astar.SearchStep();

    /*
    // TODO: add debug request activating debugging
    const LPGState* p = astar.GetOpenListStart();
    canvas.pen("FF0000", 20);
    int k = 0;
    while(p) {
    
      Vector2d m = helper->cell_middle(p->get_cell());
      canvas.drawCircle(m.x, m.y, 20);

      p = astar.GetOpenListNext();
      if (k++ >= 1000) {
        break;
      }
    }
    */

    // Don't search for too long
    // TODO: FIND A GOOD BREAK NUMBER
    //int StepCount = astar.GetStepCount();
    if (i >= 1000)
    {
      astar.CancelSearch();
    }
    i++;
  } while (SearchState == AStarSearch<LPGState>::SEARCH_STATE_SEARCHING);

  // Searching is done, check for the result
  if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_SUCCEEDED)
  {
    LPGState *node = astar.GetSolutionStart();
    // NOTE: don't include the virtual starting node on the the path
    //waypoints.push_back(node->get_cell());

    for (;;)
    {
      node = astar.GetSolutionNext();
      if (!node) {
        break;
      }
      if (!waypoints.empty() && waypoints.back().r == node->get_cell().r && waypoints.back().a == node->get_cell().a) {
        assert(false);
      }
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
  } else {
    ASSERT(false);
  }

  return waypoints;
}
// --- LPG PathPlanner ---
