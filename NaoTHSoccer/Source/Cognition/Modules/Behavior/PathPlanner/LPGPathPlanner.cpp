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
  the_cell.a = static_cast<int>( angular_part * (coords.angle() / Math::pi2) + 0.5);

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


double LPGGrid::obst_func(const Vector2d& cell_coords) const
{
  Vector2d obst;
  double r_f, dist_to_obst_mid, obst_dist_to_mid, r_d, a, r, s;

  double sum = 0;
  for (const Vector3d& obstacle : obstacles)
  {
    obst             = Vector2d(obstacle.x, obstacle.y);
    r_f              = obstacle.z;
    dist_to_obst_mid = (cell_coords - obst).abs(); // distance(cell_middle(the_cell), obst);
    obst_dist_to_mid = obst.abs(); // distance(obst, Vector2d(0, 0)); // obst.abs()
    r_d              = obst_dist_to_mid / 10.0;

    a = r_f - r_d;       // cost of constant part
    r = r_f + r_d;       // radius of constant part
    s = parameter_s * r; // radius of linear decreasing part

    sum += Math::clamp( 1.0 - (dist_to_obst_mid - r) / s, 0.0, 1.0) * a;
  }

  return sum;
}


// --- Helper ---

// --- A* State ---


bool LPGState::GetSuccessors(AStarSearch<LPGState> *astarsearch,
                             LPGState* /*parent_node*/) const
{

  // add initial nodes
  if(isOrigin()) {

    // TODO: remove magic
    for (int a = 0; a < 16; a++) {
      LPGState new_suc(helper,0,a);
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
      if(r_step == 0 && a_step == 0) { 
        continue; 
      }

      int r = the_cell.r + r_step;
      int a = the_cell.a + a_step;

      if(r < 0) { 
        continue; 
      }

      // TODO: remove the magic 16
      if(a == -1) {
        a = 15;
      } else if(a == 16) {
        a = 0;
      }

      // TODO: remove the magic 16, 17
      ASSERT(a < 16 &&  a >= 0);

      LPGState new_state(helper,r,a);
      if(!astarsearch->AddSuccessor(new_state)) { 
        return false; 
      }
    }
  }
  return true;
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
  compute_waypoints(goal, canvas, waypoints);

  Vector2d gait;
  if (waypoints.empty()) {
    return Vector2d(0, 0);
  } else if (waypoints.size() > 1) {
    gait = waypoints[1];
  } else {
    gait = waypoints[0];
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

void LPGPathPlanner::compute_waypoints(const Vector2d& goal, DrawingCanvas2D& /*canvas*/, std::vector<Vector2d>& waypoints) const
{
  waypoints.clear();

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
    if (i >= 1000) {
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

      //if (!waypoints.empty() && waypoints.back().r == node->get_cell().r && waypoints.back().a == node->get_cell().a) {
      //  assert(false);
      //}
      waypoints.push_back(node->get_coords());
    }

    // Reset A*
    astar.FreeSolutionNodes();
  }
  // This is called when search fails or is canceled or out of memory
  // All nodes will be freed implicitly
  else if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_FAILED) {
    std::cout << "SEARCH FAILED" << std::endl;
  } else if (SearchState == AStarSearch<LPGState>::SEARCH_STATE_OUT_OF_MEMORY) {
    std::cout << "OUT OF MEMORY" << std::endl;
  } else {
    ASSERT(false);
  }
}
// --- LPG PathPlanner ---
