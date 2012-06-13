/**
* @file PathPlanner.h
* 
* Definition of class AStarSearch
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Yasinovskiy Kirill</a>
*/

#include "PathPlanner.h"


// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"

PathPlanner::PathPlanner()
{
  theAStarSearch = new AStarSearch(getRobotPose(), getFieldInfo(), getPlayerInfo());
  DEBUG_REQUEST_REGISTER("PathPlanner:all_nodes_field","draw all nodes generated by AStarSearch", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:path_on_field_local","draw only the path and nodes that lead to target", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:path_on_field_global","draw only the path and nodes that lead to target", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:draw_obstacles_local", "draw obstacles on field", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:draw_obstacles_global", "draw obstacles on field", false);
}

void PathPlanner::execute()
{
  // create start and goal nodes
  goalPoint = Vector2d(getPath().targetPoint);

  // collect obstacles
  collectObstacles();
//   obstaclesPositions.resize(3);
//   obstaclesPositions[0] = Vector2d(250.0, 0.0);
//   obstaclesPositions[1] = Vector2d(0.0,-250.0);
//   obstaclesPositions[2] = Vector2d(0.0, 250.0);



  AStarNode start(Vector2d(0.0, 0.0));
  AStarNode goal(getPath().targetPoint);
  //AStarNode goal(Vector2d(1200.0, 0.0));
 
  // store the entire path length
  double pathLength;
  // start search
  AStarNode nextPoint = theAStarSearch->search(start, goal, pathLength, obstaclesPositions);
  getPath().nextPointToGo = nextPoint.getPosition();

  // debug requests sections
  DEBUG_REQUEST("PathPlanner:all_nodes_field",
  {
    theAStarSearch->drawAllNodesField();
    // draw next point to go
    PEN(ColorClasses::colorClassToHex(ColorClasses::red), 5);
    CIRCLE(nextPoint.getPosition().x, nextPoint.getPosition().y, 8);
    // draw start point
    PEN(ColorClasses::colorClassToHex(ColorClasses::green), 5);
    CIRCLE(start.getPosition().x, start.getPosition().y, 8);
    // draw goal point
    PEN(ColorClasses::colorClassToHex(ColorClasses::blue), 5);
    CIRCLE(goal.getPosition().x, goal.getPosition().y, 8);
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:path_on_field_local",
  {
    // draw start point
    PEN(ColorClasses::colorClassToHex(ColorClasses::red), 5);
    CIRCLE(nextPoint.getPosition().x, nextPoint.getPosition().y, 8);
    // draw path using AStarSearch function
    theAStarSearch->drawPathFieldLocal();
    // draw goal point
    PEN(ColorClasses::colorClassToHex(ColorClasses::blue), 5);
    CIRCLE(goal.getPosition().x, goal.getPosition().y, 8);
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:path_on_field_global",
  {
    // draw path using AStarSearch function
    theAStarSearch->drawPathFieldGlobal();
    // draw goal point
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:draw_obstacles_local",
  {
    theAStarSearch->drawObstaclesLocal();
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:draw_obstacles_global",
  {
    theAStarSearch->drawObstaclesGlobal();
  }
  );// end DEBUG_REQUEST

}

void PathPlanner::collectObstacles()
{
  // get obstacles from radar grid
  getRadarGrid().getAllObstacles(obstaclesPositions);
  // get playersmodel/percept
  /**currently not in use*/
  // get goal model

}
