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
  theAStarSearch = new AStarSearch(512, 512);
  DEBUG_REQUEST_REGISTER("PathPlanner:all_nodes_fied","draw all nodes generated by AStarSearch", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:heuristic_values","draw heuristic value for each node", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:cost_values","draw cost value for each node", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:function_values","draw functional value for each node", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:path_on_fied","draw only the path and nodes that lead to goal", false);
  DEBUG_REQUEST_REGISTER("PathPlanner:draw_obstacles", "draw obstacles on field", false);
}

void PathPlanner::execute()
{
  // create start and goal nodes
   goalPoint = Vector2d(getPath().targetPoint);


  AStarNode start(Vector2d(0.0, 0.0));
  AStarNode goal(getPath().targetPoint);
  // get obstacles from radar grid
  getRadarGrid().getAllObstacles(obstaclesPositions);

  // store the entire path length
  double pathLength;
  // start search
  AStarNode nextPoint = theAStarSearch->search(start, goal, pathLength, obstaclesPositions);
  getPath().nextPointToGo = nextPoint.getPosition();

  // debug requests sections
  DEBUG_REQUEST("PathPlanner:all_nodes_fied",
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

  DEBUG_REQUEST("PathPlanner:path_on_fied",
  {
    // draw start point
    PEN(ColorClasses::colorClassToHex(ColorClasses::red), 5);
    CIRCLE(nextPoint.getPosition().x, nextPoint.getPosition().y, 8);
    // draw path using AStarSearch function
    theAStarSearch->drawPathFiled();
    // draw goal point
    PEN(ColorClasses::colorClassToHex(ColorClasses::blue), 5);
    CIRCLE(goal.getPosition().x, goal.getPosition().y, 8);
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:heuristic_values",
  {
    theAStarSearch->drawHeuristic();
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:cost_values",
  {
    theAStarSearch->drawCost();
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:function_values",
  {
    theAStarSearch->drawFunction();
  }
  );// end DEBUG_REQUEST

  DEBUG_REQUEST("PathPlanner:draw_obstacles",
  {
    theAStarSearch->drawObstacles();
  }
  );// end DEBUG_REQUEST

}
